#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "FieldCharacter.h"
#include "Squares.h"
#include "BattleFieldManager.h"
#include "ReadyCharacterHUD.h"
#include "ReadyCharacterHUDW.h"
#include "InFieldCharacterMenu.h"
#include "InFieldCharacterMenuWord.h"
#include "TimeManager.h"

#include "EnumClassList.h"

#include <memory>

class ReadyCharacterHUD;
class ReadyCharacterHUDW;
class InFieldCharacterMenu;
class InFieldCharacterMenuWord;

enum class ReadySceneState
{
    Init,
    Menu,
    InField,
    Loading,
    Finish
};

class BattleReadyScene : public GameComponent
{
private:
    std::unique_ptr<SpriteCharacter> m_Sprite;

    GAME_SCENES m_nextScene;

    int sta;

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();
public:
    vector<Platoon*> InFieldAlliesCharacterList = { nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr };

    vector<Platoon*> RDAlliesCharacterList = { nullptr ,nullptr ,nullptr ,nullptr ,nullptr };
    vector<Platoon*> RDEnemyCharacterList = { nullptr ,nullptr ,nullptr ,nullptr ,nullptr };

    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;

    ReadyCharacterHUD* rdcharacterHUD;
    ReadyCharacterHUDW* rdcharacterHUDW;
    InFieldCharacterMenu* infieldcharacterMenu;
    InFieldCharacterMenuWord* infieldcharacterMenuW;

    void SetNextScene(GAME_SCENES nextSc);

    void SetCharacterData(Platoon* readyChara, Platoon* nextChara, Squares* square, int index, int squareIndex);

    void ResetCount();
    int menuIndex = 0;
    int m_InFieldCharacterMenuIndex = 0;
	bool ready = false;
    ReadySceneState m_ReadySceneState = ReadySceneState::Init;
    bool lighting = false;
    float lightingcount = 0.0f;

    XMFLOAT3 textcolor = {1.0f, 1.0f, 0.7f};

    float m_InitCount = 0.0f;
    const float kMaxInitCount = 5.0f;
    float m_LoadCount = 0.0f;
    const float kMaxLoadCount = 3.0f;
    float m_FinishCount = 0.0f;
    const float kMaxFinishCount = 1.5f;
};

