#include "MenuUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>


#include "SceneManager.h"
#include "SpriteCharacter.h"

using namespace std::chrono;

void MenuUI::initAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();

    m_sprite.resize(18);

    XMFLOAT4 pattern(0, 0, 1, 1);

    for (int i = 0; i < m_sprite.size(); i++)
    {
        m_sprite[i] = std::make_unique<SpriteCharacter>();
        m_sprite[i]->setTextureId(L"NixieTubesTexture");
        m_sprite[i]->SetCameraLabel(L"HUDCamera", 0);
        m_sprite[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_sprite[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_sprite[i]->SetSpritePattern(0, 1, 1, pattern);
        m_sprite[i]->setScale(110.0f, 90.0f, 0.1f);
        m_sprite[i]->setSpriteIndex(0);
    }
    
    m_sprite[0]->setPosition(posx[1], posy[0], m_PosZ);
    m_sprite[1]->setPosition(posx[2], posy[0], m_PosZ);

    m_sprite[2]->setPosition(posx[1], posy[1], m_PosZ);
    m_sprite[3]->setPosition(posx[2], posy[1], m_PosZ);

    m_sprite[4]->setPosition(posx[1], posy[2], m_PosZ);
    m_sprite[5]->setPosition(posx[2], posy[2], m_PosZ);

    m_sprite[6]->setPosition(posx[1], posy[3], m_PosZ);
    m_sprite[7]->setPosition(posx[2], posy[3], m_PosZ);

    m_sprite[8]->setPosition(posx[1], posy[4], m_PosZ);
    m_sprite[9]->setPosition(posx[2], posy[4], m_PosZ);
    m_sprite[10]->setPosition(posx[3], posy[4], m_PosZ);
    m_sprite[11]->setPosition(posx[4], posy[4], m_PosZ);
    m_sprite[12]->setPosition(posx[5], posy[4], m_PosZ);

    m_sprite[13]->setPosition(posx[0], posy[0], m_PosZ);
    m_sprite[14]->setPosition(posx[0], posy[1], m_PosZ);
    m_sprite[15]->setPosition(posx[0], posy[2], m_PosZ);
    m_sprite[16]->setPosition(posx[0], posy[3], m_PosZ);
    m_sprite[17]->setPosition(posx[0], posy[4], m_PosZ);
}

bool MenuUI::frameAction()
{
    if (MenuUIenable)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
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

            m_sprite[0]->setPosition(posx[1], posy[0], m_PosZ);
            m_sprite[1]->setPosition(posx[2], posy[0], m_PosZ);

            m_sprite[2]->setPosition(posx[1], posy[1], m_PosZ);
            m_sprite[3]->setPosition(posx[2], posy[1], m_PosZ);

            m_sprite[4]->setPosition(posx[1], posy[2], m_PosZ);
            m_sprite[5]->setPosition(posx[2], posy[2], m_PosZ);

            m_sprite[6]->setPosition(posx[1], posy[3], m_PosZ);
            m_sprite[7]->setPosition(posx[2], posy[3], m_PosZ);

            m_sprite[8]->setPosition(posx[1], posy[4], m_PosZ);
            m_sprite[9]->setPosition(posx[2], posy[4], m_PosZ);
            m_sprite[10]->setPosition(posx[3], posy[4], m_PosZ);
            m_sprite[11]->setPosition(posx[4], posy[4], m_PosZ);
            m_sprite[12]->setPosition(posx[5], posy[4], m_PosZ);

            m_sprite[13]->setPosition(posx[0], posy[0], m_PosZ);
            m_sprite[14]->setPosition(posx[0], posy[1], m_PosZ);
            m_sprite[15]->setPosition(posx[0], posy[2], m_PosZ);
            m_sprite[16]->setPosition(posx[0], posy[3], m_PosZ);
            m_sprite[17]->setPosition(posx[0], posy[4], m_PosZ);
        }

        //=========================================閉じる========================================
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

            m_sprite[0]->setPosition(posx[1], posy[0], m_PosZ);
            m_sprite[1]->setPosition(posx[2], posy[0], m_PosZ);

            m_sprite[2]->setPosition(posx[1], posy[1], m_PosZ);
            m_sprite[3]->setPosition(posx[2], posy[1], m_PosZ);

            m_sprite[4]->setPosition(posx[1], posy[2], m_PosZ);
            m_sprite[5]->setPosition(posx[2], posy[2], m_PosZ);

            m_sprite[6]->setPosition(posx[1], posy[3], m_PosZ);
            m_sprite[7]->setPosition(posx[2], posy[3], m_PosZ);

            m_sprite[8]->setPosition(posx[1], posy[4], m_PosZ);
            m_sprite[9]->setPosition(posx[2], posy[4], m_PosZ);
            m_sprite[10]->setPosition(posx[3], posy[4], m_PosZ);
            m_sprite[11]->setPosition(posx[4], posy[4], m_PosZ);
            m_sprite[12]->setPosition(posx[5], posy[4], m_PosZ);

            m_sprite[13]->setPosition(posx[0], posy[0], m_PosZ);
            m_sprite[14]->setPosition(posx[0], posy[1], m_PosZ);
            m_sprite[15]->setPosition(posx[0], posy[2], m_PosZ);
            m_sprite[16]->setPosition(posx[0], posy[3], m_PosZ);
            m_sprite[17]->setPosition(posx[0], posy[4], m_PosZ);
        }

        for (int i = 0; i < m_sprite.size(); i++)
        {
            pipe->AddRenderObject(m_sprite[i].get());
        }
        
    }

    return true;
}

void MenuUI::finishAction()
{
	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);
}

void MenuUI::OpenMenuAnim()
{
    posx[0] = -450.0f - 500.0f;
    posx[1] = -410.0f - 500.0f;
    posx[2] = -370.0f - 500.0f;
    posx[3] = -330.0f - 500.0f;
    posx[4] = -290.0f - 500.0f;
    posx[5] = -250.0f - 500.0f;

    OpenAnim = true;
    MenuAnimCount = 0.0f;
    lastTime = steady_clock::now();
}

void MenuUI::CloseMenuAnim()
{
    OpenAnim = false;
    CloseAnim = true;
    MenuAnimCount = 0.0f;
    lastTime = steady_clock::now();
}
