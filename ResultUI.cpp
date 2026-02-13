#include "ResultUI.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "KeyBindComponent.h"

int ResultUI::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
    int count = startIndex;

    while (*str != '\0')
    {
        if (std::find(m_WordList.m_chListNewsPaperJ, m_ChEnd, *str) != m_ChEnd)
        {
            m_WordSprites[count]->SetSpritePattern(0, width, height, m_FontMap[*str]);
            m_WordSprites[count]->setSpriteIndex(0);
            m_WordSprites[count]->setColor(0, 0, 0, 1);
            m_WordSprites[count]->setPosition(ltX, ltY, 0.5f);
            count++;
        }

        ltX += width;

        str++;
    }

    return count;
}

void ResultUI::initAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    m_FoldPaperSprite = std::make_unique<SpriteCharacter>();  //最初の折っている新聞
    m_FoldPaperSprite->SetCameraLabel(L"HUDCamera", 0);
    m_FoldPaperSprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_FoldPaperSprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_FoldPaperSprite->SetSpritePattern(0, 1, 1, pattern);
    m_FoldPaperSprite->setScale(200.0f, 200.0f, 0.1f);
    m_FoldPaperSprite->setPosition(m_FoldNewsPaperPos.x, m_FoldNewsPaperPos.y, m_FoldNewsPaperPos.z);
    m_FoldPaperSprite->setSpriteIndex(0);
    m_FoldPaperSprite->setTextureId(L"NewsPaperTexture");
    
    m_NewsPaperSprite = std::make_unique<SpriteCharacter>();  //勝利or敗北の一枚
    m_NewsPaperSprite->SetCameraLabel(L"HUDCamera", 0);
    m_NewsPaperSprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_NewsPaperSprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_NewsPaperSprite->SetSpritePattern(0, 1, 1, pattern);
    m_NewsPaperSprite->setScale(500.0f, 500.0f, 0.1f);
    m_NewsPaperSprite->setPosition(0.0f, 0.0f, 1.0f);
    m_NewsPaperSprite->setSpriteIndex(0);
    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTexture"); //表示されるタイミングでまたイメージを切り替える

    m_NewsPaperResultSprite = std::make_unique<SpriteCharacter>();  //勝利時のリザルト画面
    m_NewsPaperResultSprite->SetCameraLabel(L"HUDCamera", 0);
    m_NewsPaperResultSprite->setColor(1.0f, 1.0f, 1.0f, 1);
    m_NewsPaperResultSprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_NewsPaperResultSprite->SetSpritePattern(0, 1, 1, pattern);
    m_NewsPaperResultSprite->setScale(500.0f, 500.0f, 0.1f);
    m_NewsPaperResultSprite->setPosition(0.0f, 0.0f, 1.5f);
    m_NewsPaperResultSprite->setSpriteIndex(0);
    m_NewsPaperResultSprite->setTextureId(L"NewsPaperWinResultTexture");

    m_Fadeprite = std::make_unique<SpriteCharacter>();  //背景が暗転する
    m_Fadeprite->SetCameraLabel(L"HUDCamera", 0);
    m_Fadeprite->setColor(1.0f, 1.0f, 1.0f, 0.0f);
    m_Fadeprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_Fadeprite->SetSpritePattern(0, 1, 1, pattern);
    m_Fadeprite->setScale(1000.0f, 600.0f, 0.1f);
    m_Fadeprite->setPosition(0.0f, 0.0f, 2.0f);
    m_Fadeprite->setSpriteIndex(0);
    m_Fadeprite->setTextureId(L"Sprite00");

    m_WordCount = 50;

    SpriteCharacter* spc;

    for (int i = 0; i < m_WordCount; i++)
    {
        spc = new SpriteCharacter();

        spc->setTextureId(L"JPNHUDTextureNP");
        spc->SetCameraLabel(L"HUDCamera", 0);

        spc->setColor(1, 1, 1, 1);

        spc->SetGraphicsPipeLine(L"AlphaSprite");

        m_WordSprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
    }

    Texture2DContainer* tex = engine->GetTextureManager()->GetTexture(L"JPNHUDTextureNP");

    m_FontMap.reserve(wcslen(m_WordList.m_chListNewsPaperJ));

    m_ChEnd = m_WordList.m_chListNewsPaperJ + wcslen(m_WordList.m_chListNewsPaperJ);

    int index = 0;
    float invW = 1.0f / tex->fWidth;
    float invH = 1.0f / tex->fHeight;

    float x = 0.0f;
    float y = 0.0f;
    float w = 64.0f;
    float h = 114.0f;
    while (m_WordList.m_chListNewsPaperJ[index] != '\0')
    {
        XMFLOAT4 r = { x * invW, y * invH, w * invW, h * invH };
        m_FontMap[m_WordList.m_chListNewsPaperJ[index]] = r;
        x += 64.0f;

        if (x >= tex->fWidth)
        {
            x = 0.0f;
            y += h;
        }

        index++;
    }
}

bool ResultUI::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* spritePipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");
    GraphicsPipeLineObjectBase* wordPipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
    SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
    KeyBindComponent* keycomp = static_cast<KeyBindComponent*>(p_scene->getKeyComponent());

    int count = 0;

    wstring turn = L"";
    wstring killCount = L"";
    wstring killedCount = L"";
    wstring lastScore = L"";
    wstring pressSpace = L"スペースキーで終了";

    if (BFMng->GetCurrentTurn() == Turn::Result)
    {
        switch (m_ResultStates)
        {
        case ResultStates::Init:
            if (BFMng->GetPlayerWin())  //プレイヤーが勝利した場合
            {
                m_PlayerWin = true;
            }
            else
            {
                m_PlayerWin = false;
            }
            m_ResultStates = ResultStates::PostedAnim;
            break;
        case ResultStates::PostedAnim:    //新聞が投函されるアニメーション

            m_PostedAnimCount += m_TimeManager->GetDeltaTime();
            if (m_PostedAnimCount < 1.0f)
            {
                //暗転していくアニメーション
                m_FadeAlpha += 0.05f;
                if (m_FadeAlpha > 1.0f)
                {
                    m_FadeAlpha = 1.0f;
                }
                m_Fadeprite->setColor(0, 0, 0, m_FadeAlpha);

                //新聞が下に落ちるアニメーション
                m_FoldNewsPaperPos.y -= 15.0f;
                m_FoldPaperSprite->setPosition(m_FoldNewsPaperPos.x, m_FoldNewsPaperPos.y, m_FoldNewsPaperPos.z);
            }
            else if (m_PostedAnimCount > 1.0f && m_PostedAnimCount < 3.0f)
            {
                m_FoldNewsPaperScale.x += 7.0f;
                m_FoldNewsPaperScale.y += 7.0f;

                auto FoldNewsPaperRotate = m_FoldPaperSprite->getRotation();

                m_FoldPaperSprite->setRotation(FoldNewsPaperRotate.x, FoldNewsPaperRotate.y, FoldNewsPaperRotate.z + 5.0f);
                m_FoldPaperSprite->setScale(m_FoldNewsPaperScale.x, m_FoldNewsPaperScale.y, m_FoldNewsPaperScale.z);
            }
            else if (m_PostedAnimCount > 3.0f)
            {
                if (m_PlayerWin)
                m_ResultStates = ResultStates::WinAnim;
                else if (!m_PlayerWin)
                m_ResultStates = ResultStates::LoseAnim;
            }
            spritePipeLine->AddRenderObject(m_FoldPaperSprite.get());
            spritePipeLine->AddRenderObject(m_Fadeprite.get());
            break;
        case ResultStates::WinAnim:
            if (m_OpenAnimCount < kAnimSpace)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperWinTexture04");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace && m_OpenAnimCount < kAnimSpace * 2)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperWinTexture03");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace * 2 && m_OpenAnimCount < kAnimSpace * 3)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperWinTexture02");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace * 3 && m_OpenAnimCount < kAnimSpace * 4)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperWinTexture01");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace * 4)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperWinTexture");
            }

            if (m_OpenAnimCount > kAnimSpace * 4 && keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
            {
                m_ResultStates = ResultStates::WinResultAnim;
            }
            spritePipeLine->AddRenderObject(m_NewsPaperSprite.get());
            spritePipeLine->AddRenderObject(m_Fadeprite.get());
            break;
        case ResultStates::LoseAnim:
            if (m_OpenAnimCount < kAnimSpace)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperLoseTexture04");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace && m_OpenAnimCount < kAnimSpace * 2)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperLoseTexture03");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace * 2 && m_OpenAnimCount < kAnimSpace * 3)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperLoseTexture02");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace * 3 && m_OpenAnimCount < kAnimSpace * 4)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperLoseTexture01");
                m_OpenAnimCount += m_TimeManager->GetDeltaTime();
            }
            else if (m_OpenAnimCount > kAnimSpace * 4)
            {
                m_NewsPaperSprite->setTextureId(L"NewsPaperLoseTexture");
            }
            spritePipeLine->AddRenderObject(m_NewsPaperSprite.get());
            spritePipeLine->AddRenderObject(m_Fadeprite.get());
            break;
        case ResultStates::WinResultAnim:
            spritePipeLine->AddRenderObject(m_Fadeprite.get());
            switch (m_ResultAnimStates)
            {
            default:
                break;
            case ResultAnimState::Init:
                if (m_ResultAnimCount < kAnimSpace)
                {
                    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTextureAfter01");
                }
                else if (m_ResultAnimCount > kAnimSpace && m_ResultAnimCount < kAnimSpace * 2)
                {
                    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTextureAfter02");
                }
                else if (m_ResultAnimCount > kAnimSpace * 2 && m_ResultAnimCount < kAnimSpace * 3)
                {
                    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTextureAfter03");
                }
                else if (m_ResultAnimCount > kAnimSpace * 3 && m_ResultAnimCount < kAnimSpace * 4)
                {
                    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTextureAfter04");
                }
                else if (m_ResultAnimCount > kAnimSpace * 4 && m_ResultAnimCount < kAnimSpace * 5)
                {
                    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTextureAfter05");
                }
                else if (m_ResultAnimCount > kAnimSpace * 5 && m_ResultAnimCount < kAnimSpace * 6)
                {
                    m_NewsPaperSprite->setTextureId(L"NewsPaperWinTextureAfter06");
                }
                else if (m_ResultAnimCount > kAnimSpace * 6)
                {
                    m_ResultAnimCount = 0.0f;
                    m_ResultAnimStates = ResultAnimState::Turn;
                }
                m_ResultAnimCount += m_TimeManager->GetDeltaTime();
                spritePipeLine->AddRenderObject(m_NewsPaperSprite.get());
                spritePipeLine->AddRenderObject(m_NewsPaperResultSprite.get());
                break;
            case ResultAnimState::Turn:
                spritePipeLine->AddRenderObject(m_NewsPaperResultSprite.get());
                if (m_ResultAnimCount < 1.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:";

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 1.0f && m_ResultAnimCount < 2.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 2.0f && m_ResultAnimCount < 3.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:";

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 3.0f && m_ResultAnimCount < 4.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:" + to_wstring(BFMng->GetPlayerKillCount());

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 4.0f && m_ResultAnimCount < 5.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:" + to_wstring(BFMng->GetPlayerKillCount());
                    killedCount = L"倒された部隊数:";

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKilledTextPosY, 30, 45, killedCount.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 5.0f && m_ResultAnimCount < 6.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:" + to_wstring(BFMng->GetPlayerKillCount());
                    killedCount = L"倒された部隊数:" + to_wstring(BFMng->GetEnemyKillCount());

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKilledTextPosY, 30, 45, killedCount.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 6.0f && m_ResultAnimCount < 8.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:" + to_wstring(BFMng->GetPlayerKillCount());
                    killedCount = L"倒された部隊数:" + to_wstring(BFMng->GetEnemyKillCount());
                    lastScore = L"戦闘スコア:";

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKilledTextPosY, 30, 45, killedCount.c_str());
                    count = MakeSpriteString(count, kLastScoreTextPosX, kLastScoreTextPosY, 40, 60, lastScore.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 8.0f && m_ResultAnimCount < 10.0f)
                {
                    m_ResultAnimCount += m_TimeManager->GetDeltaTime();

                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:" + to_wstring(BFMng->GetPlayerKillCount());
                    killedCount = L"倒された部隊数:" + to_wstring(BFMng->GetEnemyKillCount());
                    lastScore = L"戦闘スコア:" + to_wstring(GetLastScoreValue());

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKilledTextPosY, 30, 45, killedCount.c_str());
                    count = MakeSpriteString(count, kLastScoreTextPosX, kLastScoreTextPosY, 40, 60, lastScore.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }
                }
                else if (m_ResultAnimCount > 10.0f)
                {
                    turn = L"経過ターン数:" + to_wstring(BFMng->GetTurnCount());
                    killCount = L"倒した部隊数:" + to_wstring(BFMng->GetPlayerKillCount());
                    killedCount = L"倒された部隊数:" + to_wstring(BFMng->GetEnemyKillCount());
                    lastScore = L"戦闘スコア:" + to_wstring(GetLastScoreValue());

                    count = MakeSpriteString(count, kResultTextPosX, kTurnTextPosY, 30, 45, turn.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKillTextPosY, 30, 45, killCount.c_str());
                    count = MakeSpriteString(count, kResultTextPosX, kKilledTextPosY, 30, 45, killedCount.c_str());
                    count = MakeSpriteString(count, kLastScoreTextPosX, kLastScoreTextPosY, 40, 60, lastScore.c_str());
                    count = MakeSpriteString(count, kPressSpaceTextPosX, kPressSpaceTextPosY, 30, 45, pressSpace.c_str());

                    for (int i = 0; i < count; i++)
                    {
                        wordPipeLine->AddRenderObject(m_WordSprites[i].get());
                    }

                    if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))
                    {
                        PostQuitMessage(0);
                    }
                }
                break;
            }
            
            break;
        default:
            break;
        }
    }



    return true;
}

void ResultUI::finishAction()
{
}

ResultStates ResultUI::GetResultState()
{
    return m_ResultStates;
}

int ResultUI::GetLastScoreValue()
{
    return 10000 - (BFMng->GetTurnCount() * 1000) + (BFMng->GetEnemyKillCount() * 1500) + (BFMng->GetPlayerKillCount() * 1000);
}

