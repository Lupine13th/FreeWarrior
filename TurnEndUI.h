#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "TimeManager.h"
#include "MyAccessHub.h"

class TurnEndUI : public GameComponent
{
private:
    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();
    vector<std::unique_ptr<SpriteCharacter>> m_sprite;
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    float m_WaitCount = 0.0f;
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void ResetWaitCount();
    float GetCurrentCount();
};

