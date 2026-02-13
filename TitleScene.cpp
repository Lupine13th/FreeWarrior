#include "TitleScene.h"
#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "KeyBindComponent.h"	

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

int TitleScene::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
    int count = startIndex;

    while (*str != '\0')
    {
        if (std::find(m_WordListName, m_ChEnd, *str) != m_ChEnd)
        {
            m_WordSprites[count]->SetSpritePattern(0, width, height, m_FontMap[*str]);
            m_WordSprites[count]->setSpriteIndex(0);
            m_WordSprites[count]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
            m_WordSprites[count]->setPosition(ltX, ltY, 0.0f);
            count++;
        }

        ltX += width + 8.0f;

        str++;
    }

    return count;
}		//正常

void TitleScene::initAction()
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


    XMFLOAT4 pattern(0, 0, 1, 1);

    

    m_Sprite.push_back(std::make_unique<SpriteCharacter>());
    m_Sprite[0]->setTextureId(L"TitleBackGroundTexturewithLogo");
    m_Sprite[0]->SetCameraLabel(L"HUDCamera", 0);
    m_Sprite[0]->SetGraphicsPipeLine(L"AlphaSprite");
    m_Sprite[0]->SetSpritePattern(0, 1, 1, pattern);
    m_Sprite[0]->setScale(1000.0f, 1000.0f, 0.1f);
    m_Sprite[0]->setSpriteIndex(0);
    m_Sprite[0]->setPosition(0.0f, -10.0f, 2.0f);
    m_Sprite[0]->setColor(1.0f, 1.0f, 1.0f, 1);

    m_Sprite.push_back(std::make_unique<SpriteCharacter>());
    m_Sprite[1]->setTextureId(L"TitleMenuEffectTexture");
    m_Sprite[1]->SetCameraLabel(L"HUDCamera", 0);
    m_Sprite[1]->SetGraphicsPipeLine(L"AlphaSprite");
    m_Sprite[1]->SetSpritePattern(0, 1, 1, m_PatternRect);
    m_Sprite[1]->setScale(550.0f, 550.0f, 0.1f);
    m_Sprite[1]->setSpriteIndex(0);
    m_Sprite[1]->setPosition(0.0f, -10.0f, 1.0f);
    m_Sprite[1]->setColor(1.0f, 1.0f, 1.0f, 1);

    m_Sprite.push_back(std::make_unique<SpriteCharacter>());
    m_Sprite[2]->setTextureId(L"TitleFlashEffect");
    m_Sprite[2]->SetCameraLabel(L"HUDCamera", 0);
    m_Sprite[2]->SetGraphicsPipeLine(L"AlphaSprite");
    m_Sprite[2]->SetSpritePattern(0, 1, 1, m_PatternRect);
    m_Sprite[2]->setScale(1000.0f, 1000.0f, 0.1f);
    m_Sprite[2]->setSpriteIndex(0);
    m_Sprite[2]->setPosition(0.0f, -10.0f, 0.0f);
    m_Sprite[2]->setColor(1.0f, 1.0f, 1.0f, 1);

    //Text
    m_WordCount = 50;

    SpriteCharacter* spc;

    for (int i = 0; i < m_WordCount; i++)
    {
        spc = new SpriteCharacter();

        spc->setTextureId((m_TextureType).c_str());
        spc->SetCameraLabel(L"HUDCamera", 0);

        spc->setColor(1, 1, 1, 1);

        spc->SetGraphicsPipeLine(L"AlphaSprite");

        m_WordSprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
    }

    //FontMap
    Texture2DContainer* tex = engine->GetTextureManager()->GetTexture(m_TextureType);

    m_FontMap.reserve(wcslen(m_WordListName));

    m_ChEnd = m_WordListName + wcslen(m_WordListName);

    int index = 0;
    float invW = 1.0f / tex->fWidth;
    float invH = 1.0f / tex->fHeight;

    float x = 0.0f;
    float y = 0.0f;
    float w = 64.0f;
    float h = 114.0f;
    while (m_WordListName[index] != '\0')
    {
        XMFLOAT4 r = { x * invW, y * invH, w * invW, h * invH };
        m_FontMap[m_WordListName[index]] = r;
        x += 64.0f;

        if (x >= tex->fWidth)
        {
            x = 0.0f;
            y += h;
        }

        index++;
    }

    m_TimeManager = MyAccessHub::GetTimeManager();

    engine->GetSoundManager()->playBGM(8);
}

bool TitleScene::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    SceneManager* scene = static_cast<SceneManager*>(engine->GetSceneController());
    KeyBindComponent* keyBind = static_cast<KeyBindComponent*>(scene->getKeyComponent());

    int count = 0;

    wstring startGame;
    wstring exitGame;

    switch (m_TitleState)
    {
    case TitleState::Menu:
    {
        if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
        {
            switch (m_MenuIndex)
            {
            case (int)TitleMenuState::GameStart:
				m_TitleState = TitleState::Flash;
                break;
            case (int)TitleMenuState::ExitGame:
                PostQuitMessage(0);
                break;
            }
        }
        else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))
        {
            switch (m_MenuIndex)
            {
            case (int)TitleMenuState::GameStart:
                m_MenuIndex = (int)TitleMenuState::ExitGame;
                break;
            default:
                break;
            }
        }
        else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD))
        {
            switch (m_MenuIndex)
            {
            case (int)TitleMenuState::ExitGame:
                m_MenuIndex = (int)TitleMenuState::GameStart;
                break;
            default:
                break;
            }
        }

        startGame = L"ゲームスタート";
        exitGame = L"ゲーム終了";

        count = MakeSpriteString(count, -400.0f, -100.0f, 50, 80, startGame.c_str());

        if (m_MenuIndex == (int)TitleMenuState::GameStart)
        {
			m_Sprite[1]->setPosition(-270.0f, -50.0f, 1.0f);
        }

        count = MakeSpriteString(count, -400.0f, -200.0f, 50, 80, exitGame.c_str());

        if (m_MenuIndex == (int)TitleMenuState::ExitGame)
        {
            m_Sprite[1]->setPosition(-270.0f, -150.0f, 1.0f);
        }

        for (int i = 0; i < count; i++)
        {
            pipe->AddRenderObject(m_WordSprites[i].get());
        }

		m_MenuAnimationCount += m_TimeManager->GetDeltaTime();

        if (m_MenuAnimationCount > 0.1f)
        {
			m_MenuAnimationCount = 0.0f;
            m_AnimationPages++;
            if (m_AnimationPages >= kMaxPageCount)
            {
                m_AnimationPages = 0;
			}
        }

        m_PatternRect = {
            kPageAreaList[m_AnimationPages].x * kFlipWidth / kFullFlipWidth,
            kPageAreaList[m_AnimationPages].y * kFlipHeight / kFullFlipHeight,
            kFlipWidth / kFullFlipWidth,
            kFlipHeight / kFullFlipHeight
        };

        m_Sprite[1]->SetSpritePattern(0, 1, 1, m_PatternRect);

        pipe->AddRenderObject(m_Sprite[0].get());
        pipe->AddRenderObject(m_Sprite[1].get());

        break;
    }
    case TitleState::Flash:
    {
		m_FlashAnimationCount += m_TimeManager->GetDeltaTime();
        if (m_FlashAnimationCount > 0.05f)
        {
            m_FlashAnimationCount = 0.0f;
            m_FlashAnimationPages++;
            if (m_FlashAnimationPages >= kMaxFlashPageCount)
            {
                m_TitleState = TitleState::NextScene;
            }

            m_PatternRect = {
            kPageAreaList[m_FlashAnimationPages].x * kFlipWidth / kFullFlipWidth,
            kPageAreaList[m_FlashAnimationPages].y * kFlipHeight / kFullFlipHeight,
            kFlipWidth / kFullFlipWidth,
            kFlipHeight / kFullFlipHeight
            };

            m_Sprite[2]->SetSpritePattern(0, 1, 1, m_PatternRect);
        }

        pipe->AddRenderObject(m_Sprite[0].get());
        pipe->AddRenderObject(m_Sprite[1].get());
        pipe->AddRenderObject(m_Sprite[2].get());
        break;
    }
    case TitleState::NextScene:
        MyAccessHub::getMyGameEngine()->GetSceneController()->OrderNextScene((UINT)m_NextScene);
        break;
    }
	return true;
}

void TitleScene::finishAction()
{
}

void TitleScene::SetNextScene(GAME_SCENES nextSc)
{
	m_NextScene = nextSc;
}
