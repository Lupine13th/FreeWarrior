#include "LogHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"

void LogHUD::InitAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    CharacterData* chData = GetGameObject()->GetCharacterData();

    SpriteCharacter* spc;

    for (int i = 0; i < 9; i++)
    {
        XMFLOAT4 pattern(0.0f, 0.0f, 1.0f / 512.0f, 1.0f / 512.0f);

        spc = new SpriteCharacter();
        spc->SetTextureId(L"Sprite00");
        spc->SetCameraLabel(L"HUDCamera", 0);
        spc->SetColor(1, 1, 1, 1);
        if (i % 2 == 1)
        {
            spc->SetColor(0.9f, 0.9f, 0.9f, 1);
        }
        spc->SetGraphicsPipeLine(L"AlphaSprite");
        spc->SetSpritePattern(0, 10, 10, pattern);
        spc->setSpriteIndex(0);
        spc->setPosition(-360.0f, (-90.0f - i * 25.0f), 2.0f);
        spc->SetScale(25.0f, 3.0f, 0.1f);

        m_Sprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
    }
}

bool LogHUD::FrameAction()
{
    if (BFMng->GetCurrentTurn() != Turn::First && BFMng->GetOpenLog())
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        for (int i = 0; i < 8; i++)
        {
            pipe->AddRenderObject(m_Sprites[i].get());
        }
    }
    
    return true;
}

void LogHUD::FinishAction()
{
    SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
    scene->RemoveCamera(this);
}
