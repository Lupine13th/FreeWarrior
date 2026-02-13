#include "OpeningAnimHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "KeyBindComponent.h"

using namespace std::chrono;

void OpeningAnimHUD::initAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();
    m_TimeManager = MyAccessHub::GetTimeManager();

    engine->InitCameraConstantBuffer(chData);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));

    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));

    o_sprite.resize(5);

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    for (int i = 0; i < o_sprite.size(); i++)
    {
        o_sprite[i] = std::make_unique<SpriteCharacter>();
        o_sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        o_sprite[i]->setColor(1, 1, 1, 1);
        o_sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        o_sprite[i]->SetSpritePattern(0, 1, 1, pattern);
        o_sprite[i]->setSpriteIndex(0);
    }

    o_sprite[0]->setTextureId(L"RadioTexture");
    o_sprite[0]->setScale(RadioScale.x, RadioScale.y, 0.1f);
    o_sprite[0]->setPosition(RadioPos.x, RadioPos.y, 1.0f);

    o_sprite[1]->setTextureId(L"PaporTexture");
    o_sprite[1]->setScale(RadioScale.x, RadioScale.y, 0.1f);
    o_sprite[1]->setPosition(RadioPos.x, RadioPos.y, 2.5f);

    o_sprite[2]->setTextureId(L"TargetWinTexture");
    o_sprite[2]->setScale(TarWinScale.x, TarWinScale.y, 0.1f);
    o_sprite[2]->setPosition(TarWinPos.x, TarWinPos.y, 2.0f);

    o_sprite[3]->setTextureId(L"TargetDefTexture");
    o_sprite[3]->setScale(TarDefScale.x, TarDefScale.y, 0.1f);
    o_sprite[3]->setPosition(TarDefPos.x, TarDefPos.y, 2.0f);

    o_sprite[4]->setTextureId(L"FadeInTexture");
    o_sprite[4]->setScale(FadeScale.x, FadeScale.y, 0.1f);
    o_sprite[4]->setPosition(FadePos.x, FadePos.y, 2.0f);
}

bool OpeningAnimHUD::frameAction()
{
    if (BFMng->GetCurrentTurn() == Turn::First)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();

        SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());

        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        KeyBindComponent* keycomp = static_cast<KeyBindComponent*>(p_scene->getKeyComponent());

        switch (oState)
        {
        default:
            break;
        case OpeningState::FadeAnimation:
            if (OPAnimCount < 1.0f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();
                FadePos.x -= 1200.0f * m_TimeManager->GetDeltaTime();
                o_sprite[4]->setPosition(FadePos.x, FadePos.y, 2.0f);
                pipe->AddRenderObject(o_sprite[4].get());
            }
            else if (OPAnimCount > 1.0f)
            {
				OPAnimCount = 0.0f;
                oState = OpeningState::None;
			}
            break;
        case OpeningState::BeforeAnim:

            if (OPAnimCount < 0.5f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();
                RadioPos.x -= 30.0f;
                o_sprite[0]->setPosition(RadioPos.x, RadioPos.y, 1.0f);
                o_sprite[1]->setPosition(RadioPos.x, RadioPos.y, 2.5f);
            }
            else if (OPAnimCount > 0.5f && OPAnimCount < 0.55f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();
                RadioPos.x = 30.0f;
                o_sprite[0]->setPosition(RadioPos.x, RadioPos.y, 1.0f);
                o_sprite[1]->setPosition(RadioPos.x, RadioPos.y, 2.5);
            }
            else if (OPAnimCount > 0.55f && OPAnimCount < 1.0f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();
                RadioPos.y -= 10.0f;

                RadioScale.x += 16.0f;
                RadioScale.y += 16.0f;

                o_sprite[0]->setPosition(RadioPos.x, RadioPos.y, 1.0f);
                o_sprite[0]->setScale(RadioScale.x, RadioScale.y, 0.1f);

                o_sprite[1]->setPosition(RadioPos.x, RadioPos.y, 2.5f);
                o_sprite[1]->setScale(RadioScale.x, RadioScale.y, 0.1f);
            }
            else if (OPAnimCount > 1.0f)
            {
                oState = OpeningState::WinTarget;
				engine->GetSoundManager()->play(13);
            }

            pipe->AddRenderObject(o_sprite[0].get());
            pipe->AddRenderObject(o_sprite[1].get());

            break;

        case OpeningState::WinTarget:

            if (OPAnimCount > 1.0f && OPAnimCount < 2.0f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();
                TarWinPos.y += 7.0f;
                o_sprite[2]->setPosition(TarWinPos.x, TarWinPos.y, 2.0f);
            }
            else if (OPAnimCount > 2.0f)
            {
                if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
                {
                    oState = OpeningState::DefTarget;
                }
            }

            pipe->AddRenderObject(o_sprite[0].get());
            pipe->AddRenderObject(o_sprite[1].get());
            pipe->AddRenderObject(o_sprite[2].get());

            break;

        case OpeningState::DefTarget:

            if (OPAnimCount > 2.0f && OPAnimCount < 2.5f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();

                TarWinPos.y += 6.0f;

                TarWinScale.x -= 6.0f;
                TarWinScale.y -= 6.0f;

                o_sprite[2]->setPosition(TarWinPos.x, TarWinPos.y, 2.0f);
                o_sprite[2]->setScale(TarWinScale.x, TarWinScale.y, 0.1f);

                pipe->AddRenderObject(o_sprite[2].get());
            }
            else if (OPAnimCount > 2.5f && OPAnimCount < 3.0f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();

                TarWinPos.x += 30.0f;

                o_sprite[2]->setPosition(TarWinPos.x, TarWinPos.y, 2.0f);

                pipe->AddRenderObject(o_sprite[2].get());
            }
            else if (OPAnimCount > 3.0f && OPAnimCount < 4.0f)
            {
                if (!m_InitDefeat)
                {
                    engine->GetSoundManager()->play(13);
                    m_InitDefeat = true;
                }

                OPAnimCount += m_TimeManager->GetDeltaTime();

                TarDefPos.y += 7.0f;

                o_sprite[3]->setPosition(TarDefPos.x, TarDefPos.y, 2.0f);

                pipe->AddRenderObject(o_sprite[3].get());
            }
            else if (OPAnimCount > 4.0f)
            {
                if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
                {
                    oState = OpeningState::AfterAnim;
                }
                pipe->AddRenderObject(o_sprite[3].get());
            }

            pipe->AddRenderObject(o_sprite[0].get());
            pipe->AddRenderObject(o_sprite[1].get());

            break;
        case OpeningState::AfterAnim:
            if (OPAnimCount > 4.0f && OPAnimCount < 4.5f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();

                TarDefPos.y += 6.0f;

                TarDefScale.x -= 6.0f;
                TarDefScale.y -= 6.0f;

                o_sprite[3]->setPosition(TarDefPos.x, TarDefPos.y, 2.0f);
                o_sprite[3]->setScale(TarDefScale.x, TarDefScale.y, 0.1f);

                pipe->AddRenderObject(o_sprite[3].get());
            }
            else if (OPAnimCount > 4.5f && OPAnimCount < 5.0f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();

                TarDefPos.x += 30.0f;

                o_sprite[3]->setPosition(TarDefPos.x, TarDefPos.y, 2.0f);

                pipe->AddRenderObject(o_sprite[3].get());
            }
            else if (OPAnimCount > 5.0f && OPAnimCount < 5.2f)
            {
                OPAnimCount += m_TimeManager->GetDeltaTime();

                RadioPos.y -= 15.0f;

                o_sprite[0]->setPosition(RadioPos.x, RadioPos.y, 1.0f);
                o_sprite[1]->setPosition(RadioPos.x, RadioPos.y, 1.0f);
            }

            pipe->AddRenderObject(o_sprite[0].get());
            pipe->AddRenderObject(o_sprite[1].get());

            break;
        }
    }

    
    
    return true;
}

void OpeningAnimHUD::finishAction()
{
}
