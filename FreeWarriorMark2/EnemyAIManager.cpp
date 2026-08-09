#include "EnemyAIManager.h"
#include "BattleFieldManager.h"
#include "FieldCharacter.h"
#include "Admin.h"
#include "SoundManager.h"
#include "FlyingCameraController.h"
#include "Abilities.h"

#include <MyAccessHub.h>
#include <vector>
#include <string>
#include <chrono>
#include <climits>
#include <windows.h>
#include <cstdio>
#include <queue>
#include <map>

void EnemyAIManager::InitAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	BFMng = MyAccessHub::GetBFManager();
	BSMng = MyAccessHub::GetBattleSceneManager();
}

bool EnemyAIManager::FrameAction()
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	MyGameEngine* p_engine = MyAccessHub::GetMyGameEngine();

	FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();

	//==============================================
	//==========敵の行動パターンを各自設定==========
	//==============================================

	if (BFMng->GetCurrentTurn() == Turn::Enemy)	
	{
		//初回１フレームの処理
		if (!m_Firsttime)
		{
			p_engine->GetSoundManager()->playBGM(1);	//敵ターンBGM再生
			ReadJsonFile(m_CurrentAIData);
			m_NextOccupiedPositionList.clear();	//移動予定先リストをクリア
			for (int i = 0; i < BFMng->GetAlliesCharacterList().size(); i++)	
			{
				if (!BFMng->GetAlliesCharacterList()[i]->GetIsDead())
				{
					m_NextOccupiedPositionList.push_back(BFMng->GetAlliesCharacterList()[i]->GetCharacterPosOnSquares());	//移動予定先リストにプレイヤーユニットの位置を入れる(位置が被らないように)
				}
			}
			m_Firsttime = true;	//初回処理終了
		}

		if (!BFMng->GetEnemyCharacterList().empty())
		{
			for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)
			{
				Platoon* currentEnemy = BFMng->GetEnemyCharacterList()[i];	//インデックス数で敵情報を取得

				if (!currentEnemy->GetIsDead())
				{
					//AI行動の生成
					vector<EnemyAction> possibleActions = GeneratePossibleActions(currentEnemy);	

					//生成した行動の評価
					EnemyAction bestAction = SelectBestAction(currentEnemy, possibleActions);

					//最も評価の高い行動の行動タイプによって行動を確定
					switch (bestAction.m_ActionType)
					{
						case AIActionType::Wait:
							currentEnemy->SetEnemyActionType(EnemyActionType::Wait);
							currentEnemy->SetTargetAISquare(nullptr);	//待機のため標的は空
							currentEnemy->SetTargetAICharacterID(-1);	//上に同じく
							currentEnemy->SetMoveAISquareID(-1);		//移動先も空
							break;
						case AIActionType::Attack:
							currentEnemy->SetEnemyActionType(EnemyActionType::Attack);
							currentEnemy->SetTargetAISquare(BFMng->GetFieldSquaresList()[bestAction.m_TargetSqureaID]);	//攻撃する敵の位置を取得
							currentEnemy->SetTargetAICharacterID(bestAction.m_TargetCharacterID);						//攻撃する敵のIDを取得
							currentEnemy->SetMoveAISquareID(-1);														//移動先は空
							break;
						case AIActionType::Move:
							currentEnemy->SetEnemyActionType(EnemyActionType::Move);
							currentEnemy->SetTargetAISquare(nullptr);						//攻撃位置は空
							currentEnemy->SetMoveAISquareID(bestAction.m_MoveSquareID);		//対象も空
							m_NextOccupiedPositionList.push_back(bestAction.m_MoveSquareID);//このユニットの移動予定位置を移動予定先リストに登録
							break;
					}
				}
			}
		}

		int moveCount = 0;
		int deadCount = 0;

		for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)		//現在行動決定したキャラクターの数をカウント
		{
			if (BFMng->GetEnemyCharacterList()[i]->GetEnemyActionType() != EnemyActionType::None && !BFMng->GetEnemyCharacterList()[i]->GetIsDead())
			{
				moveCount++;	//行動した敵の数を加算
			}
			if (BFMng->GetEnemyCharacterList()[i]->GetIsDead())
			{
				deadCount++;	//壊滅した部隊の数を加算
				ResetAI(BFMng->GetEnemyCharacterList()[i]);
			}
		}

		if (moveCount == BFMng->GetEnemyCharacterList().size() - deadCount && BFMng->GetCurrentTurn() == Turn::Enemy)	//全キャラクターの行動が決定したらEnemyMoveへ
		{
			BFMng->SetCurrentTurn(Turn::EnemyAction);
		}
		
	}

	//================================
	//==========敵の行動実行==========
	//================================

	Platoon* currentEnemy = BFMng->GetEnemyCharacterList()[m_MoveAIcount];

	if (BFMng->GetCurrentTurn() == Turn::EnemyAction && m_DelayCount == 0.0f)	//アニメーション設定
	{

		if (currentEnemy->GetIsDead())	//もし死んでいたらスキップ
		{
			ResetAI(currentEnemy);
			m_DelayCount = 0.0f;
			if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
			{
				m_MoveAIcount++;
			}
			if (m_MoveAIcount >= BFMng->GetEnemyCharacterList().size())
			{
				BFMng->ChangeTurn();
				p_engine->GetSoundManager()->playBGM(0);
			}
			return true;
		}
		switch (currentEnemy->GetEnemyActionType())	//アニメーションの設定
		{
		default:
			break;
		case EnemyActionType::Attack:
			BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetFBXData()->SetAnimeInit(L"ATTACK", BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon());
			break;
		case EnemyActionType::Move:
			BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetFBXData()->SetAnimeInit(L"WALK", BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon());
			break;
		case EnemyActionType::None:
		case EnemyActionType::Wait:
			break;
		}

		Fcam->ChangeCameraPosition();
	}

	if (BFMng->GetCurrentTurn() == Turn::EnemyAction && m_DelayCount < 1.5f)	//ディレイ時間
	{
		m_DelayCount += m_TimeManager->GetDeltaTime();
	}
	else if (BFMng->GetCurrentTurn() == Turn::EnemyAction && m_DelayCount > 1.5f && m_DelayCount < 4.0f)
	{
		if (!m_OnlyOneTime)
		{
			int NowPos = -1;
			int NextPos = -1;
			switch (currentEnemy->GetEnemyActionType())
			{
			case EnemyActionType::Attack:	//攻撃処理
				if (BFMng->GetFieldSquaresList()[currentEnemy->GetTargetAISquare()->GetSquareID()]->chara != nullptr)
				{
					BFMng->SetAttackingCharacterSquares(BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]);

					switch (SelectAttackAction(currentEnemy, BFMng->GetAlliesCharacterList()[currentEnemy->GetAITargetCharacterID()]))
					{
						case AbilityType::None:
							BFMng->Attack(BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());											//攻撃処理
							BSMng->SetBattlePosition(Animations::Attack, BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());			//バトルアニメーション
							break;
						case AbilityType::ConcentratedFire:
							BFMng->GetAbilities()->ConcentratedFire(BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());					//攻撃処理
							BSMng->SetBattlePosition(Animations::ConcentratedFire, BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());	//バトルアニメーション
							break;
						case AbilityType::BayonetCharge:
							BFMng->GetAbilities()->BayonetCharge(BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());					//攻撃処理
							BSMng->SetBattlePosition(Animations::BayonetCharge, BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());		//バトルアニメーション
							break;
						case AbilityType::Scout:
							BFMng->GetAbilities()->Scout(BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());							//攻撃処理
							BSMng->SetBattlePosition(Animations::Scout, BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetPlatoon(), currentEnemy->GetTargetAISquare()->GetPlatoon());				//バトルアニメーション
							break;
					}

				}
				m_OnlyOneTime = true;
				break;
			case EnemyActionType::Move:	//移動処理
				NowPos = currentEnemy->GetCharacterPosOnSquares();
				NextPos = currentEnemy->GetAIMoveSquareID();
				BSMng->SetMovePosition(BFMng->GetFieldSquaresList()[NowPos], BFMng->GetFieldSquaresList()[NextPos], BFMng->GetFieldSquaresList()[currentEnemy->GetCharacterPosOnSquares()]->GetFBXData());
				BFMng->Move(NowPos, NextPos, currentEnemy->GetPlatoonID());
				m_OnlyOneTime = true;
				break;
			case EnemyActionType::Wait:	//待機処理
				BFMng->Wait(currentEnemy->GetCharacterPosOnSquares());
				m_DelayCount = 0.0f;
				if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
				{
					m_MoveAIcount++;
				}
				if (m_MoveAIcount == BFMng->GetEnemyCharacterList().size() && BFMng->GetCurrentTurn() == Turn::EnemyAction)
				{
					BFMng->ChangeTurn();
					p_engine->GetSoundManager()->playBGM(0);
				}
				return true;
				break;
			case EnemyActionType::None:
				m_DelayCount = 0.0f;
				if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
				{
					m_MoveAIcount++;
				}
				if (m_MoveAIcount == BFMng->GetEnemyCharacterList().size() && BFMng->GetCurrentTurn() == Turn::EnemyAction)
				{
					BFMng->ChangeTurn();
					p_engine->GetSoundManager()->playBGM(0);
				}
				return true;
				break;
			default:
				break;
			}
			Fcam->ChangeCameraPosition();
		}
		m_DelayCount += m_TimeManager->GetDeltaTime();
	}
	else if (BFMng->GetCurrentTurn() == Turn::EnemyAction && m_DelayCount > 4.0f)	//行動終了処理
	{
		BFMng->CheckMoved();
		if (currentEnemy->GetEnemyActionType() == EnemyActionType::Attack && BFMng->GetFieldSquaresList()[currentEnemy->GetTargetAISquare()->GetSquareID()]->chara != nullptr)	//攻撃した相手がまだいる場合は、死亡していないか確認
		{
			BFMng->CheckDead(BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[currentEnemy->GetTargetAISquare()->GetSquareID()]->ThereCharaID]);
		}
		ResetAI(currentEnemy);
		m_DelayCount = 0.0f;
		m_OnlyOneTime = false;
		if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
		{
			m_MoveAIcount++;
		}
	}

	if (m_MoveAIcount == BFMng->GetEnemyCharacterList().size() && BFMng->GetCurrentTurn() == Turn::EnemyAction)	//全キャラクターの行動が終了したらターン交代
	{
		BFMng->ChangeTurn();
		p_engine->GetSoundManager()->playBGM(0);
	}
	return true;
}

void EnemyAIManager::FinishAction()
{
}

bool EnemyAIManager::CheckEnemyData()
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	int movedcount = 0;
	for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)
	{
		if (BFMng->GetEnemyCharacterList()[i]->GetIsActioned())
		{
			movedcount++;
		}
	}

	if (movedcount == BFMng->GetEnemyCharacterList().size())
	{
		BFMng->SetCurrentTurn(Turn::Allies);
		return false;
	}
	else
	{
		return true;
	}
}


Squares* EnemyAIManager::GetnearCharaPos(float renge, float charaposX, float charaposY)	//近くのキャラクターの位置を取得
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	vector<float> distance;
	vector<float> index;
	int minDistanceIndex = -1;
	float minDistance = FLT_MAX;

	for (int x = -renge; x <= renge; x++)
	{
		for (int y = -renge; y <= renge; y++)
		{
			int charaPosition = charaposX + (charaposY * 10);
			int nx = BFMng->GetFieldSquaresList()[charaposX + charaposY * 10]->charaPosX + x;
			int ny = BFMng->GetFieldSquaresList()[charaposX + charaposY * 10]->charaPosY + y;

			if (std::abs(x) + std::abs(y) <= renge &&	//マンハッタン距離での範囲計算
				charaPosition + x + (y * 10) >= 0 &&
				charaPosition + x + (y * 10) <= 149 &&		//座標の範囲内である
				nx >= 0 && nx <= 9 &&
				ny >= 0 && ny <= 14)					//上下左右の限界突破をしていない
			{
				int serchingpos = (x + charaposX) + ((y + charaposY) * 10);
				if (BFMng->GetFieldSquaresList()[serchingpos]->chara != nullptr)
				{
					if (BFMng->GetFieldSquaresList()[serchingpos]->chara->GetAdmin() == Admin::Rebel)
					{
						distance.push_back(std::abs(x) + std::abs(y));
						index.push_back(BFMng->GetFieldSquaresList()[serchingpos]->chara->GetCharacterPosOnSquares());
					}
				}
			}
		}
	}

	if (distance.size() != 0)
	{
		for (int i = 0; i < distance.size(); i++)
		{
			if (minDistance > distance[i])
			{
				minDistance = distance[i];
				minDistanceIndex = index[i];
			}
		}
		return BFMng->GetFieldSquaresList()[minDistanceIndex];
	}
	else
	{
		return nullptr;
	}
}

vector<EnemyAction> EnemyAIManager::GeneratePossibleActions(Platoon* currentCharacter)
{
	vector<EnemyAction> actionList;	//生成された可能な行動
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	int fieldWidth = 10;
	int fieldHeight = 15;

	//待機アクションは常に可能
	EnemyAction waitAction;
	waitAction.m_ActionType = AIActionType::Wait;
	actionList.push_back(waitAction);

	//攻撃アクションの生成
	Squares* targetPosition = GetnearCharaPos(currentCharacter->GetAttackRenge(), BFMng->GetFieldSquaresList()[currentCharacter->GetCharacterPosOnSquares()]->charaPosX, BFMng->GetFieldSquaresList()[currentCharacter->GetCharacterPosOnSquares()]->charaPosY);
	if (targetPosition != nullptr)
	{
		EnemyAction attackAction;
		attackAction.m_ActionType = AIActionType::Attack;
		attackAction.m_TargetSqureaID = targetPosition->GetSquareID();
		attackAction.m_TargetCharacterID = targetPosition->chara->GetPlatoonID();

		actionList.push_back(attackAction);
	}

	//最も近いプレイヤーキャラクターの位置を取得
	currentCharacter->SetNearestEnemySquare(GetnearCharaPos(15.0f, BFMng->GetFieldSquaresList()[currentCharacter->GetCharacterPosOnSquares()]->charaPosX, BFMng->GetFieldSquaresList()[currentCharacter->GetCharacterPosOnSquares()]->charaPosY));

	//プレイヤーの傾向が瀕死、もしくはリーダーであれば、最も近いプレイヤーキャラクターの位置を最優先で設定
	if (m_CurrentAIData.m_PlayerTendency == PlayerTendency::NearDead || m_CurrentAIData.m_PlayerTendency == PlayerTendency::Leader)
	{
		currentCharacter->SetNearestEnemySquare(BFMng->GetFieldSquaresList()[m_CurrentAIData.m_FocusAliesCharacterID]);
	}

	//BFS(幅優先探索)による移動可能なマスの探索
	queue<pair<int, int>> bfsQueue;	//探索用のキュー（マスIDと移動コストのペア）
	map<int, int> visited;			//通ったことのあるマス（マスIDと移動コストのペア）
	int startSquareID = currentCharacter->GetCharacterPosOnSquares();

	bfsQueue.push({ startSquareID, 0 });
	visited[startSquareID] = 0;

	int dx[] = { 0, 0, -1, 1 };		//上下左右
	int dy[] = { 1, -1, 0, 0 };		//上下左右

	//移動可能なマスを探索
	while (!bfsQueue.empty())
	{
		pair<int, int> currentPosition = bfsQueue.front();
		bfsQueue.pop();

		int currentSquareID = currentPosition.first;
		int currentCost = currentPosition.second;

		auto sqData = BFMng->GetFieldSquaresList()[currentSquareID];
		bool isOccupied = (sqData->chara != nullptr && sqData->chara != currentCharacter);	//マスにキャラクターがいるか

		if (!isOccupied)
		{
			if (currentSquareID != startSquareID)	//移動アクション(開始地点以外)
			{
				EnemyAction moveAction;
				moveAction.m_ActionType = AIActionType::Move;
				moveAction.m_MoveSquareID = currentSquareID;
				actionList.push_back(moveAction);
			}
		}

		//移動コストが機動力に収まる範囲であれば隣接マスを探索
		if (currentCost < currentCharacter->GetMoveRenge())
		{
			int currentPostionX = BFMng->GetFieldSquaresList()[currentSquareID]->charaPosX;
			int currentPostionY = BFMng->GetFieldSquaresList()[currentSquareID]->charaPosY;

			for (int i = 0; i < 4; i++)
			{
				int nextPositionX = currentPostionX + dx[i];
				int nextPositionY = currentPostionY + dy[i];

				if (nextPositionX >= 0 && nextPositionX < fieldWidth && nextPositionY >= 0 && nextPositionY < fieldHeight)
				{
					int nextSquareID = nextPositionX + (nextPositionY * fieldWidth);

					//次のマスが移動可能か（空いている、もしくは自分のいるマスである）
					bool isMovable = (BFMng->GetFieldSquaresList()[nextSquareID]->chara == nullptr || BFMng->GetFieldSquaresList()[nextSquareID]->chara == currentCharacter);

					//既にvisitedにない、もしくはより低いコストで訪れた場合はキューに追加
					if (isMovable && (visited.find(nextSquareID) == visited.end()))
					{
						visited[nextSquareID] = currentCost + 1;
						bfsQueue.push({ nextSquareID, currentCost + 1 });
					}
				}
			}
		}
	}

	return actionList;
}

float EnemyAIManager::EvaluateAction(Platoon* currentCharacter, const EnemyAction& action)
{
	float score = 0.0f;
	float distance = 0.0f;
	Squares* targetSquare = nullptr;
	vector<int> isOccupied;	//移動先に敵がいるかどうかのフラグ
	switch (action.m_ActionType)
	{
	case AIActionType::Attack:
		// 攻撃行動 
		
		if (action.m_TargetCharacterID != -1)
		{
			switch (m_CurrentAIData.m_PlayerTendency)
			{
			case PlayerTendency::Leader:	//リーダーを優先して攻撃するAI
				if (action.m_TargetCharacterID == m_CurrentAIData.m_FocusAliesCharacterID)
				{
					score += 2000.0f;
				}
				else
				{
					score += 1500.0f;
				}
				break;
			case PlayerTendency::NearDead:	//瀕死のキャラクターを優先して攻撃するAI
				if (action.m_TargetCharacterID == m_CurrentAIData.m_FocusAliesCharacterID)
				{
					score += 2000.0f;
				}
				else
				{
					score += 1500.0f;
				}
				break;
			case PlayerTendency::Offensive:	//攻撃的なプレイヤーに対しては攻撃行動の評価が低くなる
				score += 1200.0f;
				break;
			default:
				score += 1500.0f;
				break;
			}
		}
		break;
	case AIActionType::Move:
	{
		targetSquare = GetnearCharaPos(currentCharacter->GetAttackRenge(), action.m_MoveSquareID % 10, action.m_MoveSquareID / 10);	//移動先からの攻撃範囲に敵がいるかどうか

		if (targetSquare == nullptr)	//移動先からの攻撃範囲で敵がいない場合
		{
			distance = CalculateDistance(action.m_MoveSquareID, currentCharacter->GetNearestEnemySquare()->GetSquareID());
			//部隊の兵数が少なければ敵から距離を取るようにする　多ければ近づくようにする
			if (currentCharacter->GetSoldiers() / currentCharacter->GetMaxSoldiers() > 0.3f)
			{
				score += 1000.0f - distance * 5.0f;
			}
			else
			{
				score += distance * 10.0f;
			}
		}
		else							//移動先からの攻撃範囲に敵がいる場合は一番遠いマスの評価が上がる
		{
			distance = CalculateDistance(action.m_MoveSquareID, targetSquare->GetSquareID());

			score += 1000.0f + distance * 5.0f;
		}

		if (BFMng->GetFieldSquaresList()[action.m_MoveSquareID]->terrainname != Terrain::Plane && BFMng->GetFieldSquaresList()[currentCharacter->GetCharacterPosOnSquares()]->terrainname == Terrain::Plane)
		{
			switch (m_CurrentAIData.m_PlayerTendency)
			{
				case PlayerTendency::Offensive:	//攻撃的なプレイヤーに対しては平地以外のマスの評価が上がる
					score += 300.0f;
					break;
			}
		}

		if (!m_NextOccupiedPositionList.empty())
		{
			auto isOccupied = std::find(m_NextOccupiedPositionList.begin(), m_NextOccupiedPositionList.end(), action.m_MoveSquareID);

			if (isOccupied != m_NextOccupiedPositionList.end())
			{
				score = 0.0f;			//他の敵が移動する予定のマスは選ばれないようにする
			}
		}
		
		break;
	}
	case AIActionType::Wait:
		score += 1.0f; // 待機は最も低く評価
		break;
	default:
		break;
	}
	return score;
}

EnemyAction EnemyAIManager::SelectBestAction(Platoon* currentEnemy, const std::vector<EnemyAction>& possibleActions)
{
	if (possibleActions.empty())
	{
		return EnemyAction(); // 行動不可
	}

	float bestScore = -FLT_MAX;
	EnemyAction bestAction = possibleActions[0];

	vector<float> scoreList;	//各行動のスコアのリスト

	for (const auto& action : possibleActions)
	{
		float currentScore = EvaluateAction(currentEnemy, action);
		scoreList.push_back(currentScore);

		if (currentScore > bestScore)
		{
			bestScore = currentScore;
			bestAction = action;
		}
	}

	return bestAction;
}

float EnemyAIManager::CalculateDistance(int currentID, int nextID)
{
	int currentX = currentID % 10;
	int currentY = currentID / 10;
	int nextX = nextID % 10;
	int nextY = nextID / 10;

	return std::abs(currentX - nextX) + std::abs(currentY - nextY);
}

/// <summary>
/// 攻撃アクションを選択　士気の値によって行動を使うかを判定する
/// </summary>
/// <param name="attackingCharacter">攻撃側</param>
/// <param name="attackedCharacter">防御側</param>
/// <returns></returns>
AbilityType EnemyAIManager::SelectAttackAction(Platoon* attackingCharacter, Platoon* attackedCharacter)
{
	struct AbilityScore
	{
		AbilityType type;
		float score;
	};

	std::vector<AbilityType> currentCharacterAbilities = attackingCharacter->GetAbilityList();	//攻撃キャラの行動種類を取得

	std::vector<AbilityScore> abilityScores;	//それぞれの行動種類のスコア

	float maxScore = 0.0f;	//現在の最大スコア

	AbilityType bestAbility = AbilityType::None;

	Abilities* Abilities = BFMng->GetAbilities();

	for (int i = 0; i < 3; i++)
	{
		float score = 0.0f;

		switch (currentCharacterAbilities[i])
		{
			case AbilityType::ConcentratedFire:
				if (attackingCharacter->GetMorale() > 20.0f)
				{
					score += 100.0f;

					if (attackedCharacter->GetSoldiersPercent() < 0.3f)
					{
						score += 300.0f;
					}

					abilityScores.push_back({ AbilityType::ConcentratedFire, score });
				}
				break;
			case AbilityType::BayonetCharge:
				if (attackingCharacter->GetMorale() > 10.0f && attackingCharacter->GetSoldiersPercent() > 0.3f)
				{
					score += 300.0f;

					abilityScores.push_back({ AbilityType::ConcentratedFire, score });
				}
				break;
			case AbilityType::Scout:
				if (!attackedCharacter->GetIsDetected())
				{
					score += 500.0f;

					abilityScores.push_back({ AbilityType::Scout, score });
				}
				break;
		}
	}

	for (int i = 0; i < abilityScores.size(); i++)
	{
		if (abilityScores[i].score > maxScore)
		{
			maxScore = abilityScores[i].score;
			bestAbility = abilityScores[i].type;
		}
	}

	switch (bestAbility)
	{
		case AbilityType::ConcentratedFire:
			Abilities->ConcentratedFire(attackingCharacter, attackedCharacter);
			return bestAbility;
		case AbilityType::BayonetCharge:
			Abilities->BayonetCharge(attackingCharacter, attackedCharacter);
			return bestAbility;
		case AbilityType::Scout:
			Abilities->Scout(attackingCharacter, attackedCharacter);
			return bestAbility;
	}

	return bestAbility;
}

void EnemyAIManager::ResetAI(Platoon* chara)
{
	chara->SetEnemyActionType(EnemyActionType::None);
	chara->SetTargetAISquare(nullptr);
}

void EnemyAIManager::OnChangeTurn()
{
	m_Firsttime = false;
	m_MoveAIcount = 0;
}
