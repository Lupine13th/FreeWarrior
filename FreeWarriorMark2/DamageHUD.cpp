#include "DamageHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

#include "SceneManager.h"
#include "SpriteCharacter.h"

void DamageHUD::InitAction()
{
    //MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    //CharacterData* chData = GetGameObject()->GetCharacterData();

    //engine->InitCameraConstantBuffer(chData);

    //XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    //XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    //XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));

    //engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
    //engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));

    //XMFLOAT4 pattern(0.0f, 0.0f, 1.0f / 512.0f, 1.0f / 512.0f);

    //m_Sprite.resize(3);

    //for (int i = 0; i < m_Sprite.size(); i++)
    //{
    //    m_Sprite[i] = std::make_unique<SpriteCharacter>();
    //    m_Sprite[i]->SetTextureId(L"Sprite00");
    //    m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
    //    m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
    //    m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
    //    m_Sprite[i]->setSpriteIndex(0);
    //    m_Sprite[i]->SetPosition(OrigenBarPosX, kBarPosY, 2.0f);
    //    m_Sprite[i]->SetScale(OriginBarSizeX, 10.0f, 0.1f);
    //}

    //m_Sprite[0]->SetColor(0.0f, 1.0f, 0.0f, 1);                 //バーの緑部分
    //m_Sprite[0]->SetPosition(OrigenBarPosX, kBarPosY, 1.0f);

    //m_Sprite[1]->SetColor(1.0f, 0.0f, 0.0f, 1);                 //バーの赤部分
    //m_Sprite[1]->SetPosition(OrigenBarPosX, kBarPosY, 2.0f);

    //m_Sprite[2]->SetColor(0.0f, 0.0f, 0.0f, 1);                 //バーの背景部分
    //m_Sprite[2]->SetPosition(OrigenBarPosX, kBarPosY, 2.5f);
    //m_Sprite[2]->SetScale(OriginBarSizeX + 5.0f, 15.0f, 0.1f);
}

bool DamageHUD::FrameAction()
{
    //MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    //GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    //std::wstring cstr;

    //switch (m_DamegeAnimationState)
    //{
    //case DamegeAnimationState::Delay:
    //    m_DelayCount += m_TimeManager->GetDeltaTime();
    //    if (m_DelayCount > kDelayTime)
    //    {
    //        m_DamegeAnimationState = DamegeAnimationState::Play;
    //    }
    //    break;
    //case DamegeAnimationState::Play:
    //    if (animCount == 0.0f)
    //    {
    //        RedBarSizeX = startRedSizeX;
    //        m_Sprite[1]->SetScale(RedBarSizeX, 10.0f, 0.1f);
    //        m_Sprite[1]->SetPosition(kBarPosLeftX + RedBarSizeX / 2.0f, kBarPosY, 2.0f);
    //    }
    //    if (animCount < 0.5f)
    //    {
    //        float t = animCount / 0.5f;
    //        GreenBarSizeX = startGreenSizeX + (endGreenSizeX - startGreenSizeX) * t;
    //        m_Sprite[0]->SetScale(GreenBarSizeX, 10.0f, 0.1f);
    //        m_Sprite[0]->SetPosition(kBarPosLeftX + GreenBarSizeX / 2.0f, kBarPosY, 1.0f);
    //    }
    //    else if (animCount < 1.0f)
    //    {
    //        float t = (animCount - 0.5f) / 0.5f;
    //        RedBarSizeX = startRedSizeX + (endRedSizeX - startRedSizeX) * t;
    //        m_Sprite[1]->SetScale(RedBarSizeX, 10.0f, 0.1f);
    //        m_Sprite[1]->SetPosition(kBarPosLeftX + RedBarSizeX / 2.0f, kBarPosY, 2.0f);
    //    }
    //    else
    //    {
    //        animCount = 0.0f;
    //        m_DelayCount = 0.0f;
    //        m_DamegeAnimationState = DamegeAnimationState::None;
    //    }

    //    for (int i = 0; i < m_Sprite.size(); i++)
    //    {
    //        pipe->AddRenderObject(m_Sprite[i].get());
    //    }
    //    animCount += m_TimeManager->GetDeltaTime();
    //    break;
    //}
    return true;
}

void DamageHUD::FinishAction()
{
}

void DamageHUD::SetDamage(float damage, float maxSoldiers, float soldiers)
{
    // 残りHPの割合
    float solPercentNow = soldiers / maxSoldiers;
    float solPercentAfter = (soldiers - damage) / maxSoldiers;
    Damage = damage;

    //緑バーの開始と終了のサイズを記録
    startGreenSizeX = OriginBarSizeX * solPercentNow;
    endGreenSizeX = OriginBarSizeX * solPercentAfter;

    //赤バーの開始と終了のサイズを記録
    startRedSizeX = startGreenSizeX;
    endRedSizeX = endGreenSizeX;

    animCount = 0.0f;
    m_DamegeAnimationState = DamegeAnimationState::Delay;
}
