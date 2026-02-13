#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleReadyScene.h"
#include "WordList.h"

#include <vector>
#include <unordered_map>

#include <memory>
class InFieldCharacterMenuWord : public GameComponent
{
private:
    WordList wordList;

    //const char* m_chList = "0123456789+-*/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ#$%&<>abcdefghijklmnopqrstuvwxyz\"()[]|";
    const wchar_t* m_chEnd;

    std::unordered_map<wchar_t, XMFLOAT4> m_fontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    int m_spriteCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str, int makingSpriteCount);

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    BattleReadyScene* BRScene = MyAccessHub::GetBRScene();

    //味方部隊
    vector<std::wstring> m_InFieldMenuTextList;

    vector<const wchar_t*> strs;

    vector<float> xpos = { -415.0f, 150.0f, 300.0f};

    vector<float> ypos = { 180.0f, 140.0f, 100.0f, 60.0f, 20.0f ,-20.0f ,-60.0f ,-100.0f ,-140.0f ,-180.0f };

    float AnimCount = 0.0f;

    vector<int> CountList;

    bool AnimationFlag = true;
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void ResetAnimation();
};

