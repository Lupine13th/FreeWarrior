#include "ReadyCharacterHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

using namespace std::chrono;

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"


void ReadyCharacterHUD::InitAction()
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

    m_Sprite.resize(6);

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

    //味方部隊その1
    m_Sprite[0]->SetScale(40.0f, 4.0f, 0.1f);
    m_Sprite[0]->setPosition(-250.0f, 120.0f, 2.0f);

    //味方部隊その2
    m_Sprite[1]->SetScale(40.0f, 4.0f, 0.1f);
    m_Sprite[1]->setPosition(-250.0f, 60.0f, 2.0f);

    //味方部隊その3
    m_Sprite[2]->SetScale(40.0f, 4.0f, 0.1f);
    m_Sprite[2]->setPosition(-250.0f, 0.0f, 2.0f);

    //味方部隊その4
    m_Sprite[3]->SetScale(40.0f, 4.0f, 0.1f);
    m_Sprite[3]->setPosition(-250.0f, -60.0f, 2.0f);

    //味方部隊その5
    m_Sprite[4]->SetScale(40.0f, 4.0f, 0.1f);
    m_Sprite[4]->setPosition(-250.0f, -120.0f, 2.0f);

    //ready
    m_Sprite[5]->SetScale(40.0f, 4.0f, 0.1f);
    m_Sprite[5]->setPosition(-250.0f, -190.0f, 2.0f);
}

bool ReadyCharacterHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    for (int i = 0; i < 5; i++)
    {
        if (i == BRScene->menuIndex && !BRScene->ready)
        {
            m_Sprite[i]->SetColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
        }
        else
        {
			m_Sprite[i]->SetColor(0.0f, 0.0f, 0.0f, 1);
        }
	}

    if (BRScene->ready)
    {
        m_Sprite[5]->SetColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
    }
    else
    {
        m_Sprite[5]->SetColor(0.0f, 0.0f, 0.0f, 1);
    }


    if (BRScene->m_ReadySceneState == ReadySceneState::Menu && BRScene->lighting)
    {
        for (int i = 0; i < m_Sprite.size(); i++)
        {
            pipe->AddRenderObject(m_Sprite[i].get());
        }
    }
    
    

    return true;
}

void ReadyCharacterHUD::FinishAction()
{
}
