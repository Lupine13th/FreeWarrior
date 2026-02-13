#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "MenuUI.h"
#include "BattleFieldManager.h"
#include "WordList.h"
#include "TimeManager.h"

#include <vector>
#include <unordered_map>

#include <memory>
class DamageHUDW: public GameComponent
{
private:
    WordList wordList;
    const wchar_t* m_chEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_fontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

    int m_spriteCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);

	int damagenum = 0;

    float animCount = 0.0f;

    float x = 50.0f;
    float y = -100.0f;

    float originx = 50.0f;
    float originy = -100.0f;

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

