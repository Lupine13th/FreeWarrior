#pragma once

#include "GameObject.h"
#include "FieldCharacter.h"
#include "BattleFieldManager.h"
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
	PlayerTendency m_PlayerTendency = PlayerTendency::None;	//プレイヤーの傾向
	int m_FocusAliesCharacterID = -1;						//攻撃、スキルの対象となるキャラクターID
	int m_TurnCount = 0;									//ターン数
};

struct EnemyAction
{
	AIActionType m_ActionType = AIActionType::None;
	int m_TargetSqureaID = -1;		//攻撃、スキルの対象となるマスID
	int m_TargetCharacterID = -1;	//攻撃、スキルの対象となるキャラクターID
	int m_MoveSquareID = -1;		//移動先のマスID
};

class EnemyAIManager:public GameComponent
{
private:
	int m_MoveCount = 0;		//移動歩数

	bool m_OnlyOneTime = false;

	bool m_Firsttime = false;

	int m_EnemyIndex = 0;

	int m_MoveAIcount = 0;

	vector<int> m_NextEnemyPositionList;	//次に攻撃する敵のIDリスト

	float m_DelayCount = 0.0f;

	TimeManager* m_TimeManager;

	EnemyMove Emove = EnemyMove::None;
	AIMode Mode = AIMode::decide;

	LearningAIData m_CurrentAIData;	//現在のAIデータ

	nlohmann::json m_JsonData;	//AIデータのJSON

	string m_JsonFilePath = "LearningAIData.json";	//AIデータの保存ファイルパス

	BattleFieldManager* BFMng = nullptr;
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	bool checkEnemyData();

	void ResetAI(FieldCharacter* chara);

	void OnChangeTurn();

	Squares* GetnearCharaPos(float renge, float charaposX, float charaposY);
	float GetMoveAiCount() { return m_MoveAIcount; }
	float GetDelayCount() { return m_DelayCount; }

	//現在のキャラクターが出来る行動を生成
	vector<EnemyAction> GeneratePossibleActions(FieldCharacter* currentCharacter);

	//生成した行動を評価
	float EvaluateAction(FieldCharacter* currentCharacter, const EnemyAction& action);

	//生成された行動から最も評価の高い行動を選択
	EnemyAction SelectBestAction(FieldCharacter* currentEnemy, const std::vector<EnemyAction>& possibleActions);

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
			{"turnCount", data.m_TurnCount},
			{"playerTendency", data.m_PlayerTendency},
			{"focusAliesCharacterID", data.m_FocusAliesCharacterID}
		};

		historyData["history"].push_back(jsonData);	//履歴データに追加

		std::ofstream file(m_JsonFilePath);			//データを保存
		if (file.is_open())
		{
			file << std::setw(4) << jsonData << std::endl; // インデントを付けて保存
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

			data.m_PlayerTendency = static_cast<PlayerTendency>(std::distance(tendencyCount.begin(), maxTendency));	//最も多い傾向をAIデータに設定
			data.m_FocusAliesCharacterID = jsonData["history"].back()["focusAliesCharacterID"].get<int>();			//最後の行動のfocusAliesCharacterIDをAIデータに設定
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

		aiData.m_TurnCount = BFMng->GetTurnCount();

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
			aiData.m_FocusAliesCharacterID = minHPCharacterID;
			aiData.m_PlayerTendency = PlayerTendency::NearDead;
			WriteJsonFile(aiData);
			return;
		}


		if (offenciveCount >= defensiveCount)	//攻撃的な行動が多い場合
		{
			aiData.m_FocusAliesCharacterID = -1;
			aiData.m_PlayerTendency = PlayerTendency::Offensive;

			if (maxDamageCharacterDamage > 30)	//最大ダメージが30を超えている場合、そのキャラクターを最優先で攻撃するリーダー傾向に設定
			{
				aiData.m_FocusAliesCharacterID = maxDamageCharacterID;
				aiData.m_PlayerTendency = PlayerTendency::Leader;
			}

			WriteJsonFile(aiData);
			return;
		}
		else									//防御的な行動が多い場合
		{
			aiData.m_FocusAliesCharacterID = -1;
			aiData.m_PlayerTendency = PlayerTendency::Defensive;

			WriteJsonFile(aiData);
			return;
		}
	}
};

