#pragma once

#include "GameObject.h"
#include "FieldCharacter.h"
#include "BattleFieldManager.h"
#include "BattleSceneManager.h"
#include "EnemyTypes.h"
#include "TimeManager.h"
#include "json.hpp"

#include <vector>
#include <fstream> 
#include <iomanip>
#include <algorithm>
#include <iterator>

// 行動の種類を拡張する
enum class AIActionType
{
	None,
	Move,
	Attack,
	UseSkill, // スキル使用も考慮に入れる
	Wait,
};

// プレイヤーの傾向のステート
enum class PlayerTendency
{
	None,	
	Offensive,	// 攻撃的
	Defensive,	// 防御的
	NearDead,	// 瀕死
	Leader,		// リーダー
	MAX,
};

struct LearningAIData
{
	PlayerTendency PlayerTendency = PlayerTendency::None;	//プレイヤーの傾向
	int FocusAliesCharacterID = -1;						//攻撃、スキルの対象となるキャラクターID
	int TurnCount = 0;									//ターン数
};

/// <summary>
/// 攻撃に必要なデータ構造体
/// </summary>
struct EnemyAction
{
	AIActionType ActionType = AIActionType::None;
	int TargetSquareID = -1;		//攻撃、スキルの対象となるマスID
	int TargetCharacterID = -1;	//攻撃、スキルの対象となるキャラクターID
	int MoveSquareID = -1;		//移動先のマスID
};

class EnemyAIManager:public GameComponent
{
private:
	int m_MoveCount = 0;		//移動歩数

	bool m_OnlyOneTime = false;

	bool m_Firsttime = false;

	int m_EnemyIndex = 0;

	int m_MoveAIcount = 0;

	vector<int> m_NextOccupiedPositionList;	//このターンに移動する予定位置のリスト

	float m_DelayCount = 0.0f;

	TimeManager* m_TimeManager;

	EnemyActionType m_EnemyActionType = EnemyActionType::None;
	AIMode Mode = AIMode::decide;

	LearningAIData m_CurrentAIData;	//現在のAIデータ

	nlohmann::json m_JsonData;	//AIデータのJSON

	string m_JsonFilePath = "LearningAIData.json";	//AIデータの保存ファイルパス

	BattleFieldManager* BFMng = nullptr;
	BattleSceneManager* BSMng = nullptr;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	bool CheckEnemyData();

	void ResetAI(Platoon* chara);

	void OnChangeTurn();

	Squares* GetnearCharaPos(float renge, float charaposX, float charaposY);
	float GetMoveAiCount() { return m_MoveAIcount; }
	float GetDelayCount() { return m_DelayCount; }

	//現在のキャラクターが出来る行動を生成
	vector<EnemyAction> GeneratePossibleActions(Platoon* currentCharacter);

	//生成した行動を評価
	float EvaluateAction(Platoon* currentCharacter, const EnemyAction& action);

	//生成された行動から最も評価の高い行動を選択
	EnemyAction SelectBestAction(Platoon* currentEnemy, const std::vector<EnemyAction>& possibleActions);

	//二点からマンハッタン距離を計算
	float CalculateDistance(int currentID, int nextID);

	void WriteJsonFile(const LearningAIData& data)
	{
		nlohmann::json historyData;	//履歴データ

		std::ifstream oldFile(m_JsonFilePath);	//既存のデータを読み込む
		if (oldFile.is_open())
		{
			oldFile >> historyData;
			oldFile.close();
		}

		nlohmann::json jsonData = nlohmann::json	//新しく保存するデータ
		{
			{"turnCount", data.TurnCount},
			{"playerTendency", data.PlayerTendency},
			{"focusAliesCharacterID", data.FocusAliesCharacterID}
		};

		historyData["history"].push_back(jsonData);	//履歴データに追加

		std::ofstream file(m_JsonFilePath);			//データを保存
		if (file.is_open())
		{
			file << std::setw(4) << historyData << std::endl; // インデントを付けて保存
			file.close();
		}
	}

	void ReadJsonFile(LearningAIData& data)
	{
		vector<int> tendencyCount((int)PlayerTendency::MAX, 0);		//プレイヤーの傾向のカウント PlayerTendencyの種類の分を0で初期化
		std::ifstream file(m_JsonFilePath);
		if (!file.is_open())	return;	//ファイルが開けない場合は終了

		nlohmann::json jsonData;
		file >> jsonData;

		if (jsonData.contains("history") && jsonData["history"].is_array())
		{
			for (int i = 0; i < BFMng->GetTurnCount(); i++)
			{
				jsonData["history"][i]["playerTendency"].get_to(tendencyCount[i]); 
			}

			auto maxTendency = std::max_element(tendencyCount.begin(), tendencyCount.end());	//最も多い傾向を見つける

			data.PlayerTendency = static_cast<PlayerTendency>(std::distance(tendencyCount.begin(), maxTendency));	//最も多い傾向をAIデータに設定
			data.FocusAliesCharacterID = jsonData["history"].back()["focusAliesCharacterID"].get<int>();			//最後の行動のfocusAliesCharacterIDをAIデータに設定

			if ((data.PlayerTendency == PlayerTendency::NearDead || data.PlayerTendency == PlayerTendency::Leader) && data.FocusAliesCharacterID == -1)	//瀕死やリーダーの傾向で、特に注目するキャラクターがいない場合は、防御的な傾向に変更する
			{
				data.PlayerTendency = PlayerTendency::Defensive;
			}
		}
	}

	void ClearJsonFile()
	{
		std::ofstream file(m_JsonFilePath);
		if (file.is_open())
		{
			nlohmann::json emptyData = { {"history", nlohmann::json::array()} };	//空の履歴データ
			file << std::setw(4) << emptyData << std::endl;							//インデントを付けて保存
			file.close();
		}
	}

	void CreateLearningData(vector<PlayerActionLog> playerLogList)
	{
		LearningAIData aiData;

		aiData.TurnCount = BFMng->GetTurnCount();

		int offenciveCount = 0;	//攻撃、攻撃系スキル、前進の回数
		int defensiveCount = 0;	//後退、待機、偵察スキルの回数
		int minHPCharacterID = -1;	//HP割合が最も低いキャラクターのID
		float minHPPercentage = 1.0f;	//HP割合の最小値
		int maxDamageCharacterID = -1;	//与えたダメージが最も大きいキャラクターのID
		float maxDamageCharacterDamage = 0.0f;	//そのキャラクターが与えたダメージの最大値

		for (int i = 0; i < playerLogList.size(); i++)
		{
			if (playerLogList[i].m_ActionName == ActionName::Attack)
			{
				offenciveCount++;
			}
			else if (playerLogList[i].m_ActionName == ActionName::Wait)
			{
				defensiveCount++;
			}
			else if (playerLogList[i].m_ActionName == ActionName::ConcentratedFire)
			{
				offenciveCount++;
			}
			else if (playerLogList[i].m_ActionName == ActionName::BayonetCharge)
			{
				offenciveCount++;
			}
			else if (playerLogList[i].m_ActionName == ActionName::Scout)
			{
				defensiveCount++;
			}
			else if (playerLogList[i].m_ActionName == ActionName::Move)
			{
				if (playerLogList[i].m_MoveForward > 0)
				{
					offenciveCount++;
				}
				else if (playerLogList[i].m_MoveForward <= 0)
				{
					defensiveCount++;
				}
			}

			if (playerLogList[i].m_HPparcentage < minHPPercentage)	//最も低い兵力の部隊を更新
			{
				minHPPercentage = playerLogList[i].m_HPparcentage;
				minHPCharacterID = playerLogList[i].m_CharacterID;
			}

			if (playerLogList[i].m_DamageDealt > maxDamageCharacterDamage)	//最も高いダメージを与えた部隊を更新
			{
				maxDamageCharacterDamage = playerLogList[i].m_DamageDealt;
				maxDamageCharacterID = playerLogList[i].m_CharacterID;
			}
		}

		if (minHPPercentage < 0.2f)		//最も兵力の低いキャラクターが2割未満
		{
			aiData.FocusAliesCharacterID = minHPCharacterID;
			aiData.PlayerTendency = PlayerTendency::NearDead;
			WriteJsonFile(aiData);
			return;
		}


		if (offenciveCount >= defensiveCount)	//攻撃的な行動が多い場合
		{
			aiData.FocusAliesCharacterID = -1;
			aiData.PlayerTendency = PlayerTendency::Offensive;

			if (maxDamageCharacterDamage > 30)	//最大ダメージが30を超えている場合、そのキャラクターを最優先で攻撃するリーダー傾向に設定
			{
				aiData.FocusAliesCharacterID = maxDamageCharacterID;
				aiData.PlayerTendency = PlayerTendency::Leader;
			}

			WriteJsonFile(aiData);
			return;
		}
		else									//防御的な行動が多い場合
		{
			aiData.FocusAliesCharacterID = -1;
			aiData.PlayerTendency = PlayerTendency::Defensive;

			WriteJsonFile(aiData);
			return;
		}
	}

	AbilityType SelectAttackAction(Platoon* attackingCharacter, Platoon* attackedCharacter);
};

