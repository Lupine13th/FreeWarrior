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
	Offensive,	//攻撃的
	Defensive,	//防御的
	NearDead,	//瀕死
	Leader,		//リーダー
	OneByOne,	//各個撃破
	PushLine,	//戦線押し
	MAX,
};

struct LearningAIData
{
	int TurnCount = 0;									//ターン数
	map<PlayerTendency, float> PlayerTendencyValue;		//プレイヤー傾向ごとの値
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

	//LearningAIData m_CurrentAIData;	//現在のAIデータ

	PlayerTendency m_CurrentPlayerTendency = PlayerTendency::None;	//現在のプレイヤー傾向

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

	/// <summary>
	/// JSONファイル作成&書き込み
	/// </summary>
	/// <param name="data"></param>
	void WriteJsonFile(const LearningAIData& data)
	{
		nlohmann::json historyData;	//履歴データ

		std::ifstream oldFile(m_JsonFilePath);	//既存のデータを読み込む
		if (oldFile.is_open())					//データが見つかれば
		{
			oldFile >> historyData;				//historyDataに書き込み
			oldFile.close();
		}

		nlohmann::json jsonData = nlohmann::json	//新しく保存するデータ
		{
			{"TurnCount", data.TurnCount},							//現在のターン数
			{"OneByOne", data.PlayerTendencyValue[PlayerTendency::OneByOne]},	//「各個撃破」の値を入力
			{"PushLine", data.PlayerTendencyValue[PlayerTendency::PushLine]}	//「戦線押し」の値を入力
		};

		historyData["history"].push_back(jsonData);	//履歴データに追加

		std::ofstream file(m_JsonFilePath);			//データを保存
		if (file.is_open())
		{
			file << std::setw(4) << historyData << std::endl; // インデントを付けて保存
			file.close();
		}
	}

	//void ReadJsonFile(LearningAIData& data)
	//{
	//	vector<int> tendencyCount((int)PlayerTendency::MAX, 0);		//プレイヤーの傾向のカウント PlayerTendencyの種類の分を0で初期化
	//	std::ifstream file(m_JsonFilePath);
	//	if (!file.is_open())	return;	//ファイルが開けない場合は終了

	//	nlohmann::json jsonData;
	//	file >> jsonData;

	//	if (jsonData.contains("history") && jsonData["history"].is_array())
	//	{
	//		//for (int i = 0; i < BFMng->GetTurnCount(); i++)
	//		//{
	//		//	jsonData["history"][i]["playerTendency"].get_to(tendencyCount[i]); 
	//		//}

	//		//auto maxTendency = std::max_element(tendencyCount.begin(), tendencyCount.end());	//最も多い傾向を見つける

	//		//data.PlayerTendency = static_cast<PlayerTendency>(std::distance(tendencyCount.begin(), maxTendency));	//最も多い傾向をAIデータに設定
	//		//data.FocusAliesCharacterID = jsonData["history"].back()["focusAliesCharacterID"].get<int>();			//最後の行動のfocusAliesCharacterIDをAIデータに設定

	//		//if ((data.PlayerTendency == PlayerTendency::NearDead || data.PlayerTendency == PlayerTendency::Leader) && data.FocusAliesCharacterID == -1)	//瀕死やリーダーの傾向で、特に注目するキャラクターがいない場合は、防御的な傾向に変更する
	//		//{
	//		//	data.PlayerTendency = PlayerTendency::Defensive;
	//		//}

	//		//傾向の値の平均値の1.3倍以上の傾向をプレイヤーの戦術とする
	//	}
	//}

	/// <summary>
	/// JSONファイル読み取り
	/// </summary>
	/// <param name="data"></param>
	PlayerTendency ReadJsonFile()
	{
		LearningAIData data;	//ローカル構造体　ここに書き込んで最終的に返す
		std::ifstream file(m_JsonFilePath);
		if (!file.is_open())	return PlayerTendency::None;	//ファイルが開けない場合は終了

		nlohmann::json jsonData;
		file >> jsonData;

		if (jsonData.contains("history") && jsonData["history"].is_array())
		{
			//for (int i = 0; i < BFMng->GetTurnCount(); i++)
			//{
			//	jsonData["history"][i]["playerTendency"].get_to(tendencyCount[i]); 
			//}

			//auto maxTendency = std::max_element(tendencyCount.begin(), tendencyCount.end());	//最も多い傾向を見つける

			//data.PlayerTendency = static_cast<PlayerTendency>(std::distance(tendencyCount.begin(), maxTendency));	//最も多い傾向をAIデータに設定
			//data.FocusAliesCharacterID = jsonData["history"].back()["focusAliesCharacterID"].get<int>();			//最後の行動のfocusAliesCharacterIDをAIデータに設定

			//if ((data.PlayerTendency == PlayerTendency::NearDead || data.PlayerTendency == PlayerTendency::Leader) && data.FocusAliesCharacterID == -1)	//瀕死やリーダーの傾向で、特に注目するキャラクターがいない場合は、防御的な傾向に変更する
			//{
			//	data.PlayerTendency = PlayerTendency::Defensive;
			//}

			

			float average;	//傾向の平均値
			float max;		//最大値
			PlayerTendency tendency;	//最大の傾向

			data.PlayerTendencyValue[PlayerTendency::OneByOne] = jsonData["history"].back()["OneByOne"];	//「各個撃破」の値をJSONから取得
			data.PlayerTendencyValue[PlayerTendency::PushLine] = jsonData["history"].back()["PushLine"];	//「戦線押し」の値をJSONから取得

			for (const auto& tend : data.PlayerTendencyValue)
			{
				average += tend.second;	//平均値に現在の傾向の値を足す
				if (max < tend.second)	//最大値よりも大きい場合
				{
					max = tend.second;	//最大値に入れる
					tendency = tend.first;	//最大の傾向
				}
			}

			average /= data.PlayerTendencyValue.size();	//平均値を求めるためにタイプの数で割る

			if (average * 1.3f < max)	//傾向の値の平均値の1.3倍以上の傾向をプレイヤーの戦術とする
			{
				return tendency;
			}
			else
			{
				return m_CurrentPlayerTendency;
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

	/// <summary>
	/// 学習データ制作
	/// </summary>
	/// <param name="playerLogList"></param>
	void CreateLearningData(vector<PlayerActionLog> playerLogList)
	{
		LearningAIData aiData;	//新規AIデータ

		aiData.TurnCount = BFMng->GetTurnCount();	//現在のターン数

		map<PlayerTendency, float> tendencyDict = { { PlayerTendency::OneByOne, 0.0f }, { PlayerTendency::PushLine, 0.0f }};	//プレイヤーの行動のそれぞれがどの傾向に近いかをリスト化

		for (const auto& log : playerLogList)	//ログごとに処理
		{
			switch (log.m_ActionName)
			{
			default:
				break;
			case ActionName::Move:		//===============================移動ログ時の処理=================================
				
				//==========「各個撃破」=========

				float lastDistance;		//移動前の距離
				float currentDistance;	//移動後の距離
				Platoon* weakestChara = BFMng->GetWeakenPlatoon(PlatoonListState::Enemy);	//CPUの一番HPの低いキャラを取得
				
				lastDistance = CalculateDistance(weakestChara->GetCharacterPosOnSquares(), log.m_PreviousPosition);		//一番HPが低いキャラと現在のキャラの前の位置を計算
				currentDistance = CalculateDistance(weakestChara->GetCharacterPosOnSquares(), log.m_CurrentPosition);	//一番HPが低いキャラと現在のキャラの今の位置を計算

				if (lastDistance - currentDistance > 0)	//もし接近してきていたら
				{
					tendencyDict[PlayerTendency::OneByOne] += lastDistance - currentDistance;	//接近距離が近いほど「各個撃破」傾向の評価が上がる
				}

				//==========「各個撃破」=========



				//==========「戦線押し」=========

				for (const auto& chara : BFMng->GetAlliesCharacterList())
				{
					int currentLogCharaPosY = BFMng->GetAlliesCharacterList()[log.m_CharacterID]->GetCharacterPosOnSquares() / 10;	//今のログのキャラのY座標

					int positionGap = std::abs(currentLogCharaPosY - chara->GetCharacterPosOnSquares() / 10);

					if (positionGap <= 2)	//他のキャラとのY座標差が2以内
					{
						tendencyDict[PlayerTendency::PushLine] += ((3 - positionGap) * 10);	//近いほど高得点
					}
				}

				//==========「戦線押し」=========

				break;
			case ActionName::Attack:		//===============================攻撃ログ時の処理=================================

				//==========「各個撃破」=========
				
				Platoon* weakestChara = BFMng->GetWeakenPlatoon(PlatoonListState::Enemy);	//CPUの一番HPの低いキャラを取得

				if (BFMng->GetAlliesCharacterList()[log.m_CharacterID] == weakestChara)		//もし最もHPの低いキャラクターが選ばれたならば
				{
					tendencyDict[PlayerTendency::OneByOne] += 20;							//一旦仮に20追加
				}

				//==========「各個撃破」=========



				//==========「戦線押し」=========

				

				//==========「戦線押し」=========

				break;
			case ActionName::Wait:		//===============================攻撃ログ時の処理=================================

				//==========「各個撃破」=========
				
				tendencyDict[PlayerTendency::OneByOne] -= 10;	//待機の場合、「各個撃破」傾向は下がる

				//==========「各個撃破」=========



				//==========「戦線押し」=========

				for (const auto& chara : BFMng->GetAlliesCharacterList())
				{
					int currentLogCharaPosY = BFMng->GetAlliesCharacterList()[log.m_CharacterID]->GetCharacterPosOnSquares() / 10;	//今のログのキャラのY座標

					int positionGap = std::abs(currentLogCharaPosY - chara->GetCharacterPosOnSquares() / 10);

					if (positionGap <= 2)	//他のキャラとのY座標差が2以内
					{
						tendencyDict[PlayerTendency::PushLine] += ((3 - positionGap) * 15);	//近いほど高得点　移動時よりも高得点になりやすい
					}
				}

				//==========「戦線押し」=========

				break;
			case ActionName::BayonetCharge:		//===============================特殊攻撃系ログ時の処理=================================
			case ActionName::ConcentratedFire:

				//==========「各個撃破」=========

				Platoon* weakestChara = BFMng->GetWeakenPlatoon(PlatoonListState::Enemy);	//CPUの一番HPの低いキャラを取得

				if (BFMng->GetAlliesCharacterList()[log.m_CharacterID] == weakestChara)		//もし最もHPの低いキャラクターが選ばれたならば
				{
					tendencyDict[PlayerTendency::OneByOne] += 25;							//一旦仮に20追加
				}

				//==========「各個撃破」=========



				//==========「戦線押し」=========



				//==========「戦線押し」=========

				break;

			case ActionName::Scout:					//===============================偵察ログ時の処理=================================

				//==========「各個撃破」=========

				tendencyDict[PlayerTendency::OneByOne] += 5;							//一旦仮に15追加

				//==========「各個撃破」=========



				//==========「戦線押し」=========

				tendencyDict[PlayerTendency::PushLine] += 15;							//一旦仮に15追加

				//==========「戦線押し」=========

				break;
			}
		}

		//int offenciveCount = 0;	//攻撃、攻撃系スキル、前進の回数
		//int defensiveCount = 0;	//後退、待機、偵察スキルの回数
		//int minHPCharacterID = -1;	//HP割合が最も低いキャラクターのID
		//float minHPPercentage = 1.0f;	//HP割合の最小値
		//int maxDamageCharacterID = -1;	//与えたダメージが最も大きいキャラクターのID
		//float maxDamageCharacterDamage = 0.0f;	//そのキャラクターが与えたダメージの最大値

		//for (int i = 0; i < playerLogList.size(); i++)
		//{
		//	if (playerLogList[i].m_ActionName == ActionName::Attack)
		//	{
		//		offenciveCount++;
		//	}
		//	else if (playerLogList[i].m_ActionName == ActionName::Wait)
		//	{
		//		defensiveCount++;
		//	}
		//	else if (playerLogList[i].m_ActionName == ActionName::ConcentratedFire)
		//	{
		//		offenciveCount++;
		//	}
		//	else if (playerLogList[i].m_ActionName == ActionName::BayonetCharge)
		//	{
		//		offenciveCount++;
		//	}
		//	else if (playerLogList[i].m_ActionName == ActionName::Scout)
		//	{
		//		defensiveCount++;
		//	}
		//	else if (playerLogList[i].m_ActionName == ActionName::Move)
		//	{
		//		if (playerLogList[i].m_MoveForward > 0)
		//		{
		//			offenciveCount++;
		//		}
		//		else if (playerLogList[i].m_MoveForward <= 0)
		//		{
		//			defensiveCount++;
		//		}
		//	}

		//	if (playerLogList[i].m_HPparcentage < minHPPercentage)	//最も低い兵力の部隊を更新
		//	{
		//		minHPPercentage = playerLogList[i].m_HPparcentage;
		//		minHPCharacterID = playerLogList[i].m_CharacterID;
		//	}

		//	if (playerLogList[i].m_DamageDealt > maxDamageCharacterDamage)	//最も高いダメージを与えた部隊を更新
		//	{
		//		maxDamageCharacterDamage = playerLogList[i].m_DamageDealt;
		//		maxDamageCharacterID = playerLogList[i].m_CharacterID;
		//	}
		//}

		//if (minHPPercentage < 0.2f)		//最も兵力の低いキャラクターが2割未満
		//{
		//	aiData.FocusAliesCharacterID = minHPCharacterID;
		//	aiData.PlayerTendency = PlayerTendency::NearDead;
		//	WriteJsonFile(aiData);
		//	return;
		//}


		//if (offenciveCount >= defensiveCount)	//攻撃的な行動が多い場合
		//{
		//	aiData.FocusAliesCharacterID = -1;
		//	aiData.PlayerTendency = PlayerTendency::Offensive;

		//	if (maxDamageCharacterDamage > 30)	//最大ダメージが30を超えている場合、そのキャラクターを最優先で攻撃するリーダー傾向に設定
		//	{
		//		aiData.FocusAliesCharacterID = maxDamageCharacterID;
		//		aiData.PlayerTendency = PlayerTendency::Leader;
		//	}

		//	WriteJsonFile(aiData);
		//	return;
		//}
		//else									//防御的な行動が多い場合
		//{
		//	aiData.FocusAliesCharacterID = -1;
		//	aiData.PlayerTendency = PlayerTendency::Defensive;

		//	WriteJsonFile(aiData);
		//	return;
		//}
	}

	/// <summary>
	/// 攻撃アクションを選択　士気の値によって行動を使うかを判定する
	/// </summary>
	/// <param name="attackingCharacter">攻撃側</param>
	/// <param name="attackedCharacter">防御側</param>
	/// <returns></returns>
	AbilityType SelectAttackAction(Platoon* attackingCharacter, Platoon* attackedCharacter);
};

