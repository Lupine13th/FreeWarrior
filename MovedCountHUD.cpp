#include "MovedCountHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"

void MovedCountHUD::initAction()
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

    m_Sprite.resize(BFMng->GetAlliesCharacterList().size());
    m_ActiveDogtagList.resize(BFMng->GetAlliesCharacterList().size());
    for (int i = 0; i < BFMng->GetAlliesCharacterList().size(); i++)
    {
        m_Sprite[i] = std::make_unique<SpriteCharacter>();
        m_Sprite[i]->setTextureId(L"DogtagTexture");
        m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
        m_Sprite[i]->setScale(100.0f, 100.0f, 0.1f);
        m_Sprite[i]->setSpriteIndex(0);
        m_Sprite[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_ActiveDogtagList[i] = true;
    }
    
    for (int i = 0; i < BFMng->GetAlliesCharacterList().size(); i++)
    {
        m_Sprite[i]->setPosition(kDogtagLeftPosX + kDogtagInterbal * i, kDogtagClosingPosY, 2.0f);
    }

    m_BaseSprite = std::make_unique<SpriteCharacter>();
    m_BaseSprite->setTextureId(L"DogtagBaseTexture");
    m_BaseSprite->SetCameraLabel(L"HUDCamera", 0);
    m_BaseSprite->SetGraphicsPipeLine(L"Sprite");
    m_BaseSprite->SetSpritePattern(0, 1, 1, pattern);
    m_BaseSprite->setScale(400.0f, 300.0f, 0.1f);
    m_BaseSprite->setSpriteIndex(0);
    m_BaseSprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_BaseSprite->setPosition(0.0f, 260.0f, 2.5f);
}

bool MovedCountHUD::frameAction()
{
    if (BFMng->GetHUDEnableCondition())
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        for (int i = 0; i < BFMng->GetAlliesCharacterList().size(); i++)
        {
            if (!BFMng->GetAlliesCharacterList()[i]->Moved)  //i番目のキャラクターがまだ行動していない時
            {
                m_Sprite[i]->setPosition(kDogtagLeftPosX + kDogtagInterbal * i, kDogtagClosingPosY, 2.0f);
                if (BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara != nullptr)
                {
                    if (BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara->CharaID == i)
                    {
                        switch (BFMng->GetMode())
                        {
                        case Mode::MenuMode:
                        case Mode::AttackMode:
                        case Mode::AbilityMode:
                        case Mode::MoveMode:
                            if (m_DogtagAnimCount < 0.15f)
                            {
                                m_DogtagAnimCount += m_TimeManager->GetDeltaTime();
                                m_DogtagPosY = m_DogtagAnimCount * 200.0f;
                                m_Sprite[i]->setPosition(kDogtagLeftPosX + kDogtagInterbal * i, kDogtagClosingPosY - m_DogtagPosY, 2.0f);
                            }
                            else if (0.15f < m_DogtagAnimCount && m_DogtagAnimCount < 0.2f)
                            {
                                m_DogtagAnimCount += m_TimeManager->GetDeltaTime();
                                m_DogtagPosY += 3.0f;
                                m_Sprite[i]->setPosition(kDogtagLeftPosX + kDogtagInterbal * i, kDogtagClosingPosY - m_DogtagPosY, 2.0f);
                            }
                            else if (0.2f < m_DogtagAnimCount)
                            {
                                m_DogtagPosY = 30.0f;
                                m_Sprite[i]->setPosition(kDogtagLeftPosX + kDogtagInterbal * i, kDogtagClosingPosY - m_DogtagPosY, 2.0f);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
                
                pipe->AddRenderObject(m_Sprite[i].get());
            }
            else    //i番目のキャラクターが行動済みの時
            {
                if (m_ActiveDogtagList[i])  //ドッグタグUIがまだ出ている場合
                {
                    if (m_DogtagClosingCount < 0.5f)
                    {
                        m_DogtagClosingCount += m_TimeManager->GetDeltaTime();
                        auto dogtagClosingPosY = kDogtagClosingPosY + m_DogtagClosingCount * 300.0f;
                        m_Sprite[i]->setPosition(kDogtagLeftPosX + kDogtagInterbal * i, dogtagClosingPosY, 2.0f);
                        pipe->AddRenderObject(m_Sprite[i].get());
                    }
                    else
                    {
                        m_DogtagClosingCount = 0.0f;
                        m_ActiveDogtagList[i] = false;
                    }
                }
            }
        }

        pipe->AddRenderObject(m_BaseSprite.get());
    }
    
    return true;
}

void MovedCountHUD::finishAction()
{
}

void MovedCountHUD::ResetAnimCount()
{
    m_DogtagAnimCount = 0.0f;
}

void MovedCountHUD::ResetMovedCountUI()
{
    for (int i = 0; i < m_ActiveDogtagList.size(); i++)
    {
        m_ActiveDogtagList[i] = true;
    }
    m_DogtagClosingCount = 0.0f;
    ResetAnimCount();
}
