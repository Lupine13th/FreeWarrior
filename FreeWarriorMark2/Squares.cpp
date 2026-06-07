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
	m_SqData = GetGameObject()->GetCharacterData();
}

bool Squares::FrameAction()
{
	m_SqData->GetPipeline()->AddRenderObject(m_SqData);
	return true;
}

void Squares::FinishAction()
{

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
