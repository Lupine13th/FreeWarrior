#include "MenuUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>


#include "SceneManager.h"
#include "SpriteCharacter.h"

using namespace std::chrono;

void MenuUI::InitAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    CharacterData* chData = GetGameObject()->GetCharacterData();

    m_TimeManager = MyAccessHub::GetTimeManager();

    m_Sprite.resize(18);

    XMFLOAT4 pattern(0, 0, 1, 1);

    for (int i = 0; i < m_Sprite.size(); i++)
    {
        m_Sprite[i] = std::make_unique<SpriteCharacter>();
        m_Sprite[i]->SetTextureId(L"NixieTubesTexture");
        m_Sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_Sprite[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_Sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_Sprite[i]->SetSpritePattern(0, 1, 1, pattern);
        m_Sprite[i]->setScale(110.0f, 90.0f, 0.1f);
        m_Sprite[i]->setSpriteIndex(0);
    }
    
    m_Sprite[0]->setPosition(posx[1], posy[0], m_PosZ);
    m_Sprite[1]->setPosition(posx[2], posy[0], m_PosZ);

    m_Sprite[2]->setPosition(posx[1], posy[1], m_PosZ);
    m_Sprite[3]->setPosition(posx[2], posy[1], m_PosZ);

    m_Sprite[4]->setPosition(posx[1], posy[2], m_PosZ);
    m_Sprite[5]->setPosition(posx[2], posy[2], m_PosZ);

    m_Sprite[6]->setPosition(posx[1], posy[3], m_PosZ);
    m_Sprite[7]->setPosition(posx[2], posy[3], m_PosZ);

    m_Sprite[8]->setPosition(posx[1], posy[4], m_PosZ);
    m_Sprite[9]->setPosition(posx[2], posy[4], m_PosZ);
    m_Sprite[10]->setPosition(posx[3], posy[4], m_PosZ);
    m_Sprite[11]->setPosition(posx[4], posy[4], m_PosZ);
    m_Sprite[12]->setPosition(posx[5], posy[4], m_PosZ);

    m_Sprite[13]->setPosition(posx[0], posy[0], m_PosZ);
    m_Sprite[14]->setPosition(posx[0], posy[1], m_PosZ);
    m_Sprite[15]->setPosition(posx[0], posy[2], m_PosZ);
    m_Sprite[16]->setPosition(posx[0], posy[3], m_PosZ);
    m_Sprite[17]->setPosition(posx[0], posy[4], m_PosZ);
}

bool MenuUI::FrameAction()
{
    if (MenuUIenable)
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        if (OpenAnim)
        {
            auto currentTime = steady_clock::now();

            duration<float> elapsedTime = currentTime - lastTime;

            float deltaTime = elapsedTime.count();

            lastTime = currentTime;

            if (MenuAnimCount < 0.5f)
            {
                posx[0] = -450.0f - 500.0f * persent;
                posx[1] = -410.0f - 500.0f * persent;
                posx[2] = -370.0f - 500.0f * persent;
                posx[3] = -330.0f - 500.0f * persent;
                posx[4] = -290.0f - 500.0f * persent;
                posx[5] = -250.0f - 500.0f * persent;
                MenuAnimCount += deltaTime;
                persent = (0.5f - MenuAnimCount) / 0.5f;
            }
            else if (MenuAnimCount > 0.5f)
            {
                MenuAnimCount = 0.0f;
                OpenAnim = false;
                posx[0] = -450.0f;
                posx[1] = -410.0f;
                posx[2] = -370.0f;
                posx[3] = -330.0f;
                posx[4] = -290.0f;
                posx[5] = -250.0f;
            }

            m_Sprite[0]->setPosition(posx[1], posy[0], m_PosZ);
            m_Sprite[1]->setPosition(posx[2], posy[0], m_PosZ);

            m_Sprite[2]->setPosition(posx[1], posy[1], m_PosZ);
            m_Sprite[3]->setPosition(posx[2], posy[1], m_PosZ);

            m_Sprite[4]->setPosition(posx[1], posy[2], m_PosZ);
            m_Sprite[5]->setPosition(posx[2], posy[2], m_PosZ);

            m_Sprite[6]->setPosition(posx[1], posy[3], m_PosZ);
            m_Sprite[7]->setPosition(posx[2], posy[3], m_PosZ);

            m_Sprite[8]->setPosition(posx[1], posy[4], m_PosZ);
            m_Sprite[9]->setPosition(posx[2], posy[4], m_PosZ);
            m_Sprite[10]->setPosition(posx[3], posy[4], m_PosZ);
            m_Sprite[11]->setPosition(posx[4], posy[4], m_PosZ);
            m_Sprite[12]->setPosition(posx[5], posy[4], m_PosZ);

            m_Sprite[13]->setPosition(posx[0], posy[0], m_PosZ);
            m_Sprite[14]->setPosition(posx[0], posy[1], m_PosZ);
            m_Sprite[15]->setPosition(posx[0], posy[2], m_PosZ);
            m_Sprite[16]->setPosition(posx[0], posy[3], m_PosZ);
            m_Sprite[17]->setPosition(posx[0], posy[4], m_PosZ);
        }

        //===========閉じるアニメーション==========
        if (CloseAnim)
        {
            auto currentTime = steady_clock::now();

            duration<float> elapsedTime = currentTime - lastTime;

            float deltaTime = elapsedTime.count();

            lastTime = currentTime;

            if (MenuAnimCount < 0.5f)
            {
                posx[0] = -450.0f - 500.0f * persent;
                posx[1] = -410.0f - 500.0f * persent;
                posx[2] = -370.0f - 500.0f * persent;
                posx[3] = -330.0f - 500.0f * persent;
                posx[4] = -290.0f - 500.0f * persent;
                posx[5] = -250.0f - 500.0f * persent;
                MenuAnimCount += deltaTime;
                persent = MenuAnimCount / 0.5f;
            }
            else if (MenuAnimCount > 0.5f)
            {
                MenuAnimCount = 0.0f;
                CloseAnim = false;
                MenuUIenable = false;
                posx[0] = -450.0f - 500.0f;
                posx[1] = -410.0f - 500.0f;
                posx[2] = -370.0f - 500.0f;
                posx[3] = -330.0f - 500.0f;
                posx[4] = -290.0f - 500.0f;
                posx[5] = -250.0f - 500.0f;
            }

            m_Sprite[0]->setPosition(posx[1], posy[0], m_PosZ);
            m_Sprite[1]->setPosition(posx[2], posy[0], m_PosZ);

            m_Sprite[2]->setPosition(posx[1], posy[1], m_PosZ);
            m_Sprite[3]->setPosition(posx[2], posy[1], m_PosZ);

            m_Sprite[4]->setPosition(posx[1], posy[2], m_PosZ);
            m_Sprite[5]->setPosition(posx[2], posy[2], m_PosZ);

            m_Sprite[6]->setPosition(posx[1], posy[3], m_PosZ);
            m_Sprite[7]->setPosition(posx[2], posy[3], m_PosZ);

            m_Sprite[8]->setPosition(posx[1], posy[4], m_PosZ);
            m_Sprite[9]->setPosition(posx[2], posy[4], m_PosZ);
            m_Sprite[10]->setPosition(posx[3], posy[4], m_PosZ);
            m_Sprite[11]->setPosition(posx[4], posy[4], m_PosZ);
            m_Sprite[12]->setPosition(posx[5], posy[4], m_PosZ);

            m_Sprite[13]->setPosition(posx[0], posy[0], m_PosZ);
            m_Sprite[14]->setPosition(posx[0], posy[1], m_PosZ);
            m_Sprite[15]->setPosition(posx[0], posy[2], m_PosZ);
            m_Sprite[16]->setPosition(posx[0], posy[3], m_PosZ);
            m_Sprite[17]->setPosition(posx[0], posy[4], m_PosZ);
        }

        //for (int i = 0; i < m_Sprite.size(); i++)
        //{
        //    pipe->AddRenderObject(m_Sprite[i].get());
        //}
        
    }

    return true;
}

void MenuUI::FinishAction()
{
	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);
}

//開くアニメーション
void MenuUI::OpenMenuAnim()
{
    //位置を初期化
    posx[0] = -450.0f - 500.0f;
    posx[1] = -410.0f - 500.0f;
    posx[2] = -370.0f - 500.0f;
    posx[3] = -330.0f - 500.0f;
    posx[4] = -290.0f - 500.0f;
    posx[5] = -250.0f - 500.0f;

    //アニメーションスタート
    OpenAnim = true;
    MenuAnimCount = 0.0f;
    lastTime = steady_clock::now();
}

//閉じるアニメーション
void MenuUI::CloseMenuAnim()
{
    OpenAnim = false;
    CloseAnim = true;
    MenuAnimCount = 0.0f;
    lastTime = steady_clock::now();
}
