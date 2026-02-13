#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "TimeManager.h"
#include "MyAccessHub.h"

#include <vector>
#include <chrono>

class MonitorSelectUI : public GameComponent
{
private:
    TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

    vector<std::unique_ptr<SpriteCharacter>> m_sprite;

    bool Animation = false;
    bool CloseAnimation = false;

    bool firstAnim = false;

    float AnimCount = 0.0f;

    float firstAnimCount = 0.0f;

    float closeAnimCount = 0.0f;

    float MenuBaseposx = -480.0f;

    float NixieBaseposx = -350.0f;

    float Gageposx = -700.0f;

    float persent = 0.0f;

public:
    int selectIndex = 0;

    void ResetAnim();

    void OpenAnim();
    void CloseAnim();

    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

