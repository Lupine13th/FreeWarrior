#include "SoldierHUD.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>

#include "SceneManager.h"
#include "SpriteCharacter.h"

int SoldierHUD::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
    int count = startIndex;

    while (*str != '\0')
    {
        if (std::find(m_WordList.m_chListDigi, m_ChEnd, *str) != m_ChEnd)
        {
            m_WordSprites[count]->SetSpritePattern(0, width, height, m_FontMap[*str]);
            m_WordSprites[count]->setSpriteIndex(0);

            m_WordSprites[count]->setPosition(ltX, ltY, 0.0f);
            count++;
        }

        ltX += width + 2.0f;

        str++;
    }

    return count;
}

int SoldierHUD::MakeSpriteStringRightAligned(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
    int count = startIndex;

    int len = 0;
    const wchar_t* tmp = str;
    while (*tmp != L'\0')
    {
        if (std::find(m_WordList.m_chListDigi, m_ChEnd, *tmp) != m_ChEnd)
            len++;
        tmp++;
    }

    float totalWidth = len * (width + 2.0f) - 2.0f;

    float startX = ltX - totalWidth;

    while (*str != L'\0')
    {
        if (std::find(m_WordList.m_chListDigi, m_ChEnd, *str) != m_ChEnd)
        {
            m_WordSprites[count]->SetSpritePattern(0, width, height, m_FontMap[*str]);
            m_WordSprites[count]->setSpriteIndex(0);
            m_WordSprites[count]->setPosition(startX, ltY, 0.0f);
            count++;
            startX += width + 2.0f;
        }

        str++;
    }

    return count;
}

void SoldierHUD::initAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    //==========兵数メーター===========
    m_SoldierMeterSprite = std::make_unique<SpriteCharacter>();
    m_SoldierMeterSprite->setTextureId(L"SoldierTexture");
    m_SoldierMeterSprite->SetCameraLabel(L"HUDCamera", 0);
    m_SoldierMeterSprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_SoldierMeterSprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_SoldierMeterSprite->SetSpritePattern(0, 1, 1, pattern);
    m_SoldierMeterSprite->setScale(200.0f, 200.0f, 0.1f);
    m_SoldierMeterSprite->setSpriteIndex(0);
    m_SoldierMeterSprite->setPosition(380.0f, 70.0f, 2.0f);

    //==========士気メーター===========
    m_MoraleMeterSprite = std::make_unique<SpriteCharacter>();
    m_MoraleMeterSprite->setTextureId(L"MoraleTexture");
    m_MoraleMeterSprite->SetCameraLabel(L"HUDCamera", 0);
    m_MoraleMeterSprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_MoraleMeterSprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_MoraleMeterSprite->SetSpritePattern(0, 1, 1, pattern);
    m_MoraleMeterSprite->setScale(200.0f, 200.0f, 0.1f);
    m_MoraleMeterSprite->setSpriteIndex(0);
    m_MoraleMeterSprite->setPosition(380.0f, -30.0f, 2.0f);

    //Text
    m_WordCount = 50;

    SpriteCharacter* spc;

    for (int i = 0; i < m_WordCount; i++)
    {
        spc = new SpriteCharacter();

        spc->setTextureId(L"DiginalHUDTexture");
        spc->SetCameraLabel(L"HUDCamera", 0);

        spc->setColor(1, 1, 1, 1);

        //UI用のパイプラインは別にすべきなんだけども、記述量を減らしたいので使いまわし
        spc->SetGraphicsPipeLine(L"AlphaSprite");

        m_WordSprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
    }

    //FontMap
    Texture2DContainer* tex = engine->GetTextureManager()->GetTexture(L"DiginalHUDTexture");

    m_FontMap.reserve(wcslen(m_WordList.m_chListDigi));

    m_ChEnd = m_WordList.m_chListDigi + wcslen(m_WordList.m_chListDigi);

    int index = 0;
    float invW = 1.0f / tex->fWidth;
    float invH = 1.0f / tex->fHeight;

    float x = 0.0f;
    float y = 0.0f;
    float w = 64.0f;
    float h = 114.0f;
    while (m_WordList.m_chListDigi[index] != '\0')
    {
        XMFLOAT4 r = { x * invW, y * invH, w * invW, h * invH };
        m_FontMap[m_WordList.m_chListDigi[index]] = r;
        x += 64.0f;

        if (x >= tex->fWidth)
        {
            x = 0.0f;
            y += h;
        }

        index++;
    }
}

bool SoldierHUD::frameAction()
{
    if (BFMng->GetHUDEnableCondition())
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");
        GraphicsPipeLineObjectBase* wordPipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");

        int count = 0;

        int SelectId = 0;

        wstring soldiers;
        wstring morale;

        //兵数を表示するデジタル数字
        switch (BFMng->GetMode())
        {
        default:
            break;
        case Mode::FieldMode:
        case Mode::MenuMode:
            SelectId = BFMng->GetSelectID();
            if (BFMng->GetFieldSquaresList()[SelectId]->chara != nullptr)
            {
                if (BFMng->GetFieldSquaresList()[SelectId]->chara->CharaAdmin == Admin::Imperial && !BFMng->GetFieldSquaresList()[SelectId]->chara->Detected)
                {
                    soldiers = L"ERR";
                    morale = L"ERR";
                    break;
                }
                soldiers = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaSoldiers));
                morale = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaMorales));
            }
            break;
        case Mode::AttackMode:
        case Mode::MoveMode:
        case Mode::AbilityMode:
            SelectId = BFMng->GetTargetID();
            if (BFMng->GetFieldSquaresList()[SelectId]->chara != nullptr)
            {
                if (BFMng->GetFieldSquaresList()[SelectId]->chara->CharaAdmin == Admin::Imperial && !BFMng->GetFieldSquaresList()[SelectId]->chara->Detected)
                {
                    soldiers = L"ERR";
                    morale = L"ERR";
                    break;
                }
                soldiers = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaSoldiers));
                morale = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaMorales));
            }
            break;
        }

        count = MakeSpriteStringRightAligned(count, 341.0f, 110.0f, 15, 23, soldiers.c_str());
        count = MakeSpriteStringRightAligned(count, 341.0f, 10.0f, 15, 23, morale.c_str());

        for (int i = 0; i < count; i++)
        {
            //wordPipeLine->AddRenerObject(m_WordSprites[i].get());
        }

        /*pipe->AddRenerObject(m_SoldierMeterSprite.get());
        pipe->AddRenerObject(m_MoraleMeterSprite.get());*/
    }

    return true;
}

void SoldierHUD::finishAction()
{
    SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
    scene->RemoveCamera(this);
}
