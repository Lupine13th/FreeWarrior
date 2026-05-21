#include "InFieldCharacterMenu.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

using namespace std::chrono;

void InFieldCharacterMenu::InitAction()
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

	//0:背景 1~10:キャラ 11~18:枠(横)
    m_Sprite.resize(11);

    for (int i = 0; i < m_Sprite.size(); i++)
    {
        m_Sprite[i] = std::make_unique<SpriteCharacter>();
        m_Sprite[i]->SetTextureId(L"Sprite00");
        m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_Sprite[i]->SetSpritePattern(0, 10, 10, pattern);
        m_Sprite[i]->setSpriteIndex(0);
        m_Sprite[i]->SetColor(0.0f, 0.0f, 0.0f, 1);
    }

    m_Sprite[0]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[0]->setPosition(-180.0f, 180.0f, 1.0f);

    m_Sprite[1]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[1]->setPosition(-180.0f, 140.0f, 1.0f);

    m_Sprite[2]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[2]->setPosition(-180.0f, 100.0f, 1.0f);

    m_Sprite[3]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[3]->setPosition(-180.0f, 60.0f, 1.0f);

    m_Sprite[4]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[4]->setPosition(-180.0f, 20.0f, 1.0f);

    m_Sprite[5]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[5]->setPosition(-180.0f, -20.0f, 1.0f);

    m_Sprite[6]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[6]->setPosition(-180.0f,-60.0f, 1.0f);

    m_Sprite[7]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[7]->setPosition(-180.0f, -100.0f, 1.0f);

    m_Sprite[8]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[8]->setPosition(-180.0f, -140.0f, 1.0f);

    m_Sprite[9]->SetScale(50.0f, 3.0f, 0.1f);
    m_Sprite[9]->setPosition(-180.0f, -180.0f, 1.0f);
}

bool InFieldCharacterMenu::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");


    for (int i = 0; i < 9; i++)
    {
        if (i == BRScene->m_InFieldCharacterMenuIndex && !BRScene->ready)
        {
            m_Sprite[i]->SetColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
        }
        else
        {
            m_Sprite[i]->SetColor(0.0f, 0.0f, 0.0f, 1);
        }
    }

    if (BRScene->m_ReadySceneState == ReadySceneState::InField && BRScene->lighting)
    {
        for (int i = 0; i < m_Sprite.size(); i++)
        {
            pipe->AddRenderObject(m_Sprite[i].get());
        }
    }
    
    return true;
}

void InFieldCharacterMenu::FinishAction()
{
}
