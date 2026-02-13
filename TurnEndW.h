#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "WordList.h"
#include "TurnEndUI.h"

#include <vector>
#include <unordered_map>

#include <memory>

class TurnEndW : public GameComponent
{
private:
    WordList wordList;
    const wchar_t* m_chEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_fontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    int m_spriteCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);

    vector<std::wstring> cstr;

    vector<const wchar_t*> str;

	vector<float> posx = { -170.0f, 80.0f, -120.0f };

	vector<float> posy = { 100.0f, -50.0f, -50.0f };

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    TurnEndUI* m_TurnEndUI = nullptr;
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

