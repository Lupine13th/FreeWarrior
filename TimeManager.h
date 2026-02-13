#pragma once
#include "GameObject.h"

#include <chrono>

using namespace std;

class TimeManager : public GameComponent
{
public:
	// GameComponent を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	float GetDeltaTime() const;
private:
	chrono::steady_clock::time_point lastTime = chrono::steady_clock::now();
	float deltaTime = 0.0f;
};

