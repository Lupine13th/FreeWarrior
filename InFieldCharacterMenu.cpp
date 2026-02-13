#include "InFieldCharacterMenu.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

using namespace std::chrono;

#include "SceneManager.h"
#include "SpriteCharacter.h"

void InFieldCharacterMenu::initAction()
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

	//0:背景 1~10:キャラ 11~18:枠(横)
    m_sprite.resize(11);

    for (int i = 0; i < m_sprite.size(); i++)
    {
        m_sprite[i] = std::make_unique<SpriteCharacter>();
        m_sprite[i]->setTextureId(L"Sprite00");
        m_sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_sprite[i]->SetSpritePattern(0, 10, 10, pattern);
        m_sprite[i]->setSpriteIndex(0);
        m_sprite[i]->setColor(0.0f, 0.0f, 0.0f, 1);
    }

    m_sprite[0]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[0]->setPosition(-180.0f, 180.0f, 1.0f);

    m_sprite[1]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[1]->setPosition(-180.0f, 140.0f, 1.0f);

    m_sprite[2]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[2]->setPosition(-180.0f, 100.0f, 1.0f);

    m_sprite[3]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[3]->setPosition(-180.0f, 60.0f, 1.0f);

    m_sprite[4]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[4]->setPosition(-180.0f, 20.0f, 1.0f);

    m_sprite[5]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[5]->setPosition(-180.0f, -20.0f, 1.0f);

    m_sprite[6]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[6]->setPosition(-180.0f,-60.0f, 1.0f);

    m_sprite[7]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[7]->setPosition(-180.0f, -100.0f, 1.0f);

    m_sprite[8]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[8]->setPosition(-180.0f, -140.0f, 1.0f);

    m_sprite[9]->setScale(50.0f, 3.0f, 0.1f);
    m_sprite[9]->setPosition(-180.0f, -180.0f, 1.0f);
}

bool InFieldCharacterMenu::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");


    for (int i = 0; i < 9; i++)
    {
        if (i == BRScene->m_InFieldCharacterMenuIndex && !BRScene->ready)
        {
            m_sprite[i]->setColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
        }
        else
        {
            m_sprite[i]->setColor(0.0f, 0.0f, 0.0f, 1);
        }
    }

    if (BRScene->m_ReadySceneState == ReadySceneState::InField && BRScene->lighting)
    {
        for (int i = 0; i < m_sprite.size(); i++)
        {
            pipe->AddRenderObject(m_sprite[i].get());
        }
    }
    
    return true;
}

void InFieldCharacterMenu::finishAction()
{
}
