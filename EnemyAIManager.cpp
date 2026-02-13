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
			for (int i = 0; i < nextPosList.size(); i++)
			{
				nextPosList[i] = 0.0f;
			}
			m_Firsttime = true;
		}
		if (!BFMng->GetEnemyCharacterList().empty())
		{
			for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)
			{
				FieldCharacter* currentEnemy = BFMng->GetEnemyCharacterList()[i];

				if (!currentEnemy->Dead)
				{
					Squares* targetPos = GetnearCharaPos(currentEnemy->CharaRenge, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY);	//攻撃可能なキャラの位置を取得
					if (targetPos != nullptr)	//行動を攻撃に設定
					{
						currentEnemy->AIMove = EnemyMove::Attack;
						currentEnemy->targetAIPos = targetPos;
					}
					else if (targetPos == nullptr && currentEnemy->CharaAI != AIroutine::Defence)	//行動を移動に設定
					{
						currentEnemy->AIMove = EnemyMove::Move;
						Squares* target = GetnearCharaPos(15, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY);
						float MovePosX = 0;
						float MovePosY = 0;

						m_MoveCount = 0;

						for (int j = 0; j < currentEnemy->CharaMoveRenge; j++)	//機動力の回数だけ回す
						{
							Squares* inRengeCharacterPos = nullptr;
							if (target->charaPosX - 1 > BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX)
							{
								MovePosX++;
								m_MoveCount++;
								inRengeCharacterPos = GetnearCharaPos(currentEnemy->CharaRenge, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX + MovePosX, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY + MovePosY);
							}
							else if (target->charaPosX + 1 < BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX)
							{
								MovePosX--;
								m_MoveCount++;
								inRengeCharacterPos = GetnearCharaPos(currentEnemy->CharaRenge, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX + MovePosX, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY + MovePosY);
							}

							if (inRengeCharacterPos != nullptr)	//もし攻撃可能なキャラの位置が見つかったら抜ける
							{
								break;
							}

							if (m_MoveCount == currentEnemy->CharaMoveRenge)	//行動可能回数に達したら抜ける
							{
								break;
							}

							if (target->charaPosY - 1 > BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY)
							{
								MovePosY++;
								m_MoveCount++;
								inRengeCharacterPos = GetnearCharaPos(currentEnemy->CharaRenge, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX + MovePosX, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY + MovePosY);
							}
							else if (target->charaPosY + 1 < BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY)
							{
								MovePosY--;
								m_MoveCount++;
								inRengeCharacterPos = GetnearCharaPos(currentEnemy->CharaRenge, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosX + MovePosX, BFMng->GetFieldSquaresList()[currentEnemy->CharaPos]->charaPosY + MovePosY);
							}

							if (inRengeCharacterPos != nullptr)	//もし攻撃可能なキャラの位置が見つかったら抜ける
							{
								break;
							}

							if (m_MoveCount == currentEnemy->CharaMoveRenge)	//行動可能回数に達したら抜ける
							{
								break;
							}
						}

						DEBUG_FLOAT(currentEnemy->CharaPos + (MovePosX + MovePosY * 10));

						for (int j = 0; j < nextPosList.size(); j++)	//移動先が他の敵と被っていないか確認
						{
							if (nextPosList[j] == currentEnemy->CharaPos + (MovePosX + MovePosY * 10))
							{
								if (MovePosX > 0)
								{
									MovePosX--;
									break;
								}
								else if (MovePosX < 0)
								{
									MovePosX++;
									break;
								}

								if (MovePosY > 0)
								{
									MovePosY--;
									break;
								}
								else if (MovePosY < 0)
								{
									MovePosY++;
									break;
								}
							}
						}

						while (BFMng->GetFieldSquaresList()[currentEnemy->CharaPos + (MovePosX + MovePosY * 10)]->chara != nullptr)	//移動先にキャラがいる場合は動かせるまで調整
						{
							if (MovePosX > 0)
							{
								MovePosX--;
							}
							else if (MovePosX < 0)
							{
								MovePosX++;
							}
							else if (MovePosY > 0)
							{
								MovePosY--;
							}
							else if (MovePosY < 0)
							{
								MovePosY++;
							}
							else
							{
								// これ以上動かせない場合は無限ループ回避のために抜ける
								break;
							}
						}
						
						currentEnemy->targetAIPos = target;

						nextPosList[i] = currentEnemy->CharaPos + (MovePosX + MovePosY * 10);

						currentEnemy->nextAIPosX = MovePosX;
						currentEnemy->nextAIPosY = MovePosY;
					}
					else
					{
						currentEnemy->AIMove = EnemyMove::Wait;
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

	if (BFMng->GetCurrentTurn() == Turn::EnemyMove && m_DelayCount == 0.0f)	//アニメーション設定
	{
		if (BFMng->GetEnemyCharacterList()[m_MoveAIcount]->Dead)	//もし死んでいたらスキップ
		{
			ResetAI(BFMng->GetEnemyCharacterList()[m_MoveAIcount]);
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
		switch (BFMng->GetEnemyCharacterList()[m_MoveAIcount]->AIMove)	//アニメーションの設定
		{
		default:
			break;
		case EnemyMove::Attack:
			BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos]->fbxD->SetAnime(L"ATTACK01");
			break;
		case EnemyMove::Move:
			BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos]->fbxD->SetAnime(L"WALK01");
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
			switch (BFMng->GetEnemyCharacterList()[m_MoveAIcount]->AIMove)
			{
			case EnemyMove::Attack:	//攻撃処理
				if (BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->targetAIPos->GetSquareID()]->chara != nullptr)	
				{
					BFMng->SetAttackingCharacterSquares(BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos]);
					BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos]->SetAnimation(Animations::Attack, BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaAdmin, BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos], BFMng->GetEnemyCharacterList()[m_MoveAIcount]->targetAIPos);
					BFMng->Attack(BFMng->GetEnemyCharacterList()[m_MoveAIcount], BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->targetAIPos->GetSquareID()]->ThereCharaID]);
				}
				else	//攻撃対象がいなかった場合の処理（念のため）
				{
					//BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos]->SetAnimation(Animations::Attack, BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaAdmin, BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos], BFMng->GetEnemyCharacterList()[m_MoveAIcount]->targetAIPos);
				}
				m_OnlyOneTime = true;
				break;
			case EnemyMove::Move:	//移動処理
				NowPos = BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos;
				NextPos = nextPosList[m_MoveAIcount];
				BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos]->SetAnimation(Animations::Move, BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaAdmin, BFMng->GetFieldSquaresList()[NowPos], BFMng->GetFieldSquaresList()[NextPos]);
				BFMng->Move(NowPos, NextPos, BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaID);
				m_OnlyOneTime = true;
				break;
			case EnemyMove::Wait:	//待機処理
				BFMng->Wait(BFMng->GetEnemyCharacterList()[m_MoveAIcount]->CharaPos);
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
		if (BFMng->GetEnemyCharacterList()[m_MoveAIcount]->AIMove == EnemyMove::Attack && BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->targetAIPos->GetSquareID()]->chara != nullptr)
		{
			BFMng->CheckDead(BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[m_MoveAIcount]->targetAIPos->GetSquareID()]->ThereCharaID]);
		}
		ResetAI(BFMng->GetEnemyCharacterList()[m_MoveAIcount]);
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
						distance.push_back(((x + charaposX) - charaposX) * ((x + charaposX) - charaposX) + ((y + charaposY) - charaposY) * ((y + charaposY) - charaposY));
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

void EnemyAIManager::ResetAI(FieldCharacter* chara)
{
	chara->AIMove = EnemyMove::None;
	chara->AItargetID = -1;
	chara->nextAIPosX = -1;
	chara->nextAIPosY = -1;
	chara->targetAIPos = nullptr;
}

void EnemyAIManager::OnChangeTurn()
{
	m_Firsttime = false;
	m_MoveAIcount = 0;
}

