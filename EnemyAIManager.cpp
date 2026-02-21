#include "EnemyAIManager.h"
#include "BattleFieldManager.h"
#include "FieldCharacter.h"
#include "Admin.h"
#include "SoundManager.h"
#include "FlyingCameraController.h"

#include <MyAccessHub.h>
#include <vector>
#include <string>
#include <chrono>
#include <climits>
#include <windows.h>
#include <cstdio>
#include <queue>
#include <map>

#define DEBUG_FLOAT(val) { char buf[128]; sprintf_s(buf, "%f\n", val); OutputDebugStringA(buf); }

void EnemyAIManager::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
}

bool EnemyAIManager::frameAction()
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	MyGameEngine* p_engine = MyAccessHub::getMyGameEngine();

	FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();

	//==========敵の行動パターンを各自設定==========

	if (BFMng->GetCurrentTurn() == Turn::Enemy)	
	{
		if (!m_Firsttime)
		{
			p_engine->GetSoundManager()->playBGM(1);
			m_Firsttime = true;
		}
		if (!BFMng->GetEnemyCharacterList().empty())
		{
			for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)
			{
				FieldCharacter* currentEnemy = BFMng->GetEnemyCharacterList()[i];

				if (!currentEnemy->Dead)
				{
					//AI行動の生成
					vector<EnemyAction> possibleActions = GeneratePossibleActions(currentEnemy);	

					//生成した行動の評価
					EnemyAction bestAction = SelectBestAction(currentEnemy, possibleActions);

					//最も評価の高い行動を設定
					switch (bestAction.m_ActionType)
					{
						case AIActionType::Wait:
							currentEnemy->AIMove = EnemyMove::Wait;
							currentEnemy->targetAISquare = nullptr;
							currentEnemy->targetAICharacterID = -1;
							currentEnemy->moveAISquareID = -1;
							break;
						case AIActionType::Attack:
							currentEnemy->AIMove = EnemyMove::Attack;
							currentEnemy->targetAISquare = BFMng->GetFieldSquaresList()[bestAction.m_TargetSqureaID];
							currentEnemy->targetAICharacterID = bestAction.m_TargetCharacterID;
							currentEnemy->moveAISquareID = -1;
							break;
						case AIActionType::Move:
							currentEnemy->AIMove = EnemyMove::Move;
							currentEnemy->targetAISquare = nullptr;
							currentEnemy->moveAISquareID = bestAction.m_MoveSquareID;
							break;
					}
				}
			}
		}

		int movecount = 0;
		int deadcount = 0;

		for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)		//現在行動決定したキャラクターの数をカウント
		{
			if (BFMng->GetEnemyCharacterList()[i]->AIMove != EnemyMove::None && !BFMng->GetEnemyCharacterList()[i]->Dead)
			{
				movecount++;
			}
			if (BFMng->GetEnemyCharacterList()[i]->Dead)
			{
				deadcount++;
				ResetAI(BFMng->GetEnemyCharacterList()[i]);
			}
		}

		if (movecount == BFMng->GetEnemyCharacterList().size() - deadcount && BFMng->GetCurrentTurn() == Turn::Enemy)	//全キャラクターの行動が決定したらEnemyMoveへ
		{
			BFMng->SetCurrentTurn(Turn::EnemyMove);
		}
		
	}


	//==========敵の行動実行==========

	FieldCharacter* currentEnemy = BFMng->GetEnemyCharacterList()[m_MoveAIcount];

	if (BFMng->GetCurrentTurn() == Turn::EnemyMove && m_DelayCount == 0.0f)	//アニメーション設定
	{

		if (currentEnemy->Dead)	//もし死んでいたらスキップ
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
		switch (currentEnemy->AIMove)	//アニメーションの設定
		{
		default:
			break;
		case EnemyMove::Attack:
			BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->fbxD->SetAnime(L"ATTACK01");
			break;
		case EnemyMove::Move:
			BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->fbxD->SetAnime(L"WALK01");
			break;
		case EnemyMove::None:
		case EnemyMove::Wait:
			break;
		}

		Fcam->ChangeCameraPosition();
	}

	if (BFMng->GetCurrentTurn() == Turn::EnemyMove && m_DelayCount < 1.5f)	//ディレイ時間
	{
		m_DelayCount += m_TimeManager->GetDeltaTime();
	}
	else if (BFMng->GetCurrentTurn() == Turn::EnemyMove && m_DelayCount > 1.5f && m_DelayCount < 4.0f)
	{
		if (!m_OnlyOneTime)
		{
			int NowPos = -1;
			int NextPos = -1;
			switch (currentEnemy->AIMove)
			{
			case EnemyMove::Attack:	//攻撃処理
				if (BFMng->GetFieldSquaresList()[currentEnemy->targetAISquare->GetSquareID()]->chara != nullptr)	
				{
					BFMng->SetAttackingCharacterSquares(BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]);
					BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->SetAnimation(Animations::Attack, currentEnemy->CharaAdmin, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos], currentEnemy->targetAISquare);
					BFMng->Attack(currentEnemy, BFMng->GetAlliesCharacterList()[currentEnemy->targetAICharacterID]);
				}
				else	//攻撃対象がいなかった場合の処理（念のため）
				{
					//BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->SetAnimation(Animations::Attack, currentEnemy->CharaAdmin, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos], currentEnemy->targetAISquare);
				}
				m_OnlyOneTime = true;
				break;
			case EnemyMove::Move:	//移動処理
				NowPos = currentEnemy->CharaPos;
				NextPos = currentEnemy->moveAISquareID;
				BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->SetAnimation(Animations::Move, currentEnemy->CharaAdmin, BFMng->GetFieldSquaresList()[NowPos], BFMng->GetFieldSquaresList()[NextPos]);
				BFMng->Move(NowPos, NextPos, currentEnemy->CharaID);
				m_OnlyOneTime = true;
				break;
			case EnemyMove::Wait:	//待機処理
				BFMng->Wait(currentEnemy->CharaPos);
				m_DelayCount = 0.0f;
				if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
				{
					m_MoveAIcount++;
				}
				if (m_MoveAIcount == BFMng->GetEnemyCharacterList().size() && BFMng->GetCurrentTurn() == Turn::EnemyMove)
				{
					BFMng->ChangeTurn();
					p_engine->GetSoundManager()->playBGM(0);
				}
				return true;
				break;
			case EnemyMove::None:
				m_DelayCount = 0.0f;
				if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
				{
					m_MoveAIcount++;
				}
				if (m_MoveAIcount == BFMng->GetEnemyCharacterList().size() && BFMng->GetCurrentTurn() == Turn::EnemyMove)
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
	else if (BFMng->GetCurrentTurn() == Turn::EnemyMove && m_DelayCount > 4.0f)	//行動終了処理
	{
		BFMng->CheckMoved();
		if (currentEnemy->AIMove == EnemyMove::Attack && BFMng->GetFieldSquaresList()[currentEnemy->targetAISquare->GetSquareID()]->chara != nullptr)
		{
			BFMng->CheckDead(BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[currentEnemy->targetAISquare->GetSquareID()]->ThereCharaID]);
		}
		ResetAI(currentEnemy);
		m_DelayCount = 0.0f;
		m_OnlyOneTime = false;
		if (m_MoveAIcount < BFMng->GetEnemyCharacterList().size())
		{
			m_MoveAIcount++;
		}
	}

	if (m_MoveAIcount == BFMng->GetEnemyCharacterList().size() && BFMng->GetCurrentTurn() == Turn::EnemyMove)	//全キャラクターの行動が終了したらターン交代
	{
		BFMng->ChangeTurn();
		p_engine->GetSoundManager()->playBGM(0);
	}
	return true;
}

void EnemyAIManager::finishAction()
{
}

bool EnemyAIManager::checkEnemyData()
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	int movedcount = 0;
	for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)
	{
		if (BFMng->GetEnemyCharacterList()[i]->Moved)
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
			//int serchingpos = (x + charaposX) + ((y + charaposY) * 10);
			//if (serchingpos < 150 && serchingpos > -1)
			//{
			//	if (BFMng->GetFieldSquaresList()[serchingpos]->chara != nullptr)
			//	{
			//		if (BFMng->GetFieldSquaresList()[serchingpos]->chara->CharaAdmin == Admin::Rebel)
			//		{
			//			distance.push_back(((x + charaposX) - charaposX) * ((x + charaposX) - charaposX) + ((y + charaposY) - charaposY) * ((y + charaposY) - charaposY));
			//			index.push_back(BFMng->GetFieldSquaresList()[serchingpos]->chara->CharaPos);
			//		}
			//	}
			//}

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
					if (BFMng->GetFieldSquaresList()[serchingpos]->chara->CharaAdmin == Admin::Rebel)
					{
						distance.push_back(std::abs(x) + std::abs(y));
						index.push_back(BFMng->GetFieldSquaresList()[serchingpos]->chara->CharaPos);
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

vector<EnemyAction> EnemyAIManager::GeneratePossibleActions(FieldCharacter* currentCharacter)
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
	Squares* targetPosition = GetnearCharaPos(currentCharacter->CharaRenge, BFMng->GetFieldSquaresList()[currentCharacter->CharaPos]->charaPosX, BFMng->GetFieldSquaresList()[currentCharacter->CharaPos]->charaPosY);
	if (targetPosition != nullptr)
	{
		EnemyAction attackAction;
		attackAction.m_ActionType = AIActionType::Attack;
		attackAction.m_TargetSqureaID = targetPosition->GetSquareID();
		attackAction.m_TargetCharacterID = targetPosition->chara->CharaID;

		actionList.push_back(attackAction);
	}

	currentCharacter->m_NearestEnemySquare = GetnearCharaPos(15.0f, BFMng->GetFieldSquaresList()[currentCharacter->CharaPos]->charaPosX, BFMng->GetFieldSquaresList()[currentCharacter->CharaPos]->charaPosY);

	//BFS(幅優先探索)による移動可能なマスの探索
	queue<pair<int, int>> bfsQueue;	//探索用のキュー（マスIDと移動コストのペア）
	map<int, int> visited;			//通ったことのあるマス（マスIDと移動コストのペア）
	int startSquareID = currentCharacter->CharaPos;

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
		if (currentCost < currentCharacter->CharaMoveRenge)
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

float EnemyAIManager::EvaluateAction(FieldCharacter* currentCharacter, const EnemyAction& action)
{
	float score = 0.0f;
	float distance = 0.0f;
	switch (action.m_ActionType)
	{
	case AIActionType::Attack:
		// 攻撃行動 
		if (action.m_TargetCharacterID != -1)
		{
			score += 1000.0f;
		}
		break;
	case AIActionType::Move:
		// 移動行動の場合、敵に近づく移動ならスコアを上げる
		distance = CalculateDistance(action.m_MoveSquareID, currentCharacter->m_NearestEnemySquare->GetSquareID());
		score += 1000.0f - distance * 10.0f;
		break;
	case AIActionType::Wait:
		score += 1.0f; // 待機は最も低く評価
		break;
	default:
		break;
	}
	return score;
}

EnemyAction EnemyAIManager::SelectBestAction(FieldCharacter* currentEnemy, const std::vector<EnemyAction>& possibleActions)
{
	if (possibleActions.empty())
	{
		return EnemyAction(); // 行動不可
	}

	float bestScore = -FLT_MAX;
	EnemyAction bestAction = possibleActions[0];

	for (const auto& action : possibleActions)
	{
		float currentScore = EvaluateAction(currentEnemy, action);
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

void EnemyAIManager::ResetAI(FieldCharacter* chara)
{
	chara->AIMove = EnemyMove::None;
	chara->targetAISquare = nullptr;
}

void EnemyAIManager::OnChangeTurn()
{
	m_Firsttime = false;
	m_MoveAIcount = 0;
}

