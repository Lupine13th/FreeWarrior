#include "InFieldCharacterMenuImage.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"

void InFieldCharacterMenuImage::initAction()
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

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    m_Sprite = std::make_unique<SpriteCharacter>();
    m_Sprite->SetCameraLabel(L"HUDCamera", 0);
    m_Sprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_Sprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_Sprite->SetSpritePattern(0, 1, 1, pattern);
    m_Sprite->setScale(270.0f, 270.0f, 0.1f);
    m_Sprite->setSpriteIndex(0);
    m_Sprite->setPosition(270.0f, 50.0f, 1.0f); // 表示位置を調整
}

bool InFieldCharacterMenuImage::frameAction()
{
    if (BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex] != nullptr)
    {
        switch (BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex]->CharaKind)
        {
        default:
            break;
        case SoldiersType::infantry:
            m_Sprite->setTextureId(L"RebelInfTexture");
            break;
        case SoldiersType::machinegunner:
            m_Sprite->setTextureId(L"RebelCavTexture");
            break;
        case SoldiersType::artillery:
            m_Sprite->setTextureId(L"RebelArtTexture");
            break;
        case SoldiersType::scout:
            m_Sprite->setTextureId(L"RebelSctTexture");
            break;
        case SoldiersType::armored:
            m_Sprite->setTextureId(L"RebelArmTexture");
            break;
        }

        if (BRScene->m_ReadySceneState == ReadySceneState::InField)
        {
            MyGameEngine* engine = MyAccessHub::getMyGameEngine();
            GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");
            pipe->AddRenderObject(m_Sprite.get());
        }
    }
   
    return true;
}

void InFieldCharacterMenuImage::finishAction()
{
}
