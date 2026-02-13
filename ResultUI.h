#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "MyAccessHub.h"
#include "BattleReadyScene.h"
#include "TimeManager.h"
#include "WordList.h"

#include <vector>
#include <unordered_map>

#include <memory>

enum class ResultStates
{
    Init,
    PostedAnim,
    WinAnim,
    LoseAnim,
    WinResultAnim
};

enum class ResultAnimState
{
    Init,
    Turn,
    Kill,
    Score
};

class ResultUI:public GameComponent
{
private:
    std::unique_ptr<SpriteCharacter> m_FoldPaperSprite; //折った新聞
    std::unique_ptr<SpriteCharacter> m_Fadeprite;       //暗転するフェード
    std::unique_ptr<SpriteCharacter> m_NewsPaperSprite;       //新聞の表面。勝利時と敗北時でテクスチャを切り替える
    std::unique_ptr<SpriteCharacter> m_NewsPaperResultSprite;       //新聞の内面。勝利時のみ表示され、結果が描画されていく

    //==========テキストの設定==========
    WordList m_WordList;

    const wchar_t* m_ChEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_FontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_WordSprites;

    int m_WordCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);
    //==========テキストの設定==========

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

    ResultStates m_ResultStates = ResultStates::Init;
    ResultAnimState m_ResultAnimStates = ResultAnimState::Init;

    bool m_ResultOn = false;

    bool m_PlayerWin = false;

    float kAnimSpace = 0.1f;

    float m_PostedAnimCount = 0.0f;     //投函のアニメカウント

    float m_OpenAnimCount = 0.0f;     //新聞を開くアニメカウント

    float m_ResultAnimCount = 0.0f;     //新聞をめくる＆結果画面のアニメカウント

    float m_FadeAlpha = 0.0f;     //フェードアウト用float

    const float kResultTextPosX = -200.0f;  //リザルトのテキストのX座標

    const float kLastScoreTextPosX = -200.0f;  //リザルトのテキストのX座標

    const float kPressSpaceTextPosX = -50.0f;  //PressSpaceテキストのX座標

    const float kTurnTextPosY = 50.0f;

    const float kKillTextPosY = 0.0f;

    const float kKilledTextPosY = -50.0f;

    const float kLastScoreTextPosY = -125.0f;

    const float kPressSpaceTextPosY = -200.0f;


    XMFLOAT3 m_FoldNewsPaperPos = { 0.0f, 900.0f, 1.0f };
    XMFLOAT3 m_FoldNewsPaperScale = { 200.0f, 200.0f, 0.1f };

    XMFLOAT3 m_FoldNewsPaperLastPos = { 0.0f, 0.0f, 1.0f };
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    ResultStates GetResultState();

    int GetLastScoreValue();
};

