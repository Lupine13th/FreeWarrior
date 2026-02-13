#include "ReadyCharacterHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

using namespace std::chrono;

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"


void ReadyCharacterHUD::initAction()
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

    m_sprite.resize(6);

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

    //味方部隊その1
    m_sprite[0]->setScale(40.0f, 4.0f, 0.1f);
    m_sprite[0]->setPosition(-250.0f, 120.0f, 2.0f);

    //味方部隊その2
    m_sprite[1]->setScale(40.0f, 4.0f, 0.1f);
    m_sprite[1]->setPosition(-250.0f, 60.0f, 2.0f);

    //味方部隊その3
    m_sprite[2]->setScale(40.0f, 4.0f, 0.1f);
    m_sprite[2]->setPosition(-250.0f, 0.0f, 2.0f);

    //味方部隊その4
    m_sprite[3]->setScale(40.0f, 4.0f, 0.1f);
    m_sprite[3]->setPosition(-250.0f, -60.0f, 2.0f);

    //味方部隊その5
    m_sprite[4]->setScale(40.0f, 4.0f, 0.1f);
    m_sprite[4]->setPosition(-250.0f, -120.0f, 2.0f);

    //ready
    m_sprite[5]->setScale(40.0f, 4.0f, 0.1f);
    m_sprite[5]->setPosition(-250.0f, -190.0f, 2.0f);
}

bool ReadyCharacterHUD::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    for (int i = 0; i < 5; i++)
    {
        if (i == BRScene->menuIndex && !BRScene->ready)
        {
            m_sprite[i]->setColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
        }
        else
        {
			m_sprite[i]->setColor(0.0f, 0.0f, 0.0f, 1);
        }
	}

    if (BRScene->ready)
    {
        m_sprite[5]->setColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
    }
    else
    {
        m_sprite[5]->setColor(0.0f, 0.0f, 0.0f, 1);
    }


    if (BRScene->m_ReadySceneState == ReadySceneState::Menu && BRScene->lighting)
    {
        for (int i = 0; i < m_sprite.size(); i++)
        {
            pipe->AddRenderObject(m_sprite[i].get());
        }
    }
    
    

    return true;
}

void ReadyCharacterHUD::finishAction()
{
}
