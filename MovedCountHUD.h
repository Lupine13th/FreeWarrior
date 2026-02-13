#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "TimeManager.h"

class MovedCountHUD : public GameComponent
{
private:
    vector<std::unique_ptr<SpriteCharacter>> m_Sprite = {};
    std::unique_ptr<SpriteCharacter> m_BaseSprite = {};
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

    const float kDogtagInterbal = 25.0f;
    const float kDogtagLeftPosX = -125.0f;
    const float kDogtagClosingPosY = 300.0f;

    float m_DogtagPosY = 0.0f;
    float m_DogtagAnimCount = 0.0f;
    float m_DogtagClosingCount = 0.0f;

    vector<bool> m_ActiveDogtagList = {};

    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void ResetAnimCount();
    void ResetMovedCountUI();
};

