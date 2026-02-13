#include "Squares.h"
#include "SpriteCharacter.h"
#include "SceneManager.h"
#include "BattleFieldManager.h"
#include "EffectGenerator.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <chrono>
#include <cmath>

using namespace std::chrono;

void Squares::initAction()
{
	CharacterData* SqData = getGameObject()->getCharacterData();
	SqData->setScale(0.5f, 0.5f, 0.5f);
	SqData->setRotation(90.0f, 0.0f, 0.0f);
	SqData->SetGraphicsPipeLine(L"AlphaSprite");

	m_TimeManager = MyAccessHub::GetTimeManager();
	BFMng = MyAccessHub::GetBFManager();
}

bool Squares::frameAction()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	if (!firstcount)
	{
		m_SqData = getGameObject()->getCharacterData();
	}

	if (m_IsAnimating)
	{
		Fcam = MyAccessHub::GetFlyingCameraController();

		if (!m_IsFirstAnimation)
		{
			NowChara->fbxD->setRotation(0.0f, m_Rotate, 0.0f);
			m_IsFirstAnimation = true;
		}

		if (NowAnimation == Animations::Move)
		{
			if (m_CharacterAnimationCount < 2.0f)
			{
				m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				float t = m_CharacterAnimationCount / 2.0f;
				NowChara->fbxD->setPosition(NowCharaPos.x + m_DistanceX * t, NowCharaPos.y, NowCharaPos.z + m_DistanceY * t);
			}

			else if (m_CharacterAnimationCount > 2.0f)
			{
				NowChara->fbxD->SetAnime(L"WAIT01");

				Fcam->ChangeCameraPosition();

				m_CharacterAnimationCount = 0.0f;

				NowChara->fbxD->setPosition(NextChara->SqPos.x, NextChara->SqPos.y, NextChara->SqPos.z);
				NextChara->fbxD = NowChara->fbxD;
				NowChara->fbxD = nullptr;
				NowChara->chara = nullptr;

				BFMng->CheckMoved();

				m_IsFirstAnimation = false;
				m_IsAnimating = false;
			}
		}
		else if (NowAnimation == Animations::Attack)
		{
			if (p_scene->getCameraComponent(L"AttackerCamera") == nullptr)
			{
				BFMng->GetCameraChangerCompornent()->SetBattleCamera();
				MyAccessHub::GetHUDManager()->GetHUDObject("DamageEffectHUD")->SetAnimationState(AnimationState::Init);
				SetBattlePosition();
				AttackReaction();
			}
			else
			{
				if (m_CharacterAnimationCount < 1.0f)
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else if (m_CharacterAnimationCount < 3.0f && !m_IsDamaged)
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
					HitReaction();
				}
				else if (m_CharacterAnimationCount < 3.0f && m_IsDamaged)
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else
				{
					SetPreviousPosition();
					BFMng->GetCameraChangerCompornent()->SetMainCamera();
					BFMng->CheckMoved();
					m_CharacterAnimationCount = 0.0f;
					m_IsFirstAnimation = false;
					m_IsAnimating = false;
					m_IsDamaged = false;
					NowChara->fbxD->SetAnime(L"WAIT01");
					NextChara->fbxD->SetAnime(L"WAIT01");
					Fcam->ChangeCameraPosition();
				}
			}
		}
		else if (NowAnimation == Animations::Scout)
		{
			if (p_scene->getCameraComponent(L"AttackerCamera") == nullptr)
			{
				BFMng->GetCameraChangerCompornent()->SetBattleCamera();
				SetBattlePosition();
				AttackReaction();
				MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scout")->PlayEffect(XMFLOAT3(25.0f, 9.7f, kOriginScoutEffectPosZ), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.5f);
				m_ScoutEffectPosZ = kOriginScoutEffectPosZ;
			}
			else
			{
				m_ScoutEffectPosZ += kScoutEffectPosZInterval;
				MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scout")->SetEffectPosition(25.0f, 9.7f, m_ScoutEffectPosZ);
				if (m_CharacterAnimationCount < 1.0f)
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else if (m_CharacterAnimationCount < 3.0f && !m_IsDamaged)
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
					if (BFMng->GetIsSucceedScout())
					{
						MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scouted")->PlayEffect(XMFLOAT3(25.0f, 12.0f, 40.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.3f);
						HitReaction();
					}
					m_IsDamaged = true;
				}
				else if (m_CharacterAnimationCount < 3.0f && m_IsDamaged)
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else
				{
					SetPreviousPosition();
					BFMng->GetCameraChangerCompornent()->SetMainCamera();
					BFMng->CheckMoved();
					m_CharacterAnimationCount = 0.0f;
					m_IsFirstAnimation = false;
					m_IsAnimating = false;
					m_IsDamaged = false;
					NowChara->fbxD->SetAnime(L"WAIT01");
					NextChara->fbxD->SetAnime(L"WAIT01");
					Fcam->ChangeCameraPosition();
				}
			}
		}
	}

	m_SqData->GetPipeline()->AddRenderObject(m_SqData);
	return true;
}

void Squares::finishAction()
{

}

void Squares::HitReaction()
{
	NextChara->fbxD->SetAnime(L"DAMAGE01");

	switch (chara->CharaKind)
	{
	default:
		break;
	case SoldiersType::artillery:
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"LargeExplosive01")->PlayEffect(XMFLOAT3(35.0f, 10.0f, 55.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f);
		MyAccessHub::GetEffectGenerator()->GetEffectObject(L"LargeExplosive02")->PlayEffect(XMFLOAT3(45.0f, 10.0f, 70.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f);
		break;
	}

	switch (NextChara->chara->CharaKind)
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
		if (chara->CharaKind == SoldiersType::artillery)
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Explosive")->PlayEffect(XMFLOAT3(25.0f, 7.5f, 40.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);
		}
		break;
	}

	m_IsDamaged = true;
}

void Squares::AttackReaction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	switch (chara->CharaKind)
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
		if (chara->CharaAdmin == Admin::Rebel)
		{
			MyAccessHub::GetEffectGenerator()->GetEffectObject(L"MuzzleFlash")->PlayEffect(XMFLOAT3(25.0f, 7.5f, -44.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.1f);
		}
		else if (chara->CharaAdmin == Admin::Imperial)
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

void Squares::SetSquaresColor(SquareColor color)
{
	SpriteCharacter* SqData = static_cast<SpriteCharacter*>(getGameObject()->getCharacterData());
	switch (color)
	{
	case SquareColor::AttackCursor:
		SqData->setColor(1.0f, 0.0f, 1.0f, 1.0f);
		break;
	case SquareColor::AttackSellectCursor:
		SqData->setColor(1.0f, 0.0f, 0.5f, 1.0f);
		break;
	case SquareColor::FieldCursor:
		SqData->setColor(0.0f, 1.0f, 0.0f, 1.0f);
		break;
	case SquareColor::AbillityCursor:
		SqData->setColor(1.0f, 1.0f, 0.0f, 1.0f);
		break;
	case SquareColor::MoveCursor:
		SqData->setColor(0.0f, 1.0f, 1.0f, 1.0f);
		break;
	case SquareColor::EnemyRengeCursor:
		SqData->setTextureId(L"TargetTundraTexture");
		break;
	case SquareColor::EnemyCursor:
		SqData->setColor(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case SquareColor::NotCursor:
		SqData->setColor(1.0f, 1.0f, 1.0f, 1.0f);
		SqData->setTextureId(L"TundraTexture");
		break;
	case SquareColor::PassColor:
		SqData->setColor(0.5f, 0.7f, 0.5f, 1.0f);
		break;
	case SquareColor::TargetColor:
		SqData->setColor(1.0f, 0.3f, 0.3f, 1.0f);
		break;
	default:
		SqData->setColor(1.0f, 1.0f, 1.0f, 1.0f);
		SqData->setTextureId(L"TundraTexture");
		break;
	}
}

void Squares::SetAnimation(Animations anim, Admin admin, Squares* chara, Squares* next)
{
	NowAnimation = anim;
	NowAdmin = admin;
	NowChara = chara;
	NextChara = next;
	NowCharaPos = NowChara->SqPos;
	NextCharaPos = NextChara->SqPos;
	NowCharaRot = NowChara->fbxD->getRotation();
	if (anim == Animations::Attack || anim == Animations::Scout)
	{
		BFMng->SetAttackingCharacterSquares(NowChara);
		NextCharaRot = NextChara->fbxD->getRotation();
	}
	m_DistanceX = NextCharaPos.x - NowCharaPos.x;
	m_DistanceY = NextCharaPos.z - NowCharaPos.z;

	float angleRad = std::atan2(m_DistanceY, m_DistanceX);

	m_Rotate = 90.0f - angleRad * (180.0f / 3.14159265f);
	m_CharacterAnimationCount = 0.0f;

	m_IsAnimating = true;
}

void Squares::SetBattlePosition()
{
	NowChara->fbxD->setPosition(25.0f, 5.0f, -40.0f);
	NowChara->fbxD->setRotation(0.0f, 180.0f, 0.0f);
	NextChara->fbxD->setPosition(25.0f, 5.0f, 40.0f);
	NextChara->fbxD->setRotation(0.0f, 0.0f, 0.0f);
}

void Squares::SetPreviousPosition()
{
	NowChara->fbxD->setPosition(NowCharaPos.x, NowCharaPos.y, NowCharaPos.z);
	NowChara->fbxD->setRotation(NowCharaRot.x, NowCharaRot.y, NowCharaRot.z);
	NextChara->fbxD->setPosition(NextCharaPos.x, NextCharaPos.y, NextCharaPos.z);
	NextChara->fbxD->setRotation(NextCharaRot.x, NextCharaRot.y, NextCharaRot.z);
}

