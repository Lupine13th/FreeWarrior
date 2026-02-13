#include "TurnUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"

void TurnUI::initAction()
{
    m_Sprite = std::make_unique<SpriteCharacter>();
    
	m_Sprite->SetCameraLabel(L"HUDCamera", 0);

    m_Sprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_Sprite->SetGraphicsPipeLine(L"AlphaSprite");
    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);
    m_Sprite->SetSpritePattern(0, 1, 1, pattern);
    m_Sprite->setScale(250.0f, 250.0f, 0.1f);
    m_Sprite->setSpriteIndex(0);
    m_Sprite->setPosition(-350.0f, 250.0f, 2.0f);
    m_Sprite->setTextureId(L"TurnHUDAllies00");
}

bool TurnUI::frameAction()
{
    if (TurnUIenable && BFMng->GetCurrentTurn() != Turn::First)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        switch (m_HUDState)
        {
        case HUDState::Allies:
            m_Sprite->setTextureId(L"TurnHUDAllies00");
            break;
        case HUDState::AlliesToEnemy:
            if (m_AnimCount < kAnimInterval)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies00");
				m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval < m_AnimCount && m_AnimCount < kAnimInterval * 2)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies01");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 2 < m_AnimCount && m_AnimCount < kAnimInterval * 3)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies02");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 3 < m_AnimCount && m_AnimCount < kAnimInterval * 4)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies03");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 4 < m_AnimCount && m_AnimCount < kAnimInterval * 5)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies04");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 5 < m_AnimCount && m_AnimCount < kAnimInterval * 6)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies05");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 6 < m_AnimCount && m_AnimCount < kAnimInterval * 7)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies06");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 7 < m_AnimCount && m_AnimCount < kAnimInterval * 8)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies07");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 8 < m_AnimCount)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy00");
                m_AnimCount = 0.0f;
				m_HUDState = HUDState::Enemy;
                BFMng->ChangeTurnEnemy();
            }
            break;
        case HUDState::Enemy:
            m_Sprite->setTextureId(L"TurnHUDEnemy00");
            break;
        case HUDState::EnemyToAllies:
            if (m_AnimCount < kAnimInterval)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy00");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval < m_AnimCount && m_AnimCount < kAnimInterval * 2)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy01");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 2 < m_AnimCount && m_AnimCount < kAnimInterval * 3)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy02");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 3 < m_AnimCount && m_AnimCount < kAnimInterval * 4)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy03");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 4 < m_AnimCount && m_AnimCount < kAnimInterval * 5)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy04");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 5 < m_AnimCount && m_AnimCount < kAnimInterval * 6)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy05");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 6 < m_AnimCount && m_AnimCount < kAnimInterval * 7)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy06");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 7 < m_AnimCount && m_AnimCount < kAnimInterval * 8)
            {
                m_Sprite->setTextureId(L"TurnHUDEnemy07");
                m_AnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (kAnimInterval * 8 < m_AnimCount)
            {
                m_Sprite->setTextureId(L"TurnHUDAllies00");
                m_AnimCount = 0.0f;
                m_HUDState = HUDState::Allies;
                BFMng->ChangeTurnAllies();
            }
            break;
        }

        pipe->AddRenderObject(m_Sprite.get());
    }

    return true;
}

void TurnUI::finishAction()
{
    SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
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
