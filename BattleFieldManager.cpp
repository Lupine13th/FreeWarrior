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
			m_Lifecount = 0;
			for (int i = 0; i < m_AlliesCharacterList.size(); i++)	//現在生き残っているユニットの数を算出
			{
				if (!m_AlliesCharacterList[i]->Dead)
				{
					m_Lifecount++;	
				}
			}
			m_Firsttime = true;
			UpdateBattleField();					//マスの色を調整
			ResetPlayerActionLogs();				//プレイヤー行動傾向の記録をリセット
			m_CursorState = CursorState::Select;	//カーソルを「選択中」にする
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
					if (m_FieldSquaresList[m_SelectID]->chara != nullptr && m_FieldSquaresList[m_SelectID]->chara->CharaAdmin == Admin::Rebel && !m_FieldSquaresList[m_SelectID]->chara->Moved && !m_FieldSquaresList[m_SelectID]->chara->Dead)	//カーソルが味方部隊をさしている時
					{
						p_engine->GetSoundManager()->play(3);
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
							if (!m_AlliesCharacterList[i]->Dead )
							{
								Wait(m_AlliesCharacterList[i]->CharaPos);
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

							SetInRengeSquareColor(m_SelectID, m_FieldSquaresList[m_SelectID]->chara->CharaMoveRenge);

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
					ResetHUDs(2);
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを入力
				{
					if (m_FieldSquaresList[m_TargetID]->chara != nullptr && m_FieldSquaresList[m_TargetID]->chara->CharaAdmin != m_FieldSquaresList[m_SelectID]->chara->CharaAdmin)	//ターゲットカーソルが敵のいる位置にある
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
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキーを入力
				{
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
					m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Move, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);

					//移動処理
					Move(m_SelectID, m_TargetID, m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->CharaID);	

					m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
					ResetFieldFromMove();
					m_Mode = Mode::FieldMode;
					m_FieldSquaresList[m_TargetID]->chara->Moved = true;

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
						if (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[i] != AbilityType::None)
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
						if (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[m_AbillityIndex] != AbilityType::None)	//ヌルチェック
						{
							switch (m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[m_AbillityIndex])				//選択中のアビリティに応じてカーソルの種類を変える(現在は敵をターゲットとする行動しかない)
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
						ResetHUDs(2);
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))			//スペースキーを押したとき
					{
						if (m_FieldSquaresList[m_TargetID]->chara != nullptr && m_FieldSquaresList[m_TargetID]->chara->CharaAdmin != m_FieldSquaresList[m_SelectID]->chara->CharaAdmin)
						{
							SetAttackingCharacterSquares(m_FieldSquaresList[m_SelectID]);
							switch ((m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID]->Abilities[m_AbillityIndex]))				//選択中のアビリティに応じて戦闘アニメーションを切り替える
							{
							case AbilityType::Scout:
								m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Scout, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
								//p_engine->GetSoundManager()->play(9);
								ResetHUDs(9);
								break;
							case AbilityType::ConcentratedFire:
							case AbilityType::BayonetCharge:
								m_FieldSquaresList[m_SelectID]->SetAnimation(Animations::Attack, m_FieldSquaresList[m_SelectID]->chara->CharaAdmin, m_FieldSquaresList[m_SelectID], m_FieldSquaresList[m_TargetID]);
								//p_engine->GetSoundManager()->play(3);
								ResetHUDs(3);
								break;
							}
							
							Abiliting(m_AlliesCharacterList[m_FieldSquaresList[m_SelectID]->ThereCharaID], m_EnemyCharacterList[m_FieldSquaresList[m_TargetID]->ThereCharaID]);
							m_FieldSquaresList[m_TargetID]->SetSquaresColor(SquareColor::NotCursor);
							ResetFieldFromMove();
							m_FieldSquaresList[m_SelectID]->chara->Moved = true;
							ResetAbillityMenu();
							UpdateBattleField();
						}
					}
					else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))		//エスケープキーを押したとき
					{
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
				if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_RIGHT))			//十字キー右を押したとき
				{
					m_TurnEnd = true;																												//ターンエンド処理へ
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_LEFT))		//十字キー左を押したとき
				{
					m_TurnEnd = false;																												//一旦戻る(現状、意味は無い)
				}
				else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))				//スペースキーを押したとき
				{
					switch (m_TurnEnd)
					{
					case false:
						m_Mode = Mode::FieldMode;																									//一旦戻る(現状、意味は無い)
						break;
					case true:
						m_Mode = Mode::FieldMode;																									//ターンエンド処理
						MyAccessHub::GetAIManager()->CreateLearningData(m_PlayerActionLogs);														//学習データを作成
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
		auto charaPositionSquare = m_FieldSquaresList[m_AlliesCharacterList[i]->CharaPos]->SqPos;

		if (!m_AlliesCharacterList[i]->Moved && !m_AlliesCharacterList[i]->Dead && m_CurrentTurn == Turn::Allies)	//移動していない味方キャラがいる場合、サークルエフェクトを再生
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
			m_LogHUD->m_Sprites[m_LogHUDText->LogCount]->SetColor(0.5f, 0.5f, 0.7f, 1);
			logColors[m_LogHUDText->LogCount] = XMFLOAT4(0.5f, 0.5f, 0.7f, 1);
			break;
		case Admin::Imperial:
			m_LogHUD->m_Sprites[m_LogHUDText->LogCount]->SetColor(0.7f, 0.5f, 0.5f, 1);
			logColors[m_LogHUDText->LogCount] = XMFLOAT4(0.7f, 0.5f, 0.5f, 1);
			break;
		case Admin::None:
			m_LogHUD->m_Sprites[m_LogHUDText->LogCount]->SetColor(0.9f, 0.9f, 0.9f, 1);
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
			m_LogHUD->m_Sprites[i - 1]->SetColor(logColors[i].x, logColors[i].y, logColors[i].z, logColors[i].w);
			logColors[i - 1] = logColors[i];
		}
		m_LogHUDText->ctrs[7] = logs;
		switch (actionchara->CharaAdmin)
		{
		default:
			break;
		case Admin::Rebel:
			m_LogHUD->m_Sprites[7]->SetColor(0.5f, 0.5f, 0.7f, 1);
			logColors[7] = XMFLOAT4(0.5f, 0.5f, 0.7f, 1);
			break;
		case Admin::Imperial:
			m_LogHUD->m_Sprites[7]->SetColor(0.7f, 0.5f, 0.5f, 1);
			logColors[7] = XMFLOAT4(0.7f, 0.5f, 0.5f, 1);
			break;
		case Admin::None:
			m_LogHUD->m_Sprites[7]->SetColor(0.9f, 0.9f, 0.9f, 1);
			logColors[7] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1);
			break;
		}
	}
}

void BattleFieldManager::DeleteChara(int deadCharaPos)
{
	MyGameEngine* p_engine = MyAccessHub::GetMyGameEngine();

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
	if (chara->Dead)
	{
		return;
	}

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
		aliesStrength += m_AlliesCharacterList[i]->CharaSoldiers / m_AlliesCharacterList[i]->CharaMaxSoldiers;	//味方の全体のHP割合
	}
	for (int i = 0; i < m_EnemyCharacterList.size(); i++)
	{
		enemyStrength += m_EnemyCharacterList[i]->CharaSoldiers / m_EnemyCharacterList[i]->CharaMaxSoldiers;	//敵の全体のHP割合
	}

	m_StrengthValue.x = aliesStrength;
	m_StrengthValue.y = enemyStrength;
}

void BattleFieldManager::AddTurnCount()
{
	m_TurnCount++;
}

void BattleFieldManager::CreateMoveLog(FieldCharacter* currentCharacter, int currentPos, int nextPos)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->CharaID;
	playerActionLog.m_ActionName = ActionName::Move;
	playerActionLog.m_DamageDealt = 0;
	playerActionLog.m_MoveForward = nextPos / 10 - currentPos / 10;
	playerActionLog.m_HPparcentage = currentCharacter->CharaSoldiers / currentCharacter->CharaMaxSoldiers;

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::CreateAttackLog(FieldCharacter* currentCharacter, float damage)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->CharaID;
	playerActionLog.m_ActionName = ActionName::Attack;
	playerActionLog.m_DamageDealt = damage;
	playerActionLog.m_MoveForward = 0;
	playerActionLog.m_HPparcentage = currentCharacter->CharaSoldiers / currentCharacter->CharaMaxSoldiers;

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::CreateAbilityLog(FieldCharacter* currentCharacter, ActionName abilityName, float damage)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->CharaID;
	playerActionLog.m_ActionName = abilityName;
	playerActionLog.m_DamageDealt = damage;
	playerActionLog.m_MoveForward = 0;
	playerActionLog.m_HPparcentage = currentCharacter->CharaSoldiers / currentCharacter->CharaMaxSoldiers;

	m_PlayerActionLogs.push_back(playerActionLog);
}

void BattleFieldManager::CreateWaitLog(FieldCharacter* currentCharacter)
{
	PlayerActionLog playerActionLog;

	playerActionLog.m_CharacterID = currentCharacter->CharaID;
	playerActionLog.m_ActionName = ActionName::Wait;
	playerActionLog.m_DamageDealt = 0;
	playerActionLog.m_MoveForward = 0;
	playerActionLog.m_HPparcentage = currentCharacter->CharaSoldiers / currentCharacter->CharaMaxSoldiers;

	m_PlayerActionLogs.push_back(playerActionLog);
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

	m_AttackedCharacter = attackedchara;	//防御している部隊をメンバ変数に代入(後からCheckDeadを行うため)

	if (m_FieldSquaresList[attackingchara->CharaPos]->terrainname == Terrain::Forest || m_FieldSquaresList[attackingchara->CharaPos]->terrainname == Terrain::Tower)
	{
		damage = attackingchara->CharaPower * 1.2;	//攻撃側が森林、監視塔の地形にいる場合、ダメージ増加
	}
	else
	{
		damage = attackingchara->CharaPower;
	}

	if (attackedchara->CharaDiffence != 0.0f)
	{
		damage = damage * (1.0f - attackedchara->CharaDiffence / 100);	//部隊の装甲値に応じてダメージ減衰
	}
	

	if (!attackedchara->Detected)
	{
		damage = damage * 0.75;	//偵察出来ていない場合、ダメージ減衰
	}
	if (m_FieldSquaresList[attackedchara->CharaPos]->terrainname == Terrain::Hills || m_FieldSquaresList[attackedchara->CharaPos]->terrainname == Terrain::River)
	{
		damage = damage * 0.75;	//防御側が丘陵、河川地形にいる場合、ダメージ減衰
	}

	if (damage > attackedchara->CharaSoldiers)
	{
		damage = attackedchara->CharaSoldiers;	//オーバーフローを起こさないように
	}

	m_DamageHUD->SetDamage(damage, attackedchara->CharaMaxSoldiers, attackedchara->CharaSoldiers);	//ダメージUIを起動

	attackedchara->CharaSoldiers = attackedchara->CharaSoldiers - damage;
	attackingchara->Moved = true;

	m_IsAttacking = true;

	CreateAttackLog(attackingchara, damage);	//プレイヤーのログを制作

	SetStrengthValues();

	RefreshLogs(attackingchara, attackedchara, ActionName::Attack, damage, false);
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
	
	m_FieldSquaresList[nextPos]->chara->Moved = true;								//行動済み
	m_FieldSquaresList[nextPos]->chara->CharaPos = nextPos;							//部隊のデータ自身のいる座標も更新
	m_FieldSquaresList[nowPos]->ThereCharaID = -1;									//前にいたマスを空にする
	m_FieldSquaresList[nowPos]->SqAdmin = Admin::None;

	for (int i = 0; i < m_PassedSquaresList.size(); i++)							//移動可能距離を計算するときに使ったリストをクリア
	{
		m_PassedSquaresList[i]->passed = false;
	}
	m_PassedSquaresList.clear();

	CreateMoveLog(m_AlliesCharacterList[charaID], nowPos, nextPos);					//プレイヤーのログを制作

	RefreshLogs(m_FieldSquaresList[nextPos]->chara, m_FieldSquaresList[nextPos]->chara, ActionName::Move, 0, false);
}

void BattleFieldManager::Wait(int nowPos)
{
	m_FieldSquaresList[nowPos]->chara->Moved = true;																//行動済み
	m_FieldSquaresList[nowPos]->chara->CharaMorales += m_FieldSquaresList[nowPos]->chara->CharaMaxMorales * 0.2;	//待機すると士気が回復する
	if (m_FieldSquaresList[nowPos]->chara->CharaMorales > m_FieldSquaresList[nowPos]->chara->CharaMaxMorales)
	{
		m_FieldSquaresList[nowPos]->chara->CharaMorales = m_FieldSquaresList[nowPos]->chara->CharaMaxMorales;
	}

	CreateWaitLog(m_FieldSquaresList[nowPos]->chara);																//プレイヤーのログを制作

	CheckMoved();
	UpdateBattleField();
}
