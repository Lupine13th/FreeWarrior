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

#include <cmath> 
#include <chrono>
#include <algorithm>

#define DEBUG_FLOAT(val) { char buf[128]; sprintf_s(buf, "%f\n", val); OutputDebugStringA(buf); }

using namespace std::chrono;

void BattleFieldManager::initAction()
{
	MyGameEngine* p_engine = MyAccessHub::getMyGameEngine();
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());

	m_AlliesCharacterList.resize(5);
	m_EnemyCharacterList.resize(5);

	switch (p_scene->kPlayStates)
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

	p_engine->GetSoundManager()->playBGM(0);
}

bool BattleFieldManager::frameAction()
{
	MyGameEngine* p_engine = MyAccessHub::getMyGameEngine();

	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());

	KeyBindComponent* keycomp = static_cast<KeyBindComponent*>(p_scene->getKeyComponent());

	FlyingCameraController* Fcam = MyAccessHub::GetFlyingCameraController();

	if (m_CurrentTurn == Turn::First)
	{
		if (m_OpeningAnimHUD->OPAnimCount > 5.2f)
		{
			SetCurrentTurn(Turn::Allies);
		}
	}
	else if (m_CurrentTurn == Turn::Allies)
	{
		if (!m_Firsttime)
		{
			m_Lifecount = 0;
			for (int i = 0; i < m_AlliesCharacterList.size(); i++)
			{
				if (!m_AlliesCharacterList[i]->Dead)
				{
					m_Lifecount++;
				}
			}
			m_Firsttime = true;
			UpdateBattleField();
			m_MovedCountHUD->ResetMovedCountUI();
			m_CursorState = CursorState::Select;
		}

		switch (keycomp->getCurrentInputType())
		{
		case KeyBindComponent::INPUT_TYPES::KEYBOARD:
		{
			switch (m_Mode)
			{
				//==================================Fieldモード=====================================
			case Mode::FieldMode:
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))
				{
					m_SelectPos[(int)Vector::X]--;
					if (m_SelectPos[(int)Vector::X] < 0)
					{
						m_SelectPos[(int)Vector::X] = 0;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))
				{
					m_SelectPos[(int)Vector::X]++;
					if (m_SelectPos[(int)Vector::X] > 9)
					{
						m_SelectPos[(int)Vector::X] = 9;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))
				{
					m_SelectPos[(int)Vector::Y]--;
					if (m_SelectPos[(int)Vector::Y] < 0)
					{
						m_SelectPos[(int)Vector::Y] = 0;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))
				{
					m_SelectPos[(int)Vector::Y]++;
					if (m_SelectPos[(int)Vector::Y] > 14)
					{
						m_SelectPos[(int)Vector::Y] = 14;
					}
					ResetHUDs(-1);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
				{
					if (!m_MenuUI->MenuUIenable && m_FieldSquaresList[m_SelectID]->chara != nullptr && m_FieldSquaresList[m_SelectID]->chara->CharaAdmin == Admin::Rebel && !m_FieldSquaresList[m_SelectID]->chara->Moved && !m_FieldSquaresList[m_SelectID]->chara->Dead)
					{
						p_engine->GetSoundManager()->play(3);
						m_MenuUI->MenuUIenable = true;
						m_MenuUI->OpenMenuAnim();
						m_MenuSelectIndex = 0;

						m_InRangeIDListForMenu.clear();	//攻撃範囲内のマスIDリストをクリア

						SearchInRengeSquare(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->CharaRenge, m_InRangeIDListForMenu);	//攻撃範囲内のマスIDリストを取得

						m_InRangeEnemyList.clear();	//攻撃範囲内の敵のマスIDリストをクリア

						for (int i = 0; i < m_InRangeIDListForMenu.size(); i++)	//攻撃範囲内の敵のマスIDリストを取得
						{
							if (m_FieldSquaresList[m_InRangeIDListForMenu[i]]->chara != nullptr)
							{
								if (m_FieldSquaresList[m_InRangeIDListForMenu[i]]->chara->CharaAdmin == Admin::Imperial)
								{
									m_InRangeEnemyList.push_back(m_InRangeIDListForMenu[i]);
								}
							}
						}

						if (m_InRangeEnemyList.size() > 0)	//攻撃範囲内に敵がいる場合
						{
							m_MenuSelectIndex = 0;
						}
						else								//攻撃範囲内に敵がいない場合
						{
							m_MenuSelectIndex = 1;
						}

						m_MenuSelectUI->OpenAnim();
						m_MenuText->OpenMenuText();
						m_MovedCountHUD->ResetAnimCount();
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
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))
				{
					m_Mode = Mode::SideMenuMode;
					m_SideMenuSelectIndex = 0;
				}

				//=============================デバッグ用ボタン=================================
				else if (p_scene->kPlayStates == PlayStates::Debug || p_scene->kPlayStates == PlayStates::Release)
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
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))
				{
					m_SideMenuSelectIndex--;
					if (m_SideMenuSelectIndex < 0)
					{
						m_SideMenuSelectIndex = 0;
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))
				{
					m_SideMenuSelectIndex++;
					if (m_SideMenuSelectIndex > 2)
					{
						m_SideMenuSelectIndex = 2;
					}
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
				{
					switch (m_SideMenuSelectIndex)
					{
					case 0:
						for (int i = 0; i < m_AlliesCharacterList.size(); i++)
						{
							if (m_AlliesCharacterList[i] != nullptr)
							{
								Wait(m_AlliesCharacterList[i]->CharaPos);
							}
						}
						m_Mode = Mode::FieldMode;
						UpdateBattleField();
						break;
					case 1:
						PostQuitMessage(0);
						break;
					case 2:
						m_Mode = Mode::FieldMode;
						break;
					}
				}
				break;


				//==================================Menuモード=====================================
			case Mode::MenuMode:
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))
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
					m_MenuSelectUI->ResetAnim();
					p_engine->GetSoundManager()->play(2);
				}

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))
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
					
					m_MenuSelectUI->ResetAnim();
					p_engine->GetSoundManager()->play(2);
				}

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
				{
					if (m_FieldSquaresList[m_SelectID]->ThereCharaID != -1)
					{
						switch (m_MenuSelectIndex)
						{
						default:
							break;
						case 0:
							m_Mode = Mode::AttackMode;

							m_TargetID = m_SelectID;
							m_TargetPos[(int)Vector::X] = m_SelectPos[(int)Vector::X];
							m_TargetPos[(int)Vector::Y] = m_SelectPos[(int)Vector::Y];

							m_SelectingSquare = m_FieldSquaresList[m_SelectID];

							UpdateBattleField();

							//moniUI->MenuUIenable = false;

							m_MenuUI->CloseMenuAnim();

							m_MenuSelectUI->CloseAnim();

							p_engine->GetSoundManager()->play(3);
							break;
						case 1:
							m_Mode = Mode::MoveMode;

							m_TargetID = m_SelectID;
							m_TargetPos[(int)Vector::X] = m_SelectPos[(int)Vector::X];
							m_TargetPos[(int)Vector::Y] = m_SelectPos[(int)Vector::Y];

							m_PassedSquaresList.clear();

							m_FieldSquaresList[m_TargetID]->passed = true;
							m_PassedSquaresList.push_back(m_FieldSquaresList[m_TargetID]);

							m_SelectingSquare = m_FieldSquaresList[m_SelectID];

							UpdateBattleField();

							SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->CharaMoveRenge);

							SetEnemyRengeSquareTexture();

							m_MenuUI->CloseMenuAnim();

							m_MenuSelectUI->CloseAnim();

							p_engine->GetSoundManager()->play(3);
							break;
						case 2:
							m_Mode = Mode::AbilityMode;

							m_TargetID = m_SelectID;
							m_TargetPos[(int)Vector::X] = m_SelectPos[(int)Vector::X];
							m_TargetPos[(int)Vector::Y] = m_SelectPos[(int)Vector::Y];

							m_SelectingSquare = m_FieldSquaresList[m_SelectID];

							UpdateBattleField();

							m_AbillityMenuState = AbillityMenuState::Menu;

							m_MenuUI->CloseMenuAnim();
							m_MenuSelectUI->CloseAnim();

							m_HUDManager->GetHUDObject("AbilityHUD")->ResetHUD();

							p_engine->GetSoundManager()->play(3);
							break;
						case 3:
							m_Mode = Mode::FieldMode;
							m_MenuSelectUI->ResetAnim();
							m_MenuUI->CloseMenuAnim();
							m_MenuSelectUI->CloseAnim();

							Wait(m_SelectID);

							p_engine->GetSoundManager()->play(3);
							break;
						case 4:
							m_Mode = Mode::FieldMode;
							m_MenuSelectUI->ResetAnim();

							m_MenuUI->CloseMenuAnim();

							m_MenuSelectUI->CloseAnim();

							p_engine->GetSoundManager()->play(6);
							break;
						}

					}
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))
				{
					m_Mode = Mode::FieldMode;
					m_MenuSelectUI->ResetAnim();

					m_MenuUI->CloseMenuAnim();

					m_MenuSelectUI->CloseAnim();

					p_engine->GetSoundManager()->play(6);
				}
				break;

				//==================================Menuモード=====================================



				//==================================Attackモード=====================================
			case Mode::AttackMode:
				m_FieldSquaresList[m_SelectID]->fbxD->SetAnime(L"ATTACK01");

				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))	//左が押されたらtrue
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
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//右が押されたらtrue
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
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//左が押されたらtrue
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
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))
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
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
				{
					if (m_FieldSquaresList[m_TargetID]->chara != nullptr && m_FieldSquaresList[m_TargetID]->chara->CharaAdmin != m_FieldSquaresList[m_SelectID]->chara->CharaAdmin)
					{
						SetAttackingCharacterSquares(m_FieldSquaresList[m_SelectID]);
						m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Attack, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
						Attack(m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID], m_EnemyCharacterList[m_FieldSquaresList[m_TargetID]->ThereCharaID]);
						m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
						ResetFieldFromMove();
						m_Mode = Mode::FieldMode;
						m_FieldSquaresList[m_SelectID]->chara->Moved = true;
						ResetHUDs(3);
					}
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))
				{
					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
					m_Mode = Mode::FieldMode;
					ResetFieldFromMove();
					m_FieldSquaresList[m_SelectID]->fbxD->SetAnime(L"WAIT01");
					ResetHUDs(6);
				}
				break;

				//==================================Attackモード=====================================




				//==================================Moveモード=====================================

			case Mode::MoveMode:
				m_FieldSquaresList[m_SelectID]->fbxD->SetAnime(L"WALK01");

				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))	//左が押されたらtrue
				{
					m_TargetPos[(int)Vector::X]--;
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

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//右が押されたらtrue
				{
					m_TargetPos[(int)Vector::X]++;
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

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//左が押されたらtrue
				{
					m_TargetPos[(int)Vector::Y]--;
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

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))	//右が押されたらtrue
				{
					m_TargetPos[(int)Vector::Y]++;
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

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK) && m_SelectID != m_TargetID && m_FieldSquaresList[m_TargetID]->chara == nullptr)
				{
					m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Move, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
					Move(m_SelectID, m_TargetID, m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->CharaID);
					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
					ResetFieldFromMove();
					m_Mode = Mode::FieldMode;
					m_FieldSquaresList[m_TargetID]->chara->Moved = true;

					ResetHUDs(3);
				}

				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))
				{
					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
					ResetFieldFromMove();
					m_Mode = Mode::FieldMode;
					m_FieldSquaresList[m_SelectID]->fbxD->SetAnime(L"WAIT01");

					m_PassedSquaresList.clear();
					ResetHUDs(6);
				}
				break;

			//==================================Moveモード=====================================



			//==================================Abilityモード=====================================
			case Mode::AbilityMode:
				m_FieldSquaresList[m_SelectID]->fbxD->SetAnime(L"ATTACK01");
				switch (m_TargetMode)
				{
				case TargetMode::None:
					
					m_AbillityCount = 0;

					for (int i = 0; i < 3; i++)
					{
						if (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[i] != AbilityType::None)
						{
							m_AbillityCount++;
						}
					}

					if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))
					{
						m_AbillityIndex++;
						if (m_AbillityIndex > m_AbillityCount - 1)
						{
							m_AbillityIndex = m_AbillityCount - 1;
						}
						p_engine->GetSoundManager()->play(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))
					{
						m_AbillityIndex--;
						if (m_AbillityIndex < 0)
						{
							m_AbillityIndex = 0;
						}
						p_engine->GetSoundManager()->play(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
					{
						if (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[m_AbillityIndex] != AbilityType::None)
						{
							switch (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[m_AbillityIndex])
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
							
							m_AbillityMenuState = AbillityMenuState::Target;
							p_engine->GetSoundManager()->play(3);

							UpdateBattleField();
						}
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))
					{
						if (m_FieldSquaresList[m_TargetID]->chara != nullptr)
						{
							m_Mode = Mode::FieldMode;
							m_AbillityMenuState = AbillityMenuState::None;
							m_AbillityIndex = 0;

							p_engine->GetSoundManager()->play(6);

							UpdateBattleField();
						}
					}
					break;
				case TargetMode::EnemyTarget:
					if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))	//左が押されたらtrue
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
						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))	//右が押されたらtrue
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
						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))	//左が押されたらtrue
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
						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))
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
						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
					{
						if (m_FieldSquaresList[m_TargetID]->chara != nullptr && m_FieldSquaresList[m_TargetID]->chara->CharaAdmin != m_FieldSquaresList[m_SelectID]->chara->CharaAdmin)
						{
							SetAttackingCharacterSquares(m_FieldSquaresList[m_SelectID]);
							switch ((m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[m_AbillityIndex]))
							{
							case AbilityType::Scout:
								m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Scout, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
								p_engine->GetSoundManager()->play(9);
								break;
							case AbilityType::ConcentratedFire:
							case AbilityType::BayonetCharge:
								m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Attack, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
								p_engine->GetSoundManager()->play(3);
								break;
							}
							
							Abiliting(m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID], m_EnemyCharacterList[m_FieldSquaresList[m_TargetID]->ThereCharaID]);
							m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
							ResetFieldFromMove();
							m_FieldSquaresList[m_SelectID]->chara->Moved = true;
							ResetAbillityMenu();
							
						}
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))
					{
						m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
						ResetFieldFromMove();
						m_Mode = Mode::FieldMode;
						m_TargetMode = TargetMode::None;
						m_FieldSquaresList[m_SelectID]->fbxD->SetAnime(L"WAIT01");
						m_AbillityIndex = 0;
						ResetHUDs(6);
					}
					break;
				}
				break;

			//==================================Abilityモード=====================================


			//==================================TurnEndモード=====================================

			case Mode::TurnEndMode:
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))
				{
					m_TurnEnd = true;
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))
				{
					m_TurnEnd = false;
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
				{
					switch (m_TurnEnd)
					{
					case false:
						m_Mode = Mode::FieldMode;
						break;
					case true:
						m_Mode = Mode::FieldMode;
						m_TurnEnd = false;
						m_TurnEndUI->ResetWaitCount();
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

	if (m_IsAttacking)
	{
		m_AttackingCount += m_TimeManager->GetDeltaTime();
		if (m_AttackingCount > kMaxAttackingCount)
		{
			CheckDead(m_AttackedCharacter);
			m_AttackingCount = 0.0f;
			m_IsAttacking = false;
		}
	}

	for (int i = 0; i < m_AlliesCharacterList.size(); i++)
	{
		auto charaPositionSquare = m_FieldSquaresList[m_AlliesCharacterList[i]->CharaPos]->SqPos;

		if (!m_AlliesCharacterList[i]->Moved && m_CurrentTurn == Turn::Allies)
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Circle0" + to_wstring(i))->PlayEffect(XMFLOAT3(charaPositionSquare.x, charaPositionSquare.y + 2.0f, charaPositionSquare.z), XMFLOAT3(90.0f, 0.0f, 0.0f), 0.5f);
		}
	}

	return true;
}

void BattleFieldManager::finishAction()
{

}

void BattleFieldManager::UpdateBattleField()	//マスの色更新とターン終了の確認
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
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
		SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->CharaRenge);
		m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::AttackSellectCursor);
		m_CursorState = CursorState::Target;
		break;
	case Mode::MoveMode:
		m_TargetID = m_TargetPos[(int)Vector::X] + (10 * m_TargetPos[(int)Vector::Y]);
		SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->CharaMoveRenge);
		SetEnemyRengeSquareTexture();
		m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::PassColor);
		m_CursorState = CursorState::Target;
		break;
	case Mode::AbilityMode:
		if (m_TargetMode == TargetMode::EnemyTarget)
		{
			m_TargetID = m_TargetPos[(int)Vector::X] + (10 * m_TargetPos[(int)Vector::Y]);
			SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->CharaRenge);
			m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::AttackSellectCursor);
			m_CursorState = CursorState::Target;
		}
		break;
	}

	m_MovedCount = 0;

	for (int i = 0; i < m_AlliesCharacterList.size(); i++)
	{
		auto charaPositionSquare = m_FieldSquaresList[m_AlliesCharacterList[i]->CharaPos]->SqPos;
		if (m_AlliesCharacterList[i] != nullptr)
		{
			if (m_AlliesCharacterList[i]->Moved)
			{
				m_MovedCount++;
			}

			if (m_FieldSquaresList[m_AlliesCharacterList[i]->CharaPos]->target)
			{
				SetResult(true);
			}
		}
	}

	if (m_MovedCount == m_Lifecount)
	{
		m_Mode = Mode::TurnEndMode;
		return;
	}

	Fcam->ChangeCameraPosition();
}


XMFLOAT3 BattleFieldManager::GetCharaPos(Squares* squ)
{
	return squ->SqPos;
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

void BattleFieldManager::Abiliting(FieldCharacter* attackingchara, FieldCharacter* attackedchara)
{
	switch (attackingchara->Abilities[m_AbillityIndex])
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

void BattleFieldManager::RefreshLogs(FieldCharacter* actionchara, FieldCharacter* actionedchara, ActionName action, int damage , bool sccess)
{
	switch (action)
	{
	case ActionName::Attack:
		//攻撃ログ更新
		logs = actionchara->CharaName + L"が" + actionedchara->CharaName + L"に" + to_wstring(damage) + L"ダメージを与えた！";
		break;
	case ActionName::Move:
		//移動ログ更新
		logs = actionchara->CharaName + L"が" + to_wstring(actionchara->CharaPos) + L"へ移動した！";
		break;
	case ActionName::BayonetCharge:
		//突撃ログ更新
		logs = actionchara->CharaName + L"が" + actionedchara->CharaName + L"に突撃で" + to_wstring(damage) + L"ダメージを与えた！";
		break;
	case ActionName::ConcentratedFire:
		//集中砲火ログ更新
		logs = actionchara->CharaName + L"が" + actionedchara->CharaName + L"に集中砲火で" + to_wstring(damage) + L"ダメージを与えた！";
		break;
	case ActionName::Scout:
		//偵察ログ更新
		if (sccess)
		{
			logs = actionchara->CharaName + L"は" + actionedchara->CharaName + L"の偵察に成功した！";
		}
		else
		{
			logs = actionchara->CharaName + L"は" + actionedchara->CharaName + L"の偵察に失敗した！";
		}
		break;
	}

	if (m_LogHUDText->ctrs[7] == L"")
	{
		m_LogHUDText->ctrs[m_LogHUDText->LogCount] = logs;
		switch (actionchara->CharaAdmin)
		{
		default:
			break;
		case Admin::Rebel:
			m_LogHUD->m_sprites[m_LogHUDText->LogCount]->setColor(0.5f, 0.5f, 0.7f, 1);
			logColors[m_LogHUDText->LogCount] = XMFLOAT4(0.5f, 0.5f, 0.7f, 1);
			break;
		case Admin::Imperial:
			m_LogHUD->m_sprites[m_LogHUDText->LogCount]->setColor(0.7f, 0.5f, 0.5f, 1);
			logColors[m_LogHUDText->LogCount] = XMFLOAT4(0.7f, 0.5f, 0.5f, 1);
			break;
		case Admin::None:
			m_LogHUD->m_sprites[m_LogHUDText->LogCount]->setColor(0.9f, 0.9f, 0.9f, 1);
			logColors[m_LogHUDText->LogCount] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1);
			break;
		}
		m_LogHUDText->LogCount++;
	}
	else
	{
		for (int i = 1; i < 8; i++)
		{
			m_LogHUDText->ctrs[i - 1] = m_LogHUDText->ctrs[i];
			m_LogHUD->m_sprites[i - 1]->setColor(logColors[i].x, logColors[i].y, logColors[i].z, logColors[i].w);
			logColors[i - 1] = logColors[i];
		}
		m_LogHUDText->ctrs[7] = logs;
		switch (actionchara->CharaAdmin)
		{
		default:
			break;
		case Admin::Rebel:
			m_LogHUD->m_sprites[7]->setColor(0.5f, 0.5f, 0.7f, 1);
			logColors[7] = XMFLOAT4(0.5f, 0.5f, 0.7f, 1);
			break;
		case Admin::Imperial:
			m_LogHUD->m_sprites[7]->setColor(0.7f, 0.5f, 0.5f, 1);
			logColors[7] = XMFLOAT4(0.7f, 0.5f, 0.5f, 1);
			break;
		case Admin::None:
			m_LogHUD->m_sprites[7]->setColor(0.9f, 0.9f, 0.9f, 1);
			logColors[7] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1);
			break;
		}
	}
}

void BattleFieldManager::DeleteChara(int deadCharaPos)
{
	MyGameEngine* p_engine = MyAccessHub::getMyGameEngine();

	m_FieldSquaresList[deadCharaPos]->ThereCharaID = -1;
	m_FieldSquaresList[deadCharaPos]->SqAdmin = Admin::None;
	m_FieldSquaresList[deadCharaPos]->chara = nullptr;
	m_FieldSquaresList[deadCharaPos]->fbxD->playerData->IsActive = false;
	m_FieldSquaresList[deadCharaPos]->fbxD->setPosition(0.0f, -100.0f, 0.0f);
	m_FieldSquaresList[deadCharaPos]->fbxD = nullptr;

	p_engine->GetSoundManager()->play(7);
}

void BattleFieldManager::CheckDead(FieldCharacter* chara)
{
	if (chara->CharaSoldiers < 1.0f)
	{
		chara->CharaSoldiers = 0;
		chara->Dead = true;
		DeleteChara(chara->CharaPos);
		switch (chara->CharaAdmin)
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
		SetResult(true);
	}
	else if (m_PlayerKillCount == 5)
	{
		SetResult(false);
	}
}

void BattleFieldManager::CheckMoved()
{
	for (int i = 0; i < m_AlliesCharacterList.size(); i++)
	{
		if (m_FieldSquaresList[m_AlliesCharacterList[i]->CharaPos]->fbxD != nullptr)
		{
			auto fbxData = m_FieldSquaresList[m_AlliesCharacterList[i]->CharaPos]->fbxD;

			if (m_AlliesCharacterList[i]->Moved)
			{
				fbxData->setScale(fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2);
			}
			else
			{
				fbxData->setScale(fbxData->GetScaleValue(), fbxData->GetScaleValue(), fbxData->GetScaleValue());
			}
		}
	}

	for (int i = 0; i < m_EnemyCharacterList.size(); i++)
	{
		if (m_FieldSquaresList[m_EnemyCharacterList[i]->CharaPos]->fbxD != nullptr)
		{
			auto fbxData = m_FieldSquaresList[m_EnemyCharacterList[i]->CharaPos]->fbxD;

			if (m_EnemyCharacterList[i]->Moved)
			{
				fbxData->setScale(fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2, fbxData->GetScaleValue() / 2);
			}
			else
			{
				fbxData->setScale(fbxData->GetScaleValue(), fbxData->GetScaleValue(), fbxData->GetScaleValue());
			}
		}
	}
}

void BattleFieldManager::SetResult(bool win)
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
				if (m_FieldSquaresList[m_InRangeIDList[i]]->chara->CharaAdmin == Admin::Imperial)
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
					if (m_FieldSquaresList[m_InRangeIDList[i]]->chara->CharaAdmin == Admin::Imperial)
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
		if (!m_EnemyCharacterList[i]->Dead)
		{
			auto renge = m_EnemyCharacterList[i]->CharaRenge;
			auto enemyPosition = m_EnemyCharacterList[i]->CharaPos;

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

void BattleFieldManager::ResetFieldFromMove()
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
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	m_PreviousTurn = m_CurrentTurn;
	switch (m_PreviousTurn)
	{
	case Turn::Allies:
		m_Firsttime = false;
		m_CurrentTurn = Turn::TurnChanging;
		for (int i = 0; i < m_AlliesCharacterList.size(); i++)
		{
			m_AlliesCharacterList[i]->Moved = false;
		}
		CheckMoved();
		m_TurnUI->ChangeHUDState();
		p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
		break;
	case Turn::EnemyMove:
		AIMng->OnChangeTurn();
		m_CurrentTurn = Turn::TurnChanging;
		for (int i = 0; i < m_EnemyCharacterList.size(); i++)
		{
			m_EnemyCharacterList[i]->Moved = false;
		}
		CheckMoved();
		m_TurnUI->ChangeHUDState();
		break;
	default:
		break;
	}
}

void BattleFieldManager::ResetHUDs(int SEindex)
{
	MyGameEngine* p_engine = MyAccessHub::getMyGameEngine();

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

void BattleFieldManager::AddTurnCount()
{
	m_TurnCount++;
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

void BattleFieldManager::SetMenuUI(MenuUI* MenuUI)
{
    m_MenuUI = MenuUI;
}

void BattleFieldManager::SetMenuSelectUI(MonitorSelectUI* menuSelectUI)
{
    m_MenuSelectUI = menuSelectUI;
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

void BattleFieldManager::SetAlliesCharacterList(vector<FieldCharacter*> list)
{
	m_AlliesCharacterList = list;
}

void BattleFieldManager::SetEnemyCharacterList(vector<FieldCharacter*> list)
{
	m_EnemyCharacterList = list;
}

void BattleFieldManager::SetAbiliteis(Abilities* abilities)
{
	m_Abilities = abilities;
}

void BattleFieldManager::ChangeTurnAllies()
{
	SetCurrentTurn(Turn::Allies);
}

void BattleFieldManager::ChangeTurnEnemy()
{
	SetCurrentTurn(Turn::Enemy);
}

void BattleFieldManager::Attack(FieldCharacter* attackingchara, FieldCharacter* attackedchara)
{
	int damage = 0;

	m_AttackedCharacter = attackedchara;

	if (m_FieldSquaresList[attackingchara->CharaPos]->terrainname == Terrain::Forest || m_FieldSquaresList[attackingchara->CharaPos]->terrainname == Terrain::Tower)
	{
		damage = attackingchara->CharaPower * 1.2;
	}
	else
	{
		damage = attackingchara->CharaPower;
	}

	if (attackedchara->CharaDiffence != 0.0f)
	{
		damage = damage * (1.0f - attackedchara->CharaDiffence / 100);
	}
	

	if (!attackedchara->Detected)
	{
		damage = damage * 0.75;
	}
	if (m_FieldSquaresList[attackedchara->CharaPos]->terrainname == Terrain::Hills || m_FieldSquaresList[attackedchara->CharaPos]->terrainname == Terrain::River)
	{
		damage = damage * 0.75;
	}

	if (damage > attackedchara->CharaSoldiers)
	{
		damage = attackedchara->CharaSoldiers;
	}

	m_DamageHUD->SetDamage(damage, attackedchara->CharaMaxSoldiers, attackedchara->CharaSoldiers);

	attackedchara->CharaSoldiers = attackedchara->CharaSoldiers - damage;
	attackingchara->Moved = true;

	m_IsAttacking = true;

	RefreshLogs(attackingchara, attackedchara, ActionName::Attack, damage, false);
}

void BattleFieldManager::Move(int nowPos, int nextPos, float charaID)
{
	m_FieldSquaresList[nextPos]->ThereCharaID = charaID;
	m_FieldSquaresList[nextPos]->SqAdmin = m_FieldSquaresList[nowPos]->SqAdmin;
	if (m_FieldSquaresList[nextPos]->SqAdmin == Admin::Rebel)
	{
		m_FieldSquaresList[nextPos]->chara = m_AlliesCharacterList[charaID];
		if (nextPos / 140 > 1.0f)
		{
			SetResult(true);
		}
	}
	else if (m_FieldSquaresList[nextPos]->SqAdmin == Admin::Imperial)
	{
		m_FieldSquaresList[nextPos]->chara = m_EnemyCharacterList[charaID];
	}
	
	m_FieldSquaresList[nextPos]->chara->Moved = true;
	m_FieldSquaresList[nextPos]->chara->CharaPos = nextPos;
	m_FieldSquaresList[nowPos]->ThereCharaID = -1;
	m_FieldSquaresList[nowPos]->SqAdmin = Admin::None;

	XMFLOAT3 pos = GetCharaPos(m_FieldSquaresList[nextPos]);

	for (int i = 0; i < m_PassedSquaresList.size(); i++)
	{
		m_PassedSquaresList[i]->passed = false;
	}
	m_PassedSquaresList.clear();

	RefreshLogs(m_FieldSquaresList[nextPos]->chara, m_FieldSquaresList[nextPos]->chara, ActionName::Move, 0, false);
}

void BattleFieldManager::Wait(int nowPos)
{
	m_FieldSquaresList[nowPos]->chara->Moved = true;
	m_FieldSquaresList[nowPos]->chara->CharaMorales += m_FieldSquaresList[nowPos]->chara->CharaMaxMorales * 0.2;
	if (m_FieldSquaresList[nowPos]->chara->CharaMorales > m_FieldSquaresList[nowPos]->chara->CharaMaxMorales)
	{
		m_FieldSquaresList[nowPos]->chara->CharaMorales = m_FieldSquaresList[nowPos]->chara->CharaMaxMorales;
	}

	CheckMoved();
	UpdateBattleField();
}
