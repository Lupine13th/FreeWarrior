#pragma once
#include <HitShapes.h>

#include "GameObject.h"

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class TerrainComponent : public GameComponent
{
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};
