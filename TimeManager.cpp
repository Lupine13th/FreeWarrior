#include "TimeManager.h"

#include <iostream>
#include <algorithm>

using namespace std;

void TimeManager::initAction()
{
}

bool TimeManager::frameAction()
{
    auto now = std::chrono::steady_clock::now();
    float rawDelta = std::chrono::duration<float>(now - lastTime).count();

    // 一定時間以上の停止を検出（例: 0.5秒以上）
    if (rawDelta > 0.5f) 
    {
        // デバッガ停止やポーズ中などの場合、deltaTimeを0にする
        deltaTime = 0.016f;
    }
    else 
    {
        deltaTime = (std::max)(0.0f, (std::min)(rawDelta, 0.033f));
    }

    lastTime = now;
    return true;
}

void TimeManager::finishAction()
{
}

float TimeManager::GetDeltaTime() const
{
    return deltaTime;
}
