#include "TurnEndUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

#include "SceneManager.h"
#include "SpriteCharacter.h"

void TurnEndUI::InitAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    CharacterData* chData = GetGameObject()->GetCharacterData();

    engine->InitCameraConstantBuffer(chData);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));

    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f / 512.0f, 1.0f / 512.0f);

    m_Sprite.resize(3);

    for (int i = 0; i < m_Sprite.size(); i++)
    {
        m_Sprite[i] = std::make_unique<SpriteCharacter>();
        m_Sprite[i]->SetTextureId(L"Sprite00");
        m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
        m_Sprite[i]->setSpriteIndex(0);
    }

    m_Sprite[0]->setPosition(0.0f, 0.0f, 1.5f);
    m_Sprite[0]->setScale(500.0f, 300.0f, 0.1f);
    m_Sprite[0]->SetColor(0.0f, 0.0f, 1.0f, 1);

    m_Sprite[1]->setPosition(100.0f, -50.0f, 1.4f);
    m_Sprite[1]->setScale(150.0f, 90.0f, 0.1f);
    m_Sprite[1]->SetColor(1.0f, 1.0f, 1.0f, 1);

    m_Sprite[2]->setPosition(-100.0f, -50.0f, 1.4f);
    m_Sprite[2]->setScale(150.0f, 90.0f, 0.1f);
    m_Sprite[2]->SetColor(1.0f, 1.0f, 1.0f, 1);
}

bool TurnEndUI::FrameAction()
{
    if (BFMng->GetMode() == Mode::TurnEndMode && BFMng->GetCurrentTurn() != Turn::Result && m_WaitCount < 3.0f)
    {
        m_WaitCount += m_TimeManager->GetDeltaTime();
    }
    else if (BFMng->GetMode() == Mode::TurnEndMode && m_WaitCount > 3.0f)
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        if (!BFMng->GetTurnEnd())
        {
            m_Sprite[1]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_Sprite[2]->SetColor(1.0f, 0.0f, 0.0f, 1);
        }
        else if (BFMng->GetTurnEnd())
        {
            m_Sprite[1]->SetColor(1.0f, 0.0f, 0.0f, 1);
            m_Sprite[2]->SetColor(1.0f, 1.0f, 1.0f, 1);
        }

        for (int i = 0; i < m_Sprite.size(); i++)
        {
            pipe->AddRenderObject(m_Sprite[i].get());
        }
    }
    
    return true;
}

void TurnEndUI::FinishAction()
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
