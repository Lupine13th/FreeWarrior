#include "BattleFieldManager.h"
#include "KeyBindComponent.h"		//入力管理コンポーネント本体
#include "SceneManager.h"		//入力管理コンポーネント本体
#include "Squares.h"
#include "EnemyAIManager.h"
#include "Admin.h"
#include "SoundManager.h"
#include "MyAccessHub.h"
#include "FlyingCameraController.h"
#include "EffectGenerator.h"
#include "BattleSceneManager.h"

#include <cmath> 
#include <chrono>
#include <algorithm>

using namespace std::chrono;

class BattleSceneManager;

void BattleFieldManager::InitAction()
{
	MyGameEngine* p_engine = MyAccessHub::GetMyGameEngine();
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());

	m_AlliesCharacterList.resize(5);	//一旦5枠確保
	m_EnemyCharacterList.resize(5);		//一旦5枠確保

	switch (p_scene->kPlayStates)		//動作環境次第で始めるシーンを切り替え
	{
	default:
		break;
	case PlayStates::Debug:
		SetCurrentTurn(Turn::Allies);
		break;
	case PlayStates::Release:
		SetCurrentTurn(Turn::First);
		break;
	}

	m_HUDManager = MyAccessHub::GetHUDManager();
	m_TimeManager = MyAccessHub::GetTimeManager();

	MyAccessHub::GetAIManager()->ClearJsonFile();

	p_engine->GetSoundManager()->playBGM(0);
}

bool BattleFieldManager::FrameAction()
{
	MyGameEngine* p_engine = MyAccessHub::GetMyGameEngine();
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	KeyBindComponent* keycomp = static_cast<KeyBindComponent*>(p_scene->getKeyComponent());
	FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();
	BattleSceneManager* BSMng = MyAccessHub::GetBattleSceneManager();

	if (m_CurrentTurn == Turn::First == !p_scene->GetIsLoading())
	{
		if (m_OpeningAnimHUD->OPAnimCount > 5.2f)	//オープニングアニメ終了後
		{
			SetCurrentTurn(Turn::Allies);
		}
	}
	else if (m_CurrentTurn == Turn::Allies)			//プレイヤーが画面を動かせる時に動作する
	{
		if (!m_Firsttime)	//初回のみ
		{
			FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();
			m_Lifecount = 0;
			for (int i = 0; i < m_AlliesCharacterList.size(); i++)	//現在生き残っているユニットの数を算出
			{
				if (!m_AlliesCharacterList[i]->GetIsDead())
				{
					m_Lifecount++;	
				}
			}
			m_Firsttime = true;
			UpdateBattleField();					//マスの色を調整
			ResetPlayerActionLogs();				//プレイヤー行動傾向の記録をリセット
			m_CursorState = CursorState::Select;	//カーソルを「選択中」にする
			SetFirstAlliesCharacterCamera();
		}

		switch (keycomp->getCurrentInputType())
		{
		case KeyBindComponent::INPUT_TYPES::KEYBOARD:
		{
			switch (m_Mode)
			{
				//==================================Fieldモード=====================================
			case Mode::FieldMode:
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))			//十字キーの左を入力
				{
					m_SelectPos[(int)Vector::X]--;																								//カーソルを左に移動
					if (m_SelectPos[(int)Vector::X] < 0)
					{
						m_SelectPos[(int)Vector::X] = 0;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//十字キーの右を入力
				{
					m_SelectPos[(int)Vector::X]++;																								//カーソルを右に移動
					if (m_SelectPos[(int)Vector::X] > 9)
					{
						m_SelectPos[(int)Vector::X] = 9;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//十字キーの下を入力
				{
					m_SelectPos[(int)Vector::Y]--;																								//カーソルを下(手前)に移動
					if (m_SelectPos[(int)Vector::Y] < 0)
					{
						m_SelectPos[(int)Vector::Y] = 0;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))	//十字キーの上を入力
				{
					m_SelectPos[(int)Vector::Y]++;																								//カーソルを上(奥)に移動
					if (m_SelectPos[(int)Vector::Y] > 14)
					{
						m_SelectPos[(int)Vector::Y] = 14;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを入力
				{
					if (m_FieldSquaresList[m_SelectID]->chara != nullptr && m_FieldSquaresList[m_SelectID]->chara->GetAdmin() == Admin::Rebel && !m_FieldSquaresList[m_SelectID]->chara->GetIsActioned() && !m_FieldSquaresList[m_SelectID]->chara->GetIsDead())	//カーソルが味方部隊をさしている時
					{
						p_engine->GetSoundManager()->play(3);
						m_MenuSelectIndex = 0;

						m_InRangeIDListForMenu.clear();	//攻撃範囲内のマスIDリストをクリア

						SearchInRengeSquare(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->GetAttackRenge(), m_InRangeIDListForMenu);	//攻撃範囲内のマスIDリストを取得

						m_InRangeEnemyList.clear();	//攻撃範囲内の敵のマスIDリストをクリア

						for (int i = 0; i < m_InRangeIDListForMenu.size(); i++)	//攻撃範囲内の敵のマスIDリストを取得
						{
							if (m_FieldSquaresList[m_InRangeIDListForMenu[i]]->chara != nullptr)
							{
								if (m_FieldSquaresList[m_InRangeIDListForMenu[i]]->chara->GetAdmin() == Admin::Imperial)
								{
									m_InRangeEnemyList.push_back(m_InRangeIDListForMenu[i]);
								}
							}
						}

						if (m_InRangeEnemyList.size() > 0)	//攻撃範囲内に敵がいる場合
						{
							m_MenuSelectIndex = 0;			//攻撃コマンドにカーソルを合わせる
						}
						else								//攻撃範囲内に敵がいない場合
						{
							m_MenuSelectIndex = 1;			//移動コマンドにカーソルを合わせる
						}

						m_HUDManager->GetHUDObject("MainMenuHUD")->SetAnimationState(AnimationState::OnInit);
						m_Mode = Mode::MenuMode;
					}
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_TAB))
				{
					if (m_OpenLog)
					{
						m_OpenLog = false;
					}
					else
					{
						m_OpenLog = true;
					}
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキー入力
				{
					m_Mode = Mode::SideMenuMode;	//サイドメニュー展開
					m_SideMenuSelectIndex = 0;
				}

				//=============================デバッグ用ボタン=================================
				else if (p_scene->kPlayStates == PlayStates::Debug || p_scene->kPlayStates == PlayStates::Release)	//エンターキー入力
				{
					if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_DEBUG))
					{
						SetPlayerWin(true);
						m_CurrentTurn = Turn::Result;
					}
				}
				//=============================デバッグ用ボタン=================================
				break;

				//==================================Fieldモード=====================================

			case Mode::SideMenuMode:
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))		//十字キーの上を入力
				{
					m_SideMenuSelectIndex--;																									//メニューのカーソルを上に移動
					if (m_SideMenuSelectIndex < 0)
					{
						m_SideMenuSelectIndex = 0;
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//十字キーの下を入力
				{
					m_SideMenuSelectIndex++;																									//メニューのカーソルを下に移動
					if (m_SideMenuSelectIndex > 2)
					{
						m_SideMenuSelectIndex = 2;
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを入力
				{
					switch (m_SideMenuSelectIndex)
					{
					case 0:																														//ターン終了コマンド
						for (int i = 0; i < m_AlliesCharacterList.size(); i++)
						{
							if (!m_AlliesCharacterList[i]->GetIsDead())
							{
								Wait(m_AlliesCharacterList[i]->GetCharacterPosOnSquares());
							}
						}
						m_Mode = Mode::FieldMode;
						UpdateBattleField();
						break;
					case 1:																														//ゲーム終了コマンド
						PostQuitMessage(0);
						break;
					case 2:																														//メニューを閉じるコマンド
						m_Mode = Mode::FieldMode;
						break;
					}
				}
				break;


				//==================================Menuモード=====================================
			case Mode::MenuMode:
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))			//十字キーの上を入力
				{
					m_MenuSelectIndex++;
					if (m_MenuSelectIndex > 4)
					{
						m_MenuSelectIndex = 4;
					}
					if (m_InRangeEnemyList.size() == 0)
					{
						if (m_MenuSelectIndex == 2)
						{
							m_MenuSelectIndex = 3;
						}
					}

					m_HUDManager->GetHUDObject("MainMenuHUD")->SetAnimationState(AnimationState::Init);
					p_engine->GetSoundManager()->play(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))	//十字キーの下を入力
				{
					m_MenuSelectIndex--;
					if (m_InRangeEnemyList.size() == 0)
					{
						if (m_MenuSelectIndex < 1)
						{
							m_MenuSelectIndex = 1;
						}
						if (m_MenuSelectIndex == 2)
						{
							m_MenuSelectIndex = 1;
						}
					}
					else
					{
						if (m_MenuSelectIndex < 0)
						{
							m_MenuSelectIndex = 0;
						}
					}
					
					m_HUDManager->GetHUDObject("MainMenuHUD")->SetAnimationState(AnimationState::Init);
					p_engine->GetSoundManager()->play(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを入力
				{
					if (m_FieldSquaresList[m_SelectID]->ThereCharaID != -1)
					{
						switch (m_MenuSelectIndex)
						{
						default:
							break;
						case 0:																													//攻撃コマンド
							m_Mode = Mode::AttackMode;

							m_TargetID = m_SelectID;
							m_TargetPos[(int)Vector::X] = m_SelectPos[(int)Vector::X];
							m_TargetPos[(int)Vector::Y] = m_SelectPos[(int)Vector::Y];

							m_SelectingSquare = m_FieldSquaresList[m_SelectID];

							UpdateBattleField();

							m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::OnInit);

							p_engine->GetSoundManager()->play(3);
							break;
						case 1:																													//移動コマンド
							m_Mode = Mode::MoveMode;

							m_TargetID = m_SelectID;
							m_TargetPos[(int)Vector::X] = m_SelectPos[(int)Vector::X];
							m_TargetPos[(int)Vector::Y] = m_SelectPos[(int)Vector::Y];

							m_PassedSquaresList.clear();

							m_FieldSquaresList[m_TargetID]->passed = true;
							m_PassedSquaresList.push_back(m_FieldSquaresList[m_TargetID]);

							m_SelectingSquare = m_FieldSquaresList[m_SelectID];

							UpdateBattleField();

							SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->GetMoveRenge());

							SetEnemyRengeSquareTexture();

							p_engine->GetSoundManager()->play(3);
							break;
						case 2:																													//行動コマンド
							m_Mode = Mode::AbilityMode;

							m_TargetID = m_SelectID;
							m_TargetPos[(int)Vector::X] = m_SelectPos[(int)Vector::X];
							m_TargetPos[(int)Vector::Y] = m_SelectPos[(int)Vector::Y];

							m_SelectingSquare = m_FieldSquaresList[m_SelectID];

							UpdateBattleField();

							m_AbillityMenuState = AbillityMenuState::Menu;

							m_HUDManager->GetHUDObject("AbilityHUD")->ResetHUD();

							p_engine->GetSoundManager()->play(3);
							break;
						case 3:																													//待機コマンド
							m_Mode = Mode::FieldMode;

							Wait(m_SelectID);

							ResetHUDs(3);
							break;
						case 4:																													//メニューを閉じる
							m_Mode = Mode::FieldMode;

							p_engine->GetSoundManager()->play(6);
							break;
						}

						m_HUDManager->GetHUDObject("MainMenuHUD")->SetAnimationState(AnimationState::Finish);
						
					}
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキーを入力
				{
					m_Mode = Mode::FieldMode;
					m_HUDManager->GetHUDObject("MainMenuHUD")->SetAnimationState(AnimationState::Finish);
					p_engine->GetSoundManager()->play(6);
				}
				break;

				//==================================Menuモード=====================================



				//==================================Attackモード=====================================
			case Mode::AttackMode:
				m_FieldSquaresList[m_SelectID]->fbxD->SetAnimeInit(L"ATTACK", m_FieldSquaresList[m_SelectID]->chara);

				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))			//十字キーの左を入力
				{
					m_TargetPos[(int)Vector::X]--;																								//ターゲットカーソルを左に移動
					if (m_TargetPos[(int)Vector::X] < 0)
					{
						m_TargetPos[(int)Vector::X] = 0;
					}
					if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
					{
						m_TargetPos[(int)Vector::X]++;
					}
					m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//十字キーの右を入力
				{
					m_TargetPos[(int)Vector::X]++;																								//ターゲットカーソルを右に移動
					if (m_TargetPos[(int)Vector::X] > 9)
					{
						m_TargetPos[(int)Vector::X] = 9;
					}
					if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
					{
						m_TargetPos[(int)Vector::X]--;
					}
					m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//十字キーの下を入力
				{
					m_TargetPos[(int)Vector::Y]--;																								//ターゲットカーソルを下に移動
					if (m_TargetPos[(int)Vector::Y] < 0)
					{
						m_TargetPos[(int)Vector::Y] = 0;
					}
					if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
					{
						m_TargetPos[(int)Vector::Y]++;
					}
					m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))	//十字キーの上を入力
				{
					m_TargetPos[(int)Vector::Y]++;																								//ターゲットカーソルを上に移動
					if (m_TargetPos[(int)Vector::Y] > 14)
					{
						m_TargetPos[(int)Vector::Y] = 14;
					}
					if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
					{
						m_TargetPos[(int)Vector::Y]--;
					}
					m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを入力
				{
					if (m_FieldSquaresList[m_TargetID]->chara != nullptr && m_FieldSquaresList[m_TargetID]->chara->GetAdmin() != m_FieldSquaresList[m_SelectID]->chara->GetAdmin())	//ターゲットカーソルが敵のいる位置にある
					{
						m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::None);	//攻撃HUDを切る
						ResetPredectCamera();	

						SetAttackingCharacterSquares(m_FieldSquaresList[m_SelectID]);
						//m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Attack, m_FieldSquaresList[m_SelectID]->chara->GetAdmin(), m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
						Attack(m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID], m_EnemyCharacterList[m_FieldSquaresList[m_TargetID]->ThereCharaID]);
						BSMng->SetBattlePosition(Animations::Attack, m_FieldSquaresList[m_SelectID]->GetPlatoon(), m_FieldSquaresList[m_TargetID]->GetPlatoon());
						ResetFieldFromMove();
						m_Mode = Mode::FieldMode;
						m_FieldSquaresList[m_SelectID]->chara->SetIsActioned(true);
						ResetHUDs(3);
					}
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキーを入力
				{
					m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::None);		//攻撃HUDを切る
					ResetPredectCamera();																			//攻撃HUDを切る

					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);													//メニューを閉じる
					m_Mode = Mode::FieldMode;
					ResetFieldFromMove();
					m_FieldSquaresList[m_SelectID]->fbxD->SetAnimeInit(L"WAIT", m_FieldSquaresList[m_SelectID]->chara);							//待機アニメーション
					ResetHUDs(6);
				}
				break;

				//==================================Attackモード=====================================




				//==================================Moveモード=====================================

			case Mode::MoveMode:
				m_FieldSquaresList[m_SelectID]->fbxD->SetAnimeInit(L"WALK", m_FieldSquaresList[m_SelectID]->chara);		//移動アニメーション

				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))			//十字キーの左を入力
				{
					m_TargetPos[(int)Vector::X]--;																								//ターゲットカーソルを左に移動
					if (m_TargetPos[(int)Vector::X] < 0)
					{
						m_TargetPos[(int)Vector::X] = 0;
					}
					else
					{
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::X]++;
						}
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//十字キーの右を入力
				{
					m_TargetPos[(int)Vector::X]++;																								//ターゲットカーソルを右に移動
					if (m_TargetPos[(int)Vector::X] > 9)
					{
						m_TargetPos[(int)Vector::X] = 9;
					}
					else
					{
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::X]--;
						}
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//十字キーの下を入力
				{
					m_TargetPos[(int)Vector::Y]--;																								//ターゲットカーソルを下に移動
					if (m_TargetPos[(int)Vector::Y] < 0)
					{
						m_TargetPos[(int)Vector::Y] = 0;
					}
					else
					{
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::Y]++;
						}
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))	//十字キーの上を入力
				{
					m_TargetPos[(int)Vector::Y]++;																								//ターゲットカーソルを上に移動
					if (m_TargetPos[(int)Vector::Y] > 14)
					{
						m_TargetPos[(int)Vector::Y] = 14;
					}
					else
					{
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::Y]--;
						}
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK) && m_SelectID != m_TargetID && m_FieldSquaresList[m_TargetID]->chara == nullptr)	//スペースキーを入力＆ターゲットカーソルの位置に誰もいない
				{
					BSMng->SetMovePosition(m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID], m_FieldSquaresList[m_SelectID]->GetFBXData());

					//移動処理
					Move(m_SelectID, m_TargetID, m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->GetPlatoonID());

					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
					ResetFieldFromMove();
					m_Mode = Mode::FieldMode;

					ResetHUDs(3);
				}

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))	//エスケープキー入力時
				{
					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);												//メニューを閉じる
					ResetFieldFromMove();
					m_Mode = Mode::FieldMode;
					m_FieldSquaresList[m_SelectID]->fbxD->SetAnimeInit(L"WAIT", m_FieldSquaresList[m_SelectID]->chara);						//待機アニメーション

					m_PassedSquaresList.clear();
					ResetHUDs(6);
				}
				break;

			//==================================Moveモード=====================================



			//==================================Abilityモード=====================================
			case Mode::AbilityMode:
				m_FieldSquaresList[m_SelectID]->fbxD->SetAnimeInit(L"ATTACK", m_FieldSquaresList[m_SelectID]->chara);						//攻撃アニメーション
				switch (m_TargetMode)
				{
				case TargetMode::None:																										//ターゲットモードが無い＝メニュー展開中
					
					m_AbillityCount = 0;

					for (int i = 0; i < 3; i++)																								//部隊の持つ行動の種類をカウント
					{
						if (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->GetAbilityList()[i] != AbilityType::None)
						{
							m_AbillityCount++;
						}
					}

					if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))		//十字キー右を押したとき
					{
						m_AbillityIndex++;																											//行動を切り替え(右を選択)
						if (m_AbillityIndex > m_AbillityCount - 1)
						{
							m_AbillityIndex = m_AbillityCount - 1;
						}
						p_engine->GetSoundManager()->play(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))	//十字キー左を押したとき
					{
						m_AbillityIndex--;																											//行動を切り替え(左を選択)
						if (m_AbillityIndex < 0)
						{
							m_AbillityIndex = 0;
						}
						p_engine->GetSoundManager()->play(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを押したとき
					{
						if (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->GetAbilityList()[m_AbillityIndex] != AbilityType::None)	//ヌルチェック
						{
							switch (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->GetAbilityList()[m_AbillityIndex])				//選択中のアビリティに応じてカーソルの種類を変える(現在は敵をターゲットとする行動しかない)
							{
							default:
								break;
							case AbilityType::ConcentratedFire:
								m_TargetMode = TargetMode::EnemyTarget;
								break;
							case AbilityType::BayonetCharge:
								m_TargetMode = TargetMode::EnemyTarget;
								break;
							case AbilityType::Scout:
								m_TargetMode = TargetMode::EnemyTarget;
								break;
							}

							m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::OnInit);
							
							m_AbillityMenuState = AbillityMenuState::Target;
							p_engine->GetSoundManager()->play(3);

							UpdateBattleField();
						}
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキーを押したとき
					{
						m_Mode = Mode::FieldMode;						//メニューを閉じる
						m_AbillityMenuState = AbillityMenuState::None;
						m_AbillityIndex = 0;

						p_engine->GetSoundManager()->play(6);

						UpdateBattleField();
					}
					break;
				case TargetMode::EnemyTarget:																										//敵に対する行動
					if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))			//十字キー左を押したとき
					{
						m_TargetPos[(int)Vector::X]--;
						if (m_TargetPos[(int)Vector::X] < 0)
						{
							m_TargetPos[(int)Vector::X] = 0;
						}
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::X]++;
						}

						m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット

						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//十字キー右を押したとき
					{
						m_TargetPos[(int)Vector::X]++;
						if (m_TargetPos[(int)Vector::X] > 9)
						{
							m_TargetPos[(int)Vector::X] = 9;
						}
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::X]--;
						}

						m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット

						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//十字キー下を押したとき
					{
						m_TargetPos[(int)Vector::Y]--;
						if (m_TargetPos[(int)Vector::Y] < 0)
						{
							m_TargetPos[(int)Vector::Y] = 0;
						}
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::Y]++;
						}

						m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット

						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))	//十字キー上を押したとき
					{
						m_TargetPos[(int)Vector::Y]++;
						if (m_TargetPos[(int)Vector::Y] > 14)
						{
							m_TargetPos[(int)Vector::Y] = 14;
						}
						if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_TargetPos[(int)Vector::X] + m_TargetPos[(int)Vector::Y] * 10) == m_InRangeIDList.end())
						{
							m_TargetPos[(int)Vector::Y]--;
						}

						m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::Init);		//攻撃HUDをリセット

						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを押したとき
					{
						if (m_FieldSquaresList[m_TargetID]->chara != nullptr && m_FieldSquaresList[m_TargetID]->chara->GetAdmin() != m_FieldSquaresList[m_SelectID]->chara->GetAdmin())
						{
							SetAttackingCharacterSquares(m_FieldSquaresList[m_SelectID]);
							switch ((m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->GetAbilityList()[m_AbillityIndex]))				//選択中のアビリティに応じて戦闘アニメーションを切り替える
							{
							case AbilityType::Scout:
								BSMng->SetBattlePosition(Animations::Scout, m_FieldSquaresList[m_SelectID]->GetPlatoon(), m_FieldSquaresList[m_TargetID]->GetPlatoon());
								ResetHUDs(9);
								break;
							case AbilityType::ConcentratedFire:
								BSMng->SetBattlePosition(Animations::ConcentratedFire, m_FieldSquaresList[m_SelectID]->GetPlatoon(), m_FieldSquaresList[m_TargetID]->GetPlatoon());
								ResetHUDs(3);
								break;
							case AbilityType::BayonetCharge:
								BSMng->SetBattlePosition(Animations::BayonetCharge, m_FieldSquaresList[m_SelectID]->GetPlatoon(), m_FieldSquaresList[m_TargetID]->GetPlatoon());
								ResetHUDs(3);
								break;
							}

							m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::None);		//攻撃HUDを切る
							ResetPredectCamera();																			//攻撃HUDを切る
							
							Abiliting(m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID], m_EnemyCharacterList[m_FieldSquaresList[m_TargetID]->ThereCharaID]);
							m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
							ResetFieldFromMove();
							m_FieldSquaresList[m_SelectID]->chara->SetIsActioned(true);
							ResetAbillityMenu();
							UpdateBattleField();
						}
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキーを押したとき
					{
						m_HUDManager->GetHUDObject("BattlePredictionHUD")->SetAnimationState(AnimationState::None);		//攻撃HUDを切る
						ResetPredectCamera();																			//攻撃HUDを切る

						m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);													//メニューを閉じる
						ResetFieldFromMove();
						m_Mode = Mode::FieldMode;
						m_TargetMode = TargetMode::None;
						m_FieldSquaresList[m_SelectID]->fbxD->SetAnimeInit(L"WAIT", m_FieldSquaresList[m_SelectID]->chara);							//待機アニメーション
						m_AbillityIndex = 0;
						ResetHUDs(6);
					}
					break;
				}
				break;

			//==================================Abilityモード=====================================


			//==================================TurnEndモード=====================================

			case Mode::TurnEndMode:
				if (m_HUDManager->GetHUDObject("TurnEndHUD")->GetAnimationState() == AnimationState::OnInit)	//ディレイ中は操作を受け付けない
					break;

				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))			//十字キー右を押したとき
				{
					m_TurnEndMenuSelectIndex = 1;																									//一旦戻る(現状、意味は無い)
					m_HUDManager->GetHUDObject("TurnEndHUD")->SetAnimationState(AnimationState::Run);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))		//十字キー左を押したとき
				{
					m_TurnEndMenuSelectIndex = 2;																									//ターンエンド処理
					m_HUDManager->GetHUDObject("TurnEndHUD")->SetAnimationState(AnimationState::Run);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))				//スペースキーを押したとき
				{
					switch (m_TurnEndMenuSelectIndex)
					{
					case 1:
						m_Mode = Mode::FieldMode;																									//一旦戻る(現状、意味は無い)
						break;
					case 2:
						m_Mode = Mode::FieldMode;																									//ターンエンド処理
						MyAccessHub::GetAIManager()->CreateLearningData(m_PlayerActionLogs);														//学習データを作成
						m_HUDManager->GetHUDObject("TurnEndHUD")->SetAnimationState(AnimationState::None);
						m_TurnEndMenuSelectIndex = 0;
						AddTurnCount();
						ChangeTurn();
						break;
					default:
						break;
					}
				}
				break;
			}
		}
		}//AlliesTurn
	}
	else if (m_CurrentTurn == Turn::Result)
	{
		if (m_HUDManager->GetHUDObject("EndingHUD")->GetAnimationState() == AnimationState::None)
		{
			m_HUDManager->GetHUDObject("EndingHUD")->SetAnimationState(AnimationState::OnInit);
		}
		else if (m_HUDManager->GetHUDObject("EndingHUD")->GetAnimationState() == AnimationState::Run)
		{
			if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
			{
				if (m_PlayerWin)
				{
					m_HUDManager->GetHUDObject("EndingHUD")->SetAnimationState(AnimationState::Finish);
				}
				else
				{
					PostQuitMessage(0);
				}
			}
		}
		else if (m_HUDManager->GetHUDObject("EndingHUD")->GetAnimationState() == AnimationState::Finish)
		{
			if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
			{
				PostQuitMessage(0);
			}
		}
	}

	if (m_IsAttacking)	//攻撃アニメーション中
	{
		m_AttackingCount += m_TimeManager->GetDeltaTime();
		if (m_AttackingCount > kMaxAttackingCount)
		{
			CheckDead(m_AttackedCharacter);
			m_AttackingCount = 0.0f;
			m_IsAttacking = false;
		}
	}

	for (int i = 0; i < m_AlliesCharacterList.size(); i++)	//サークルエフェクトは必要かのループ
	{
		auto charaPositionSquare = m_FieldSquaresList[m_AlliesCharacterList[i]->GetCharacterPosOnSquares()]->GetSquarePosition();

		if (!m_AlliesCharacterList[i]->GetIsActioned() && !m_AlliesCharacterList[i]->GetIsDead() && m_CurrentTurn == Turn::Allies)	//移動していない味方キャラがいる場合、サークルエフェクトを再生
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Circle0" + to_wstring(i))->PlayEffect(XMFLOAT3(charaPositionSquare.x, charaPositionSquare.y + 2.0f, charaPositionSquare.z), XMFLOAT3(90.0f, 0.0f, 0.0f), 0.5f);
		}
	}

	return true;
}

void BattleFieldManager::FinishAction()
{

}

//マスの色更新とターン終了の確認
void BattleFieldManager::UpdateBattleField()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();

	for (int i = 0; i < m_FieldSquaresList.size(); i++)
	{
		if (m_FieldSquaresList[i]->target)
		{
			m_FieldSquaresList[i]->SetSquaresColor(SquareColor::TargetColor);
		}
	}

	switch (m_Mode)
	{
	case Mode::FieldMode:
	case Mode::MenuMode:
		m_FieldSquaresList[m_SelectID]->SetSquaresColor(SquareColor::NotCursor);
		m_SelectID = m_SelectPos[(int)Vector::X] + (10 * m_SelectPos[(int)Vector::Y]);
		m_FieldSquaresList[m_SelectID]->SetSquaresColor(SquareColor::FieldCursor);
		m_CursorState = CursorState::Select;
		break;
	case Mode::AttackMode:
		m_TargetID = m_TargetPos[(int)Vector::X] + (10 * m_TargetPos[(int)Vector::Y]);
		SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->GetAttackRenge());
		m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::AttackSellectCursor);
		m_CursorState = CursorState::Target;
		break;
	case Mode::MoveMode:
		m_TargetID = m_TargetPos[(int)Vector::X] + (10 * m_TargetPos[(int)Vector::Y]);
		SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->GetMoveRenge());
		SetEnemyRengeSquareTexture();
		m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::PassColor);
		m_CursorState = CursorState::Target;
		break;
	case Mode::AbilityMode:
		if (m_TargetMode == TargetMode::EnemyTarget)
		{
			m_TargetID = m_TargetPos[(int)Vector::X] + (10 * m_TargetPos[(int)Vector::Y]);
			SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->GetAttackRenge());
			m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::AttackSellectCursor);
			m_CursorState = CursorState::Target;
		}
		break;
	}

	m_MovedCount = 0;

	for (int i = 0; i < m_AlliesCharacterList.size(); i++)
	{
		auto charaPositionSquare = m_FieldSquaresList[m_AlliesCharacterList[i]->GetCharacterPosOnSquares()]->GetSquarePosition();
		if (m_AlliesCharacterList[i] != nullptr)
		{
			if (m_AlliesCharacterList[i]->GetIsActioned())
			{
				m_MovedCount++;
			}

			if (m_FieldSquaresList[m_AlliesCharacterList[i]->GetCharacterPosOnSquares()]->target)
			{
				SetResult(true);
				return;
			}
		}
	}

	if (m_MovedCount == m_Lifecount)
	{
		m_Mode = Mode::TurnEndMode;
		m_HUDManager->GetHUDObject("TurnEndHUD")->SetAnimationState(AnimationState::OnInit);
		return;
	}

	Fcam->ChangeCameraPosition();
}


XMFLOAT3 BattleFieldManager::GetCharaPos(Squares* squ)
{
	return squ->GetSquarePosition();
}

void BattleFieldManager::SetTerrainData()
{
	m_FieldSquaresList[30]->terrainname = Terrain::Forest;
	m_FieldSquaresList[31]->terrainname = Terrain::Forest;
	m_FieldSquaresList[32]->terrainname = Terrain::Hills;
	m_FieldSquaresList[73]->terrainname = Terrain::Tower;
	m_FieldSquaresList[75]->terrainname = Terrain::Tower;
	m_FieldSquaresList[77]->terrainname = Terrain::Tower;
	m_FieldSquaresList[94]->terrainname = Terrain::River;
	m_FieldSquaresList[95]->terrainname = Terrain::River;
	m_FieldSquaresList[96]->terrainname = Terrain::River;
	m_FieldSquaresList[97]->terrainname = Terrain::River;
}

void BattleFieldManager::Abiliting(Platoon* attackingchara, Platoon* attackedchara)
{
	switch (attackingchara->GetAbilityList()[m_AbillityIndex])
	{
	case AbilityType::ConcentratedFire:
		//集中砲火の処理
		m_Abilities->ConcentratedFire(attackingchara, attackedchara);
		m_AttackedCharacter = attackedchara;
		m_IsAttacking = true;
		break;
	case AbilityType::BayonetCharge:
		//突撃の処理
		m_Abilities->BayonetCharge(attackingchara, attackedchara);
		m_AttackedCharacter = attackedchara;
		m_IsAttacking = true;
		break;
	case AbilityType::Scout:
		//偵察の処理
		m_IsSucceedScout = m_Abilities->Scout(attackingchara, attackedchara);
		m_AttackedCharacter = attackedchara;
		m_IsAttacking = true;
		break;
	default:
		break;
	}
}

void BattleFieldManager::DeleteChara(int deadCharaPos)
{
	MyGameEngine* p_engine = MyAccessHub::GetMyGameEngine();

	m_FieldSquaresList[deadCharaPos]->ThereCharaID = -1;
	m_FieldSquaresList[deadCharaPos]->SqAdmin = Admin::None;
	m_FieldSquaresList[deadCharaPos]->chara = nullptr;
	m_FieldSquaresList[deadCharaPos]->fbxD->playerData->IsActive = false;
	m_FieldSquaresList[deadCharaPos]->fbxD->SetPosition(0.0f, -100.0f, 0.0f);
	m_FieldSquaresList[deadCharaPos]->fbxD = nullptr;

	p_engine->GetSoundManager()->play(7);
}

void BattleFieldManager::CheckDead(Platoon* chara)
{
	if (chara->GetIsDead())
	{
		return;
	}

	if (chara->GetSoldiers() < 1.0f)
	{
		chara->SetSoldier(0);
		chara->SetIsDead(true);
		DeleteChara(chara->GetCharacterPosOnSquares());
		switch (chara->GetAdmin())
		{
		default:
			break;
		case Admin::Rebel:
			m_EnemyKillCount++;
			break;
		case Admin::Imperial:
			m_PlayerKillCount++;
			break;
		}
	}

	if (m_EnemyKillCount == 5)
	{
		SetResult(false);
	}
	else if (m_PlayerKillCount == 5)
	{
		SetResult(true);
	}
}

void BattleFieldManager::CheckMoved()
{
	for (int i = 0; i < m_AlliesCharacterList.size(); i++)
	{
		if (m_FieldSquaresList[m_AlliesCharacterList[i]->GetCharacterPosOnSquares()]->fbxD != nullptr)
		{
			auto fbxData = m_FieldSquaresList[m_AlliesCharacterList[i]->GetCharacterPosOnSquares()]->fbxD;

			if (m_AlliesCharacterList[i]->GetIsActioned())
			{
				fbxData->SetScale(fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2);
			}
			else
			{
				fbxData->SetScale(fbxData->GetScaleValue(), fbxData->GetScaleValue(), fbxData->GetScaleValue());
			}
		}
	}

	for (int i = 0; i < m_EnemyCharacterList.size(); i++)
	{
		if (m_FieldSquaresList[m_EnemyCharacterList[i]->GetCharacterPosOnSquares()]->fbxD != nullptr)
		{
			auto fbxData = m_FieldSquaresList[m_EnemyCharacterList[i]->GetCharacterPosOnSquares()]->fbxD;

			if (m_EnemyCharacterList[i]->GetIsActioned())
			{
				fbxData->SetScale(fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2);
			}
			else
			{
				fbxData->SetScale(fbxData->GetScaleValue(), fbxData->GetScaleValue(), fbxData->GetScaleValue());
			}
		}
	}
}

void BattleFieldManager::SetResult(bool win)	//勝敗の設定
{
	m_PlayerWin = win;
	m_CurrentTurn = Turn::Result;
}

void BattleFieldManager::SearchInRengeSquare(int charaPosition, float renge, vector<int>& idList)
{
	for (int x = -renge; x <= renge; x++)
	{
		for (int y = -renge; y <= renge; y++)
		{
			int nx = m_FieldSquaresList[charaPosition]->charaPosX + x;
			int ny = m_FieldSquaresList[charaPosition]->charaPosY + y;

			if (std::abs(x) + std::abs(y) <= renge &&	//マンハッタン距離での範囲計算
				charaPosition + x + (y * 10) >= 0 &&
				charaPosition + x + (y * 10) <= 149 &&		//座標の範囲内である
				nx >= 0 && nx <= 9 &&
				ny >= 0 && ny <= 14)					//上下左右の限界突破をしていない
			{
				idList.push_back(charaPosition + x + (y * 10));
			}
		}
	}
}

void BattleFieldManager::ResetAbillityMenu()
{
	m_Mode = Mode::FieldMode;
	m_TargetMode = TargetMode::None;
	m_AbillityIndex = 0;
}

void BattleFieldManager::ResetPredectCamera()
{
	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());

	scene->SetActiveCameraCompornent(L"AttackerCameraForHUD", false);
	scene->SetActiveCameraCompornent(L"DefenderCameraForHUD", false);
}

int BattleFieldManager::CalculateDamage(ActionName action, Platoon* attackingCharacter, Platoon* attackedCharacter)
{
	float damage = 0;

	if (AttackingPlatoonIsAttackingTerrain(attackingCharacter))
	{
		damage = attackingCharacter->GetAttackPower() * 1.2f;						//攻撃側が森林、監視塔の地形にいる場合、ダメージ増加
	}
	else
	{
		damage = attackingCharacter->GetAttackPower();								//そうじゃなければそのまま攻撃力を持ってくる
	}

	if (attackedCharacter->GetArmor() != 0.0f)
	{
		damage = damage * (1.0f - (float)attackedCharacter->GetArmor() / 100.0f);	//部隊の装甲値に応じてダメージ減衰
	}

	if (!attackedCharacter->GetIsDetected())
	{
		damage = damage * 0.75f;														//偵察出来ていない場合、ダメージ減衰
	}

	if (AttackedPlatoonIsDefenciveTerrain(attackedCharacter))
	{
		damage = damage * 0.75f;														//防御側が丘陵、河川地形にいる場合、ダメージ減衰
	}

	if (damage > attackedCharacter->GetSoldiers())
	{
		damage = attackedCharacter->GetSoldiers();									//オーバーフローを起こさないように
	}

	switch (action)
	{
	case ActionName::ConcentratedFire:
		damage = damage * 1.5f;
		break;
	case ActionName::BayonetCharge:
		damage = damage * 1.5f;
		break;
	}

	int finalDamage = (int)damage;		//変な切り捨てが無駄に行われないように最後にfloat型をint型に変換する

	return finalDamage;
}

bool BattleFieldManager::AttackingPlatoonIsAttackingTerrain(Platoon* attackingChara)	//攻撃側が攻撃向き地形にいるかどうか？
{
	if (GetFieldSquaresList()[attackingChara->GetCharacterPosOnSquares()]->terrainname == Terrain::Forest || GetFieldSquaresList()[attackingChara->GetCharacterPosOnSquares()]->terrainname == Terrain::Tower)
	{
		return true;
	}
	return false;
}

bool BattleFieldManager::AttackedPlatoonIsDefenciveTerrain(Platoon* attackedChara)		//防御側が防御向き地形にいるかどうか？
{
	if (GetFieldSquaresList()[attackedChara->GetCharacterPosOnSquares()]->terrainname == Terrain::Hills || GetFieldSquaresList()[attackedChara->GetCharacterPosOnSquares()]->terrainname == Terrain::River)
	{
		return true;
	}
	return false;
}

bool BattleFieldManager::IsNotDetectedAndEnemyAdmin(Squares* charaSquare)
{
	if (!charaSquare->chara->GetIsDetected() && charaSquare->chara->GetAdmin() == Admin::Imperial)
	{
		return true;
	}
	return false;
}

void BattleFieldManager::SetChangedSq(int index)
{
	if (index >= 0 && index <= 149)
	{
		m_FieldSquaresList[index]->SetSquaresColor(SquareColor::MoveCursor);
		m_ChangedSquaresList.push_back(m_FieldSquaresList[index]);
	}
	
}

void BattleFieldManager::SetInRengeSquareColor(int Sqindex, float renge)	//座標から範囲をマンハッタン距離で計算
{
	for (int i = 0; i < m_InRangeIDList.size(); i++)
	{
		m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::NotCursor);
	}

	m_InRangeIDList.clear();

	SearchInRengeSquare(Sqindex, renge, m_InRangeIDList);

	for (int i = 0; i < m_InRangeIDList.size(); i++)
	{
		switch (m_Mode)
		{
		default:
			break;
		case Mode::MoveMode:
			m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::MoveCursor);
			break;
		case Mode::AttackMode:
			m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::AttackCursor);
			if (m_FieldSquaresList[m_InRangeIDList[i]]->chara != nullptr)
			{
				if (m_FieldSquaresList[m_InRangeIDList[i]]->chara->GetAdmin() == Admin::Imperial)
				{
					m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::EnemyCursor);
				}
			}
			break;
		case Mode::AbilityMode:
			if (m_TargetMode == TargetMode::EnemyTarget)
			{
				m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::AttackCursor);
				if (m_FieldSquaresList[m_InRangeIDList[i]]->chara != nullptr)
				{
					if (m_FieldSquaresList[m_InRangeIDList[i]]->chara->GetAdmin() == Admin::Imperial)
					{
						m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::EnemyCursor);
					}
				}
			}
			break;
		}
	}
}

void BattleFieldManager::SetEnemyRengeSquareTexture()
{
	m_EnemyRangeIDList.clear();

	for (int i = 0; i < m_EnemyCharacterList.size(); i++)
	{
		if (!m_EnemyCharacterList[i]->GetIsDead())
		{
			auto renge = m_EnemyCharacterList[i]->GetAttackRenge();
			auto enemyPosition = m_EnemyCharacterList[i]->GetCharacterPosOnSquares();

			SearchInRengeSquare(enemyPosition, renge, m_EnemyRangeIDList);
		}
	}

	for (int i = 0; i < m_EnemyRangeIDList.size(); i++)
	{
		if (std::find(m_InRangeIDList.begin(), m_InRangeIDList.end(), m_EnemyRangeIDList[i]) != m_InRangeIDList.end())
		{
			m_FieldSquaresList[m_EnemyRangeIDList[i]]->SetSquaresColor(SquareColor::EnemyRengeCursor);
		}
	}
}

void BattleFieldManager::ResetFieldFromMove()	//移動モードになった際などの後にマスの色を元に戻す処理
{
	for (int i = 0; i < m_ChangedSquaresList.size(); i++)
	{
		m_ChangedSquaresList[i]->SetSquaresColor(SquareColor::NotCursor);
	}
	for (int i = 0; i < m_PassedSquaresList.size(); i++)
	{
		m_PassedSquaresList[i]->SetSquaresColor(SquareColor::NotCursor);
	}
	for (int i = 0; i < m_InRangeIDList.size(); i++)
	{
		m_FieldSquaresList[m_InRangeIDList[i]]->SetSquaresColor(SquareColor::NotCursor);
	}
}

void BattleFieldManager::ChangeTurn()
{
	EnemyAIManager* AIMng = static_cast<EnemyAIManager*>(MyAccessHub::GetAIManager());
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	m_PreviousTurn = m_CurrentTurn;
	switch (m_PreviousTurn)
	{
	case Turn::Allies:
		m_Firsttime = false;
		for (int i = 0; i < m_AlliesCharacterList.size(); i++)
		{
			m_AlliesCharacterList[i]->SetIsActioned(false);
		}
		CheckMoved();
		m_HUDManager->GetHUDObject("TurnHUD")->SetAnimationState(AnimationState::Init);
		p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
		ChangeTurnEnemy();
		break;
	case Turn::EnemyAction:
		AIMng->OnChangeTurn();
		for (int i = 0; i < m_EnemyCharacterList.size(); i++)
		{
			m_EnemyCharacterList[i]->SetIsActioned(false);
		}
		CheckMoved();
		m_HUDManager->GetHUDObject("TurnHUD")->SetAnimationState(AnimationState::Init);
		ChangeTurnAllies();
		break;
	default:
		break;
	}
}

void BattleFieldManager::ResetHUDs(int SEindex)
{
	MyGameEngine* p_engine = MyAccessHub::GetMyGameEngine();

	m_HUDManager->ResetHUDWhenMoveCursor();
	m_HUDManager->GetHUDObject("CurrentTerrainHUD")->ResetHUD();
	

	if (SEindex != -1)
	{
		p_engine->GetSoundManager()->play(SEindex);
	}

	UpdateBattleField();
}

void BattleFieldManager::SetPlayerWin(bool win)
{
	m_PlayerWin = win;
}

void BattleFieldManager::SetCameraChangerCompornent(CameraChangerComponent* cameraChanger)
{
	m_CameraChangerCompornent = cameraChanger;
}

void BattleFieldManager::SetStrengthValues()
{
	float aliesStrength = 0.0f;
	float enemyStrength = 0.0f;

	for (int i = 0; i < m_AlliesCharacterList.size(); i++)
	{
		aliesStrength += m_AlliesCharacterList[i]->GetSoldiers() / m_AlliesCharacterList[i]->GetMaxSoldiers();	//味方の全体のHP割合
	}
	for (int i = 0; i < m_EnemyCharacterList.size(); i++)
	{
		enemyStrength += m_EnemyCharacterList[i]->GetSoldiers() / m_EnemyCharacterList[i]->GetMaxSoldiers();	//敵の全体のHP割合
	}

	m_StrengthValue.x = aliesStrength;
	m_StrengthValue.y = enemyStrength;
}

void BattleFieldManager::AddTurnCount()
{
	m_TurnCount++;
}

void BattleFieldManager::CreateMoveLog(Platoon* currentCharacter, int currentPos, int nextPos)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->GetPlatoonID();
	playerActionLog.m_ActionName = ActionName::Move;
	playerActionLog.m_DamageDealt = 0;
	playerActionLog.m_MoveForward = nextPos / 10 - currentPos / 10;
	playerActionLog.m_HPparcentage = currentCharacter->GetSoldiers() / currentCharacter->GetMaxSoldiers();

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::CreateAttackLog(Platoon* currentCharacter, float damage)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->GetPlatoonID();
	playerActionLog.m_ActionName = ActionName::Attack;
	playerActionLog.m_DamageDealt = damage;
	playerActionLog.m_MoveForward = 0;
	playerActionLog.m_HPparcentage = currentCharacter->GetSoldiers() / currentCharacter->GetMaxSoldiers();

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::CreateAbilityLog(Platoon* currentCharacter, ActionName abilityName, float damage)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->GetPlatoonID();
	playerActionLog.m_ActionName = abilityName;
	playerActionLog.m_DamageDealt = damage;
	playerActionLog.m_MoveForward = 0;
	playerActionLog.m_HPparcentage = currentCharacter->GetSoldiers() / currentCharacter->GetMaxSoldiers();

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::CreateWaitLog(Platoon* currentCharacter)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->GetPlatoonID();
	playerActionLog.m_ActionName = ActionName::Wait;
	playerActionLog.m_DamageDealt = 0;
	playerActionLog.m_MoveForward = 0;
	playerActionLog.m_HPparcentage = currentCharacter->GetSoldiers() / currentCharacter->GetMaxSoldiers();

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::SetFirstAlliesCharacterCamera()
{
	FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();

	Fcam->FocusFirstAlliesCharacter();	//最初の味方キャラにカメラをフォーカス
	m_SelectPos[(int)Vector::X] = m_AlliesCharacterList[0]->GetCharacterPosOnSquares() % 10;
	m_SelectPos[(int)Vector::Y] = m_AlliesCharacterList[0]->GetCharacterPosOnSquares() / 10;
}



/// <summary>
///	現在経過したターン数を取得
/// </summary>
int BattleFieldManager::GetTurnCount()
{
	return m_TurnCount;
}

/// <summary>
///	プレイヤーが倒した部隊の数を取得
/// </summary>
int BattleFieldManager::GetPlayerKillCount()
{
	return m_PlayerKillCount;
}

/// <summary>
///	敵が倒した部隊の数を取得
/// </summary>
int BattleFieldManager::GetEnemyKillCount()
{
	return m_EnemyKillCount;
}

void BattleFieldManager::SetStatusText(StatusWords* StatusWords)
{
    m_StatusText = StatusWords;
}

void BattleFieldManager::SetTerrainHUD(TerrainHUD* TerrainHUD)
{
    m_TerrainHUD = TerrainHUD;
}

void BattleFieldManager::SetMenuText(MenuText* MenuText)
{
    m_MenuText = MenuText;
}

void BattleFieldManager::SetLogHUD(LogHUD* LogHUD)
{
    m_LogHUD = LogHUD;
}

void BattleFieldManager::SetLogHUDText(LogHUDW* LogHUDW)
{
    m_LogHUDText = LogHUDW;
}

void BattleFieldManager::SetDamageHUD(DamageHUD* DamageHUD)
{
    m_DamageHUD = DamageHUD;
}

void BattleFieldManager::SetDamageHUDText(DamageHUDW* DamageHUDW)
{
    m_DamageHUDText = DamageHUDW;
}

void BattleFieldManager::SetTurnEndUI(TurnEndUI* TurnEndUI)
{
    m_TurnEndUI = TurnEndUI;
}

void BattleFieldManager::SetOpeningAnimHUD(OpeningAnimHUD* OpeningAnimHUD)
{
    m_OpeningAnimHUD = OpeningAnimHUD;
}

void BattleFieldManager::SetResultUI(ResultUI* ResultUI)
{
    m_ResultUI = ResultUI;
}

void BattleFieldManager::SetMovedCountHUD(MovedCountHUD* MovedCountHUD)
{
    m_MovedCountHUD = MovedCountHUD;
}

void BattleFieldManager::SetTurnUI(TurnUI* TurnUI)
{
    m_TurnUI = TurnUI;
}

void BattleFieldManager::SetCurrentTurn(Turn turn)
{
	if (m_CurrentTurn != Turn::Result)
	{
		m_CurrentTurn = turn;
	}
}

void BattleFieldManager::AddFieldSquare(Squares* square)
{
	m_FieldSquaresList.push_back(square);
}

void BattleFieldManager::SetAlliesCharacterList(vector<Platoon*> list)
{
	m_AlliesCharacterList = list;
}

void BattleFieldManager::SetEnemyCharacterList(vector<Platoon*> list)
{
	m_EnemyCharacterList = list;
}

void BattleFieldManager::SetAbiliteis(Abilities* abilities)
{
	m_Abilities = abilities;
}

XMFLOAT3 BattleFieldManager::GetFirstAlliesCharacterPos()
{
	if (m_AlliesCharacterList.size() > 0)
	{
		return m_FieldSquaresList[m_AlliesCharacterList[0]->GetCharacterPosOnSquares()]->GetSquarePosition();
	}
	else
	{
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

void BattleFieldManager::ChangeTurnAllies()
{
	SetCurrentTurn(Turn::Allies);
}

void BattleFieldManager::ChangeTurnEnemy()
{
	SetCurrentTurn(Turn::Enemy);
}

void BattleFieldManager::Attack(Platoon* attackingchara, Platoon* attackedchara)
{
	m_AttackedCharacter = attackedchara;

	int damage = CalculateDamage(ActionName::Attack, attackingchara, attackedchara);

	//m_DamageHUD->SetDamage(damage, attackedchara->GetMaxSoldiers(), attackedchara->GetSoldiers());	//ダメージUIを起動

	HUDObject* damageUIobj = m_HUDManager->GetHUDObject("DamageUI");
	if (DamageUI* damageUI = dynamic_cast<DamageUI*>(damageUIobj))
	{
		damageUI->SetDamage(damage, attackedchara->GetMaxSoldiers(), attackedchara->GetSoldiers());
	}

	attackedchara->SetSoldier(attackedchara->GetSoldiers() - damage);
	attackingchara->SetIsActioned(true);

	m_IsAttacking = true;

	CreateAttackLog(attackingchara, damage);	//プレイヤーのログを制作

	SetStrengthValues();
}

void BattleFieldManager::Move(int nowPos, int nextPos, float charaID)
{
	m_FieldSquaresList[nextPos]->ThereCharaID = charaID;							//移動先のマスに先にIDを入れる
	m_FieldSquaresList[nextPos]->SqAdmin = m_FieldSquaresList[nowPos]->SqAdmin;		//マスの勢力も変える(後々制圧とかの要素を追加する)

	//味方と敵がそれぞれ同じIDを持っている為、それぞれの処理を行う
	if (m_FieldSquaresList[nextPos]->SqAdmin == Admin::Rebel)						
	{
		m_FieldSquaresList[nextPos]->chara = m_AlliesCharacterList[charaID];
		if (nextPos / 140 > 1.0f)	//クリア条件
		{
			SetResult(true);
		}
	}
	else if (m_FieldSquaresList[nextPos]->SqAdmin == Admin::Imperial)
	{
		m_FieldSquaresList[nextPos]->chara = m_EnemyCharacterList[charaID];
	}
	
	m_FieldSquaresList[nextPos]->chara->SetIsActioned(true);								//行動済み
	m_FieldSquaresList[nextPos]->chara->SetCharacterPosOnSquares(nextPos);							//部隊のデータ自身のいる座標も更新
	m_FieldSquaresList[nowPos]->ThereCharaID = -1;									//前にいたマスを空にする
	m_FieldSquaresList[nowPos]->SqAdmin = Admin::None;

	for (int i = 0; i < m_PassedSquaresList.size(); i++)							//移動可能距離を計算するときに使ったリストをクリア
	{
		m_PassedSquaresList[i]->passed = false;
	}
	m_PassedSquaresList.clear();

	CreateMoveLog(m_AlliesCharacterList[charaID], nowPos, nextPos);					//プレイヤーのログを制作
}

void BattleFieldManager::Wait(int nowPos)
{
	m_FieldSquaresList[nowPos]->chara->SetIsActioned(true);																//行動済み
	m_FieldSquaresList[nowPos]->chara->SetMorale(m_FieldSquaresList[nowPos]->chara->GetMorale() + m_FieldSquaresList[nowPos]->chara->GetMaxMorale() * 0.2);	//待機すると士気が回復する
	if (m_FieldSquaresList[nowPos]->chara->GetMorale() > m_FieldSquaresList[nowPos]->chara->GetMaxMorale())
	{
		m_FieldSquaresList[nowPos]->chara->SetMorale(m_FieldSquaresList[nowPos]->chara->GetMaxMorale());
	}

	CreateWaitLog(m_FieldSquaresList[nowPos]->chara);																//プレイヤーのログを制作

	CheckMoved();
	UpdateBattleField();
}
