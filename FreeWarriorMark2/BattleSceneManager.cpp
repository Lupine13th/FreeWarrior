#include "BattleSceneManager.h"
#include "EffectGenerator.h"

void BattleSceneManager::InitAction()
{
	m_FlyingCameraController = MyAccessHub::GetFlyingCameraController();
	BFMng = MyAccessHub::GetBFManager();
	m_TimeManager = MyAccessHub::GetTimeManager();
}

bool BattleSceneManager::FrameAction()
{
	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());

	switch (m_SceneState)
	{
	default:
		break;
	case SceneState::CharacterSetting:
		m_SceneState = SceneState::Actioning;
		break;
	case SceneState::Actioning:

		switch (m_CurrentAnimation)
		{
		case Animations::Move:
			m_CurrentSquare->GetPlatoon()->GetPlayerBase()->SetMoveAnimation(m_CharacterCurrentPosition, m_CharacterNextPosition);

			m_FlyingCameraController->ChangeCameraPosition();	//カメラ位置をカーソルの位置へ(敵AI側が主に使う処理)	

			m_CharacterAnimationCount = 0.0f;

			m_NextSquare->SetFbxData(m_CurrentSquare->GetFBXData());								//モデルのデータも移動
			m_CurrentSquare->SetFbxData(nullptr);													//前のマスのデータを削除
			m_CurrentSquare->SetPlatoon(nullptr);													//前のマスのデータを削除

			m_SceneState = SceneState::Finish;	//アニメーションの状態を終了へ
			break;
		case Animations::Attack:
		case Animations::ConcentratedFire:
		case Animations::BayonetCharge:
			if (scene->getCameraComponent(L"AttackerCamera") == nullptr)	//カメラ切り替え
			{
				BFMng->GetCameraChangerCompornent()->SetBattleCamera();		//戦闘カメラ起動
				MyAccessHub::GetHUDManager()->GetHUDObject("DamageEffectHUD")->SetAnimationState(AnimationState::Init);	//HPバー準備段階
				SetAttackAnimation();			//攻撃側のアニメーションを設定
			}
			else	//カメラ切り替え後
			{
				if (m_CharacterAnimationCount < 1.0f)	//1秒ディレイ
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else									//1秒経過後、防御側のリアクションフェーズへ
				{
					m_CharacterAnimationCount = 0.0f;
					SetReactionAnimation();
					m_SceneState = SceneState::Reaction;	//アニメーションの状態をリアクションへ
				}
			}
			break;
		case Animations::Scout:
			if (scene->getCameraComponent(L"AttackerCamera") == nullptr)	//カメラ切り替え
			{
				BFMng->GetCameraChangerCompornent()->SetBattleCamera();				//戦闘カメラ起動
				SetAttackAnimation();												//攻撃側のアニメーションを設定

				MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scout")->PlayEffect(XMFLOAT3(25.0f, 9.7f, kOriginScoutEffectPosZ), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.5f);	//偵察エフェクトを起動
				m_ScoutEffectPosZ = kOriginScoutEffectPosZ;	//偵察エフェクトを初期値へ
			}
			else
			{
				m_ScoutEffectPosZ += kScoutEffectPosZInterval;	//Z値を更新
				MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scout")->SetEffectPosition(25.0f, 9.7f, m_ScoutEffectPosZ);	//行進した値を適応
				if (m_CharacterAnimationCount < 1.0f)																												//==========ここから下まで攻撃アニメーションと同じ==========
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else	//アニメーション中偵察成功時にダメージリアクションを行う
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
					if (BFMng->GetIsSucceedScout())
					{
						MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scouted")->PlayEffect(XMFLOAT3(25.0f, 12.0f, 40.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.3f);
						SetReactionAnimation();
					}
					m_CharacterAnimationCount = 0.0f;
					m_SceneState = SceneState::Reaction;	//アニメーションの状態をリアクションへ
				}																																				//==========ここから上まで攻撃アニメーションと同じ==========							
			}
			break;
		}
		break;

	case SceneState::Reaction:

		if (m_CurrentAnimation == Animations::Scout)
		{
			m_ScoutEffectPosZ += kScoutEffectPosZInterval;	//Z値を更新
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scout")->SetEffectPosition(25.0f, 9.7f, m_ScoutEffectPosZ);	//行進した値を適応
		}

		if (m_CharacterAnimationCount < 2.0f)	//2秒ディレイ
		{
			m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
		}
		else									//2秒経過後、アニメーション終了処理へ
		{
			SetPreviousPosition();									//元のマスの位置へ戻す
			BFMng->GetCameraChangerCompornent()->SetMainCamera();	//バトルカメラOff＆メインカメラ起動
			BFMng->CheckMoved();									//「行動済み」フラグの確認
			m_CharacterAnimationCount = 0.0f;
			if (m_AttackerFbxData != nullptr)
			{
				m_AttackerFbxData->SetAnimeInit(L"WAIT", m_AttackerCharacter);	//攻撃側のアニメーションを待機モーションへ
			}

			if (m_DefenderFbxData != nullptr)
			{
				m_DefenderFbxData->SetAnimeInit(L"WAIT", m_DefenderCharacter);	//防御側のアニメーションを待機モーションへ
			}
			m_FlyingCameraController->ChangeCameraPosition();	//カメラ位置をカーソルの位置へ	
		}
		
		break;
	case SceneState::Finish:
		break;
	}

    return true;
}

void BattleSceneManager::FinishAction()
{
}

void BattleSceneManager::SetBattlePosition(Animations anim, Platoon* attackerSquare, Platoon* defenderSquare)
{
	m_CharacterAnimationCount = 0.0f;

	m_CurrentAnimation = anim;

	m_AttackerCharacter = attackerSquare;
	m_DefenderCharacter = defenderSquare;

	m_AttackerFbxData = static_cast<FBXCharacterData*>(m_AttackerCharacter->GetPlayerBase()->GetGameObject()->GetCharacterData());
	m_DefenderFbxData = static_cast<FBXCharacterData*>(m_DefenderCharacter->GetPlayerBase()->GetGameObject()->GetCharacterData());

	m_AttackerCharacterPreviousPosRotMap =	//攻撃側FBXを戻す位置を保存
	{
		m_AttackerFbxData->GetPosition(), m_AttackerFbxData->GetRotation()
	};

	m_DefenderCharacterPreviousPosRotMap =	//防御側FBXを戻す位置を保存
	{
		m_DefenderFbxData->GetPosition(), m_DefenderFbxData->GetRotation()
	};

	//==============================位置と回転を戦闘アニメーション用に変更==============================
	m_AttackerFbxData->SetPosition(kAttackerFbxPosition.x, kAttackerFbxPosition.y, kAttackerFbxPosition.z);
	m_AttackerFbxData->SetRotation(kAttackerFbxRotation.x, kAttackerFbxRotation.y, kAttackerFbxRotation.z);

	m_DefenderFbxData->SetPosition(kDefenderFbxPosition.x, kDefenderFbxPosition.y, kDefenderFbxPosition.z);
	m_DefenderFbxData->SetRotation(kDefenderFbxRotation.x, kDefenderFbxRotation.y, kDefenderFbxRotation.z);
	//==============================位置と回転を戦闘アニメーション用に変更==============================

	m_SceneState = SceneState::CharacterSetting;
}

void BattleSceneManager::SetPreviousPosition()
{
	//==============================位置と回転を元に戻す==============================
	m_AttackerFbxData->SetPosition(m_AttackerCharacterPreviousPosRotMap.first.x, m_AttackerCharacterPreviousPosRotMap.first.y, m_AttackerCharacterPreviousPosRotMap.first.z);
	m_AttackerFbxData->SetRotation(m_AttackerCharacterPreviousPosRotMap.second.x, m_AttackerCharacterPreviousPosRotMap.second.y, m_AttackerCharacterPreviousPosRotMap.second.z);

	m_DefenderFbxData->SetPosition(m_DefenderCharacterPreviousPosRotMap.first.x, m_DefenderCharacterPreviousPosRotMap.first.y, m_DefenderCharacterPreviousPosRotMap.first.z);
	m_DefenderFbxData->SetRotation(m_DefenderCharacterPreviousPosRotMap.second.x, m_DefenderCharacterPreviousPosRotMap.second.y, m_DefenderCharacterPreviousPosRotMap.second.z);
	//==============================位置と回転を元に戻す==============================
}

void BattleSceneManager::SetMovePosition(Squares* currentSquare, Squares* nextSquare, FBXCharacterData* moveFbxData)
{
	m_CurrentSquare = currentSquare;
	m_NextSquare = nextSquare;
	m_MoveFbxData = moveFbxData;
	m_MoveCharacter = currentSquare->GetPlatoon();

	XMFLOAT3 currentPosition = currentSquare->GetSquarePosition();
	XMFLOAT3 nextPosition = nextSquare->GetSquarePosition();

	m_CharacterCurrentPosition = currentPosition;
	m_CharacterNextPosition = nextPosition;

	m_CurrentAnimation = Animations::Move;

	m_SceneState = SceneState::CharacterSetting;
}

void BattleSceneManager::SetAttackAnimation()
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	switch (m_AttackerCharacter->GetSoldiersType())
	{
	default:
		break;
	case SoldiersType::infantry:
	case SoldiersType::machinegunner:
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"MuzzleFlash")->PlayEffect(XMFLOAT3(25.0f, 9.7f, -42.2f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.1f);
		engine->GetSoundManager()->play(14);
		break;
	case SoldiersType::artillery:
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"CanonMuzzleFlash")->PlayEffect(XMFLOAT3(25.0f, 7.5f, -44.5f), XMFLOAT3(0.0f, 90.0f, 20.0f), 0.3f);
		engine->GetSoundManager()->play(16);
		break;
	case SoldiersType::scout:
		if (m_AttackerCharacter->GetAdmin() == Admin::Rebel)
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"MuzzleFlash")->PlayEffect(XMFLOAT3(25.0f, 7.5f, -44.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.1f);
		}
		else if (m_AttackerCharacter->GetAdmin() == Admin::Imperial)
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"MuzzleFlash")->PlayEffect(XMFLOAT3(25.0f, 6.0f, -45.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.1f);
		}
		engine->GetSoundManager()->play(14);
		break;
	case SoldiersType::armored:
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"MuzzleFlash")->PlayEffect(XMFLOAT3(25.0f, 6.5f, -42.2f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.2f);
		engine->GetSoundManager()->play(15);
		break;
	}
}

void BattleSceneManager::SetReactionAnimation()
{
	m_DefenderFbxData->SetAnimeInit(L"DAMAGE", m_DefenderCharacter);

	switch (m_AttackerCharacter->GetSoldiersType())
	{
	default:
		break;
	case SoldiersType::artillery:
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"LargeExplosive01")->PlayEffect(XMFLOAT3(35.0f, 10.0f, 55.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f);
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"LargeExplosive02")->PlayEffect(XMFLOAT3(45.0f, 10.0f, 70.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f);
		break;
	}

	switch (m_DefenderCharacter->GetSoldiersType())
	{
	default:
		break;
	case SoldiersType::artillery:
	case SoldiersType::armored:
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Explosive")->PlayEffect(XMFLOAT3(25.0f, 7.5f, 40.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);
		break;
	case SoldiersType::infantry:
	case SoldiersType::machinegunner:
	case SoldiersType::scout:
		if (m_AttackerCharacter->GetSoldiersType() == SoldiersType::artillery)
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Explosive")->PlayEffect(XMFLOAT3(25.0f, 7.5f, 40.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);
		}
		break;
	}

}
