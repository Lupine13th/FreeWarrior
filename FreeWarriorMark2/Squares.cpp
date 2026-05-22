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

void Squares::InitAction()
{
	CharacterData* SqData = GetGameObject()->GetCharacterData();
	SqData->SetScale(0.5f, 0.5f, 0.5f);
	SqData->SetRotation(90.0f, 0.0f, 0.0f);
	SqData->SetGraphicsPipeLine(L"AlphaSprite3D");

	m_TimeManager = MyAccessHub::GetTimeManager();
	BFMng = MyAccessHub::GetBFManager();
}

bool Squares::FrameAction()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	if (!firstcount)
	{
		m_SqData = GetGameObject()->GetCharacterData();
	}

	if (m_IsAnimating)
	{
		Fcam = MyAccessHub::GetFlyingCameraController();

		if (!m_IsFirstAnimation)
		{
			NowChara->fbxD->SetRotation(0.0f, m_Rotate, 0.0f);
			m_IsFirstAnimation = true;
		}

		switch (NowAnimation)
		{
		case Animations::Move:
			if (m_CharacterAnimationCount < 2.0f)	//0～2秒は移動アニメーション
			{
				m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				float t = m_CharacterAnimationCount / 2.0f;
				NowChara->fbxD->setPosition(NowCharaPos.x + m_DistanceX * t, NowCharaPos.y, NowCharaPos.z + m_DistanceY * t);
			}
			else if (m_CharacterAnimationCount > 2.0f)	//2秒経過後は移動アニメーションを終了させる
			{
				NowChara->fbxD->SetAnimeInit(L"WAIT", NowChara->chara);	//待機アニメーション

				Fcam->ChangeCameraPosition();	//カメラ位置をカーソルの位置へ(敵AI側が主に使う処理)	

				m_CharacterAnimationCount = 0.0f;

				NowChara->fbxD->setPosition(NextChara->SqPos.x, NextChara->SqPos.y, NextChara->SqPos.z);	//モデルを移動
				NextChara->fbxD = NowChara->fbxD;															//モデルのデータも移動
				NowChara->fbxD = nullptr;																	//前のマスのデータを削除
				NowChara->chara = nullptr;																	//前のマスのデータを削除

				BFMng->CheckMoved();									//「行動済み」フラグの確認

				m_IsFirstAnimation = false;								//↑
				m_IsAnimating = false;									//↓初期化
			}
			break;
		case Animations::Attack:
		case Animations::ConcentratedFire:
		case Animations::BayonetCharge:
			if (p_scene->getCameraComponent(L"AttackerCamera") == nullptr)	//カメラ切り替え
			{
				BFMng->GetCameraChangerCompornent()->SetBattleCamera();		//戦闘カメラ起動
				MyAccessHub::GetHUDManager()->GetHUDObject("DamageEffectHUD")->SetAnimationState(AnimationState::Init);	//HPバー準備段階
				SetBattlePosition();	//モデルを戦闘アニメーションの位置へ
				AttackReaction();		//攻撃側のアニメーションを設定
			}
			else	//カメラ切り替え後
			{
				if (m_CharacterAnimationCount < 1.0f)	//1秒ディレイ
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else if (m_CharacterAnimationCount < 3.0f && !m_IsDamaged)	//1～3秒は防御側のアニメーションを設定
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
					SetPreviousPosition();									//元のマスの位置へ戻す
					BFMng->GetCameraChangerCompornent()->SetMainCamera();	//バトルカメラOff＆メインカメラ起動
					BFMng->CheckMoved();									//「行動済み」フラグの確認
					m_CharacterAnimationCount = 0.0f;						//==========
					m_IsFirstAnimation = false;								//↑
					m_IsAnimating = false;									//↓初期化
					m_IsDamaged = false;									//==========
					if (NowChara->fbxD != nullptr)
					{
						NowChara->fbxD->SetAnimeInit(L"WAIT", NowChara->chara);	//攻撃側のアニメーションを待機モーションへ
					}

					if (NextChara->fbxD != nullptr)
					{
						NextChara->fbxD->SetAnimeInit(L"WAIT", NextChara->chara);	//防御側のアニメーションを待機モーションへ
					}
					Fcam->ChangeCameraPosition();	//カメラ位置をカーソルの位置へ		
				}
			}
			break;
		case Animations::Scout:
			if (p_scene->getCameraComponent(L"AttackerCamera") == nullptr)	//カメラ切り替え
			{
				BFMng->GetCameraChangerCompornent()->SetBattleCamera();		//戦闘カメラ起動
				SetBattlePosition();										//モデルを戦闘アニメーションの位置へ
				AttackReaction();											//攻撃アニメーション
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
				else if (m_CharacterAnimationCount < 3.0f && !m_IsDamaged)	//アニメーション中偵察成功時にダメージリアクションを行うため、m_IsDamagedで条件分岐
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
					if (BFMng->GetIsSucceedScout())
					{
						MyAccessHub::GetEffectGenerator()->GetEffectObject(L"Scouted")->PlayEffect(XMFLOAT3(25.0f, 12.0f, 40.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), 0.3f);
						HitReaction();
					}
					m_IsDamaged = true;
				}
				else if (m_CharacterAnimationCount < 3.0f && m_IsDamaged)	//アニメーションが終了したので時間経過
				{
					m_CharacterAnimationCount += m_TimeManager->GetDeltaTime();
				}
				else	//アニメーション終了
				{
					SetPreviousPosition();
					BFMng->GetCameraChangerCompornent()->SetMainCamera();
					BFMng->CheckMoved();
					m_CharacterAnimationCount = 0.0f;
					m_IsFirstAnimation = false;
					m_IsAnimating = false;
					m_IsDamaged = false;
					if (NowChara->fbxD != nullptr)
					{
						NowChara->fbxD->SetAnimeInit(L"WAIT", NowChara->chara);
					}

					if (NextChara->fbxD != nullptr)
					{
						NextChara->fbxD->SetAnimeInit(L"WAIT", NextChara->chara);
					}
					Fcam->ChangeCameraPosition();
				}																																						//==========ここから上まで攻撃アニメーションと同じ==========							
			}
			break;
		}
	}

	m_SqData->GetPipeline()->AddRenderObject(m_SqData);
	return true;
}

void Squares::FinishAction()
{

}

//攻撃を受けた際のリアクション　データのやり取りを円滑にするため、Squaresで宣言
void Squares::HitReaction()
{
	NextChara->fbxD->SetAnimeInit(L"DAMAGE", NextChara->chara);

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

//攻撃をする際のリアクション　データのやり取りを円滑にするため、Squaresで宣言
void Squares::AttackReaction()
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

//そのマスの色を変更する
void Squares::SetSquaresColor(SquareColor color)
{
	SpriteCharacter* SqData = static_cast<SpriteCharacter*>(GetGameObject()->GetCharacterData());
	switch (color)
	{
	case SquareColor::AttackCursor:
		SqData->SetColor(1.0f, 0.0f, 1.0f, 1.0f);
		break;
	case SquareColor::AttackSellectCursor:
		SqData->SetColor(1.0f, 0.0f, 0.5f, 1.0f);
		break;
	case SquareColor::FieldCursor:
		SqData->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
		break;
	case SquareColor::AbillityCursor:
		SqData->SetColor(1.0f, 1.0f, 0.0f, 1.0f);
		break;
	case SquareColor::MoveCursor:
		SqData->SetColor(0.0f, 1.0f, 1.0f, 1.0f);
		break;
	case SquareColor::EnemyRengeCursor:
		SqData->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		SqData->SetTextureId(L"TargetTundraTexture");
		break;
	case SquareColor::EnemyCursor:
		SqData->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case SquareColor::NotCursor:
		SqData->SetColor(1.0f, 1.0f, 1.0f, 0.5f);
		SqData->SetTextureId(L"TundraTexture");
		break;
	case SquareColor::PassColor:
		SqData->SetColor(0.5f, 0.7f, 0.5f, 1.0f);
		break;
	case SquareColor::TargetColor:
		SqData->SetColor(1.0f, 0.3f, 0.3f, 0.7f);
		break;
	default:
		SqData->SetColor(1.0f, 1.0f, 1.0f, 0.5f);
		SqData->SetTextureId(L"TundraTexture");
		break;
	}
}

//バトルカメラ移行用の情報を入手＆アニメーションをセット
void Squares::SetAnimation(Animations anim, Admin admin, Squares* chara, Squares* next)
{
	NowAnimation = anim;
	NowAdmin = admin;
	NowChara = chara;
	NextChara = next;
	NowCharaPos = NowChara->SqPos;
	NextCharaPos = NextChara->SqPos;
	NowCharaRot = NowChara->fbxD->GetRotation();
	
	switch (anim)	//攻撃系か偵察の場合、被害を受けたユニットの情報と回転を入手　(後で元の位置に戻すため)
	{
	case Animations::Attack:
	case Animations::ConcentratedFire:
	case Animations::BayonetCharge:
	case Animations::Scout:
		BFMng->SetAttackingCharacterSquares(NowChara);
		NextCharaRot = NextChara->fbxD->GetRotation();
		break;
	}

	//移動アニメーション用に平面の距離を入手
	m_DistanceX = NextCharaPos.x - NowCharaPos.x;
	m_DistanceY = NextCharaPos.z - NowCharaPos.z;

	//内積
	float angleRad = std::atan2(m_DistanceY, m_DistanceX);

	//内積と現在の回転から移動先の方向に向ける
	m_Rotate = 90.0f - angleRad * (180.0f / 3.14159265f);
	m_CharacterAnimationCount = 0.0f;

	m_IsAnimating = true;
}

//バトルカメラの位置にモデルを配置
void Squares::SetBattlePosition()
{
	NowChara->fbxD->setPosition(25.0f, 5.0f, -40.0f);
	NowChara->fbxD->SetRotation(0.0f, 180.0f, 0.0f);
	NextChara->fbxD->setPosition(25.0f, 5.0f, 40.0f);
	NextChara->fbxD->SetRotation(0.0f, 0.0f, 0.0f);
}

//モデルを元のマスの位置、角度へ
void Squares::SetPreviousPosition()
{
	if (NowChara->fbxD != nullptr)
	{
		NowChara->fbxD->setPosition(NowCharaPos.x, NowCharaPos.y, NowCharaPos.z);
		NowChara->fbxD->SetRotation(NowCharaRot.x, NowCharaRot.y, NowCharaRot.z);
	}

	if (NextChara->fbxD != nullptr)
	{
		NextChara->fbxD->setPosition(NextCharaPos.x, NextCharaPos.y, NextCharaPos.z);
		NextChara->fbxD->SetRotation(NextCharaRot.x, NextCharaRot.y, NextCharaRot.z);
	}

	MyAccessHub::GetHUDManager()->GetHUDObject("SuperiorityGaugeHUD")->SetAnimationState(AnimationState::Init);
}

//==========Fbxデータは各マスが持っているため、データが混濁しないためにマスで宣言する==========
