#include "TurnUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"

void TurnUI::InitAction()
{
    m_Sprite = std::make_unique<SpriteCharacter>();
    
	m_Sprite->SetCameraLabel(L"HUDCamera", 0);

    m_Sprite->SetColor(1.0f, 1.0f, 1.0f, 1);
    m_Sprite->SetGraphicsPipeLine(L"AlphaSprite");
    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);
    m_Sprite->SetSpritePattern(0, 1, 1, pattern);
    m_Sprite->setScale(250.0f, 250.0f, 0.1f);
    m_Sprite->setSpriteIndex(0);
    m_Sprite->setPosition(-350.0f, 250.0f, 2.0f);
    m_Sprite->SetTextureId(L"TurnHUDAllies00");
}

bool TurnUI::FrameAction()
{
    if (TurnUIenable && BFMng->GetCurrentTurn() != Turn::First)
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        switch (m_HUDState)                             //ターン切り替えアニメーション
        {
        case HUDState::Allies:                          //味方ターン
            m_Sprite->SetTextureId(L"TurnHUDAllies00");
            break;
        case HUDState::AlliesToEnemy:                   //味方ターンから敵ターンへ
            if (m_AnimCount < kAnimInterval)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies00");
				m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval < m_AnimCount && m_AnimCount < kAnimInterval * 2)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies01");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 2 < m_AnimCount && m_AnimCount < kAnimInterval * 3)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies02");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 3 < m_AnimCount && m_AnimCount < kAnimInterval * 4)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies03");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 4 < m_AnimCount && m_AnimCount < kAnimInterval * 5)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies04");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 5 < m_AnimCount && m_AnimCount < kAnimInterval * 6)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies05");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 6 < m_AnimCount && m_AnimCount < kAnimInterval * 7)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies06");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 7 < m_AnimCount && m_AnimCount < kAnimInterval * 8)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies07");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 8 < m_AnimCount)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy00");
                m_AnimCount = 0.0f;
				m_HUDState = HUDState::Enemy;
                BFMng->ChangeTurnEnemy();
            }
            break;
        case HUDState::Enemy:                               //敵ターン
            m_Sprite->SetTextureId(L"TurnHUDEnemy00");
            break;
        case HUDState::EnemyToAllies:                       //敵ターンから味方ターンへ
            if (m_AnimCount < kAnimInterval)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy00");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval < m_AnimCount && m_AnimCount < kAnimInterval * 2)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy01");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 2 < m_AnimCount && m_AnimCount < kAnimInterval * 3)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy02");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 3 < m_AnimCount && m_AnimCount < kAnimInterval * 4)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy03");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 4 < m_AnimCount && m_AnimCount < kAnimInterval * 5)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy04");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 5 < m_AnimCount && m_AnimCount < kAnimInterval * 6)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy05");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 6 < m_AnimCount && m_AnimCount < kAnimInterval * 7)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy06");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 7 < m_AnimCount && m_AnimCount < kAnimInterval * 8)
            {
                m_Sprite->SetTextureId(L"TurnHUDEnemy07");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 8 < m_AnimCount)
            {
                m_Sprite->SetTextureId(L"TurnHUDAllies00");
                m_AnimCount = 0.0f;
                m_HUDState = HUDState::Allies;
                BFMng->ChangeTurnAllies();
            }
            break;
        }

        //pipe->AddRenderObject(m_Sprite.get());
    }

    return true;
}

void TurnUI::FinishAction()
{
    SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
    scene->RemoveCamera(this);
}

void TurnUI::ChangeHUDState()
{
    if (BFMng->GetPreviousTurn() == Turn::Allies)
    {
        m_HUDState = HUDState::AlliesToEnemy;
    }
    else if (BFMng->GetPreviousTurn() == Turn::EnemyMove)
    {
        m_HUDState = HUDState::EnemyToAllies;
    }
}
