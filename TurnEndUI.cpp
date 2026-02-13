#include "TurnEndUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

#include "SceneManager.h"
#include "SpriteCharacter.h"

void TurnEndUI::initAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();

    engine->InitCameraConstantBuffer(chData);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));

    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f / 512.0f, 1.0f / 512.0f);

    m_sprite.resize(3);

    for (int i = 0; i < m_sprite.size(); i++)
    {
        m_sprite[i] = std::make_unique<SpriteCharacter>();
        m_sprite[i]->setTextureId(L"Sprite00");
        m_sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_sprite[i]->SetSpritePattern(0, 1, 1, pattern);
        m_sprite[i]->setSpriteIndex(0);
    }

    m_sprite[0]->setPosition(0.0f, 0.0f, 1.5f);
    m_sprite[0]->setScale(500.0f, 300.0f, 0.1f);
    m_sprite[0]->setColor(0.0f, 0.0f, 1.0f, 1);

    m_sprite[1]->setPosition(100.0f, -50.0f, 1.4f);
    m_sprite[1]->setScale(150.0f, 90.0f, 0.1f);
    m_sprite[1]->setColor(1.0f, 1.0f, 1.0f, 1);

    m_sprite[2]->setPosition(-100.0f, -50.0f, 1.4f);
    m_sprite[2]->setScale(150.0f, 90.0f, 0.1f);
    m_sprite[2]->setColor(1.0f, 1.0f, 1.0f, 1);
}

bool TurnEndUI::frameAction()
{
    if (BFMng->GetMode() == Mode::TurnEndMode && BFMng->GetCurrentTurn() != Turn::Result && m_WaitCount < 3.0f)
    {
        m_WaitCount += m_TimeManager->GetDeltaTime();
    }
    else if (BFMng->GetMode() == Mode::TurnEndMode && m_WaitCount > 3.0f)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        if (!BFMng->GetTurnEnd())
        {
            m_sprite[1]->setColor(1.0f, 1.0f, 1.0f, 1);
            m_sprite[2]->setColor(1.0f, 0.0f, 0.0f, 1);
        }
        else if (BFMng->GetTurnEnd())
        {
            m_sprite[1]->setColor(1.0f, 0.0f, 0.0f, 1);
            m_sprite[2]->setColor(1.0f, 1.0f, 1.0f, 1);
        }

        for (int i = 0; i < m_sprite.size(); i++)
        {
            pipe->AddRenderObject(m_sprite[i].get());
        }
    }
    
    return true;
}

void TurnEndUI::finishAction()
{
}

void TurnEndUI::ResetWaitCount()
{
    m_WaitCount = 0.0f;
}

float TurnEndUI::GetCurrentCount()
{
    return m_WaitCount;
}
