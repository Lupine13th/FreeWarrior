#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "BattleReadyScene.h"
#include "WordList.h"

#include <vector>
#include <unordered_map>

#include <memory>

class ReadyCharacterHUDW : public GameComponent
{
private:
	WordList wordList;
    //const char* m_chList = "0123456789+-*/!?ABCDEFGHIJKLMNOPQRSTUVWXYZ#$%&<>abcdefghijklmnopqrstuvwxyz\"()[]|";
    const wchar_t* m_chEnd = nullptr;

    std::unordered_map<wchar_t, XMFLOAT4> m_fontMap;
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    int m_spriteCount;

    int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str, int makingSpriteCount);

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    BattleReadyScene* BRScene = MyAccessHub::GetBRScene();

    //味方部隊の上の看板
    std::wstring allies = L"反乱軍(プレイヤー)";

    //敵部隊の上の看板
    std::wstring enemies = L"帝国軍";

    //味方部隊
    vector<std::wstring> ally;

    //敵部隊
    vector<std::wstring> enemy;
    
    vector<const wchar_t*> strs;

	//0:左端 1:敵部隊
    vector<float> xpos = { -400.0f, 0.0f};

	//0:部隊選出命令 1:看板 2:部隊1 3:部隊2 4:部隊3 5:部隊4 6:部隊5 7;ready
	vector<float> ypos = { 250.0f, 180.0f, 120.0f, 60.0f, 0.0f ,-60.0f, -120.0f, -190.0f};

    float AnimCount = 0.0f;

    vector<int> CountList;

    bool AnimationFlag = true;

public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void ResetAnimation();

    
};

