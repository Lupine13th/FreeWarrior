#include "LogHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"

void LogHUD::initAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();

    SpriteCharacter* spc;

    for (int i = 0; i < 9; i++)
    {
        spc = new SpriteCharacter();

        spc->setTextureId(L"Sprite00");
        spc->SetCameraLabel(L"HUDCamera", 0);

        spc->setColor(1, 1, 1, 1);

        if (i % 2 == 1)
        {
            spc->setColor(0.9f, 0.9f, 0.9f, 1);
        }

        spc->SetGraphicsPipeLine(L"AlphaSprite");

        XMFLOAT4 pattern(0.0f, 0.0f, 1.0f / 512.0f, 1.0f / 512.0f);

        spc->SetSpritePattern(0, 10, 10, pattern);

        spc->setSpriteIndex(0);

        spc->setPosition(-360.0f, (-90.0f - i * 25.0f), 2.0f);

        spc->setScale(25.0f, 3.0f, 0.1f);

        m_sprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
    }
}

bool LogHUD::frameAction()
{
    if (BFMng->GetCurrentTurn() != Turn::First && BFMng->GetOpenLog())
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        for (int i = 0; i < 8; i++)
        {
            pipe->AddRenderObject(m_sprites[i].get());
        }
    }
    
    return true;
}

void LogHUD::finishAction()
{
    SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
    scene->RemoveCamera(this);
}
