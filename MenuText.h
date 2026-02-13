#pragma once
#include "GameObject.h"
#include "SpriteCharacter.h"
#include "MenuUI.h"
#include "BattleFieldManager.h"
#include "WordList.h"
#include "TimeManager.h"

#include <vector>
#include <unordered_map>
#include <chrono>

#include <memory>

class MenuText :
    public GameComponent
{
private:
    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();
	WordList wordList;
    const wchar_t* m_chEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_fontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    const vector<const wchar_t*> kStringList =
    {
        L"攻撃",
        L"移動",
        L"行動",
        L"待機",
        L"キャンセル",
        L">",
    };

    const vector<float> kPositionY =
    {
        185.0f,
        105.0f,
        25.0f,
        -55.0f,
        -135.0f,
    };

    const float kTextPositionX = -410.0f;
    const float kArrowPositionX = -450.0f;

    int m_spriteCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);

    MenuUI* moniUI;

    float TextCount = 0.0f;

    bool m_IsEnable = false;

public:
    // GameComponent を介して継承されました
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    void OpenMenuText();

    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

