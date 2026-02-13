#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "TimeManager.h"

enum class HUDState
{
    None,
    Allies,
    AlliesToEnemy,
    Enemy,
	EnemyToAllies
};

class TurnUI : public GameComponent
{
private:
    std::unique_ptr<SpriteCharacter> m_Sprite;

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

	HUDState m_HUDState = HUDState::None;

    float m_AnimCount = 0.0f;
    const float kAnimInterval = 0.03f;
public:
    bool TurnUIenable = true;

    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void ChangeHUDState();
};

