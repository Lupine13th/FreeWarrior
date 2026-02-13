#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "MenuUI.h"
#include "WordList.h"
#include "BattleFieldManager.h"

#include <vector>
#include <unordered_map>

#include <memory>

class LogHUDW : public GameComponent
{
private:
	WordList wordList;

    //const char* m_chList = "0123456789+-*/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ#$%&<>abcdefghijklmnopqrstuvwxyz\"()[]|";
    const wchar_t* m_chEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_fontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    int m_spriteCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
public:
	int LogCount = 0;

    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    std::wstring ctrs[8] = 
    {
        L"",
        L"",
        L"",
        L"",
        L"",
        L"",
        L"",
        L"",
    };

    const wchar_t* strs[8] = 
    {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
    };

    float posx = -475.0f;
    float posy[8] = 
    {
		-90.0f,
        -115.0f,
		-140.0f,
		-165.0f,
		-190.0f,
		-215.0f,
		-240.0f,
		-265.0f
    };
};

