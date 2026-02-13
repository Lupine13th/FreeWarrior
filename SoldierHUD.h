#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "WordList.h"

class SoldierHUD : public GameComponent
{
private:
    std::unique_ptr<SpriteCharacter> m_SoldierMeterSprite;
    std::unique_ptr<SpriteCharacter> m_MoraleMeterSprite;
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    //==========テキストの設定==========
    WordList m_WordList;

    const wchar_t* m_ChEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_FontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_WordSprites;

    int m_WordCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);

    int MakeSpriteStringRightAligned(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);
    //==========テキストの設定==========
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

