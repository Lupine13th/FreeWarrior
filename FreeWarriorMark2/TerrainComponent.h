#pragma once

#include "GameObject.h"

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class TerrainComponent : public GameComponent
{
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};
