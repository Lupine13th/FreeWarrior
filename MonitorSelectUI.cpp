#include "MonitorSelectUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "TimeManager.h"

using namespace std::chrono;

void MonitorSelectUI::ResetAnim()   //バーの位置が画面外に戻る
{
    Animation = true;
    Gageposx = -700.0f;
    m_Sprite[5]->setPosition(Gageposx, 143.0f, 1.5f);
    AnimCount = 0.0f;
}

void MonitorSelectUI::OpenAnim()    //メニューの位置を画面外へ＆アニメーション開始フラグをオン
{
    firstAnim = true;
    MenuBaseposx = -980.0f;
    NixieBaseposx = -850.0f;
    firstAnimCount = 0.0f;
    Animation = false;
}

void MonitorSelectUI::CloseAnim()   //メニューの位置を画面外へ移動するアニメーション
{
    firstAnim = false;
    Animation = false;
    CloseAnimation = true;
    closeAnimCount = 0.0f;
}

void MonitorSelectUI::InitAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    CharacterData* chData = GetGameObject()->GetCharacterData();

    m_Sprite.resize(7);

    engine->InitCameraConstantBuffer(chData);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));

    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    for (int i = 0; i < 7; i++)
    {
		if (i < 5)          //ニキシー管の部分
        {
            m_Sprite[i] = std::make_unique<SpriteCharacter>();
            m_Sprite[i]->SetTextureId(L"NixieBaseTexture");
            m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
            m_Sprite[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
            m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
            m_Sprite[i]->setScale(250.0f, 250.0f, 1.0f);
            m_Sprite[i]->setSpriteIndex(0);
        }
		else if (i == 5)    //メニューの部分
        {
            m_Sprite[i] = std::make_unique<SpriteCharacter>();
            m_Sprite[i]->SetTextureId(L"MenuBaseTexture");
            m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
            m_Sprite[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
            m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
            m_Sprite[i]->setScale(200.0f, 330.0f, 1.0f);
            m_Sprite[i]->setSpriteIndex(0);
        }
		else if (i == 6)    //ゲージの部分
        {
            m_Sprite[i] = std::make_unique<SpriteCharacter>();
            m_Sprite[i]->SetTextureId(L"NixieGageTexture");
            m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
            m_Sprite[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
            m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
            m_Sprite[i]->setScale(300.0f, 300.0f, 1.0f);
            m_Sprite[i]->setSpriteIndex(0);
        }
    }

    m_Sprite[0]->setPosition(NixieBaseposx, 140.0f, 1.0f);
    m_Sprite[1]->setPosition(NixieBaseposx, 60.0f, 1.0f);
    m_Sprite[2]->setPosition(NixieBaseposx, -20.0f, 1.0f);
    m_Sprite[3]->setPosition(NixieBaseposx, -100.0f, 1.0f);
    m_Sprite[4]->setPosition(NixieBaseposx, -180.0f, 1.0f);
    m_Sprite[5]->setPosition(MenuBaseposx, 60.0f, 0.5f);
}

bool MonitorSelectUI::FrameAction()
{

    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    if (BFMng->GetMenuUI()->MenuUIenable)
    {
        if (!CloseAnimation)
        {
            if (firstAnim && !Animation)    //メニューUIが出てくるアニメ―ション
            {
                if (firstAnimCount < 0.5f)
                {
                    persent = (0.5f - firstAnimCount) / 0.5f;
					Gageposx = -700.0f;
                    MenuBaseposx = -480.0f - 500.0f * persent;
                    NixieBaseposx = -350.0f - 500.0f * persent;
                    firstAnimCount += m_TimeManager->GetDeltaTime();
                }
                else if (firstAnimCount > 0.5f)
                {
                    firstAnimCount = 0.0f;
                    firstAnim = false;
                    MenuBaseposx = -480.0f;
                    NixieBaseposx = -350.0f;
                    ResetAnim();
                }

                m_Sprite[0]->setPosition(NixieBaseposx, 140.0f, 1.0f);
                m_Sprite[1]->setPosition(NixieBaseposx, 60.0f, 1.0f);
                m_Sprite[2]->setPosition(NixieBaseposx, -20.0f, 1.0f);
                m_Sprite[3]->setPosition(NixieBaseposx, -100.0f, 1.0f);
                m_Sprite[4]->setPosition(NixieBaseposx, -180.0f, 1.0f);
                m_Sprite[5]->setPosition(MenuBaseposx, 60.0f, 0.5f);
                m_Sprite[6]->setPosition(Gageposx, 60.0f, 0.5f);

                for (int i = 0; i < m_Sprite.size(); i++)
                {
                    pipe->AddRenderObject(m_Sprite[i].get());
                }
            }
            else if (!firstAnim && Animation)   //メニューUIのバーのアニメーション
            {
                if (AnimCount < 0.5f)
                {
                    persent = (0.5f - AnimCount) / 0.5f;
                    Gageposx = -350.0f - 350.0f * persent;
                    AnimCount += m_TimeManager->GetDeltaTime();
                }
                else if (AnimCount > 0.5f)
                {
                    AnimCount = 0.0f;
                    Animation = false;
                    Gageposx = -350.0f;
                }

                for (int i = 0; i < m_Sprite.size(); i++)
                {
                    pipe->AddRenderObject(m_Sprite[i].get());
                }
            }
            else
            {
                for (int i = 0; i < m_Sprite.size(); i++)
                {
                    pipe->AddRenderObject(m_Sprite[i].get());
                }
            }
        }
        else
        {
            if (closeAnimCount < 0.5f)
            {
                persent = closeAnimCount / 0.5f;
                MenuBaseposx = -480.0f - 500.0f * persent;
                NixieBaseposx = -350.0f - 500.0f * persent;
                closeAnimCount += m_TimeManager->GetDeltaTime();
                //persent = closeAnimCount / 0.5f;
            }
            else if (closeAnimCount > 0.5f)
            {
                closeAnimCount = 0.0f;
                CloseAnimation = false;
                MenuBaseposx = -480.0f - 500.0f;
                NixieBaseposx = -350.0f - 500.0f;
            }

            m_Sprite[0]->setPosition(NixieBaseposx, 140.0f, 1.0f);
            m_Sprite[1]->setPosition(NixieBaseposx, 60.0f, 1.0f);
            m_Sprite[2]->setPosition(NixieBaseposx, -20.0f, 1.0f);
            m_Sprite[3]->setPosition(NixieBaseposx, -100.0f, 1.0f);
            m_Sprite[4]->setPosition(NixieBaseposx, -180.0f, 1.0f);
            m_Sprite[5]->setPosition(MenuBaseposx, 60.0f, 0.5f);

            for (int i = 0; i < 6; i++)
            {
                if (i != 6)
                {
                    pipe->AddRenderObject(m_Sprite[i].get());
                }
            }
        }
        

        //ゲージのアニメーション
        switch (BFMng->GetMenuSelectIndex())
        {
        default:
            break;
        case 0:
            m_Sprite[6]->setPosition(Gageposx, 143.0f, 1.5f);
            break;
        case 1:
            m_Sprite[6]->setPosition(Gageposx, 63.0f, 1.5f);
            break;
        case 2:
            m_Sprite[6]->setPosition(Gageposx, -17.0f, 1.5f);
            break;
        case 3:
            m_Sprite[6]->setPosition(Gageposx, -97.0f, 1.5f);
            break; 
        case 4:
            m_Sprite[6]->setPosition(Gageposx, -177.0f, 1.5f);
            break; 
        }
    }
	return true;
}

void MonitorSelectUI::FinishAction()
{
}
