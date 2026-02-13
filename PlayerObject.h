#pragma once
#include "SpriteCharacter.h"
#include "GameObject.h"


class PlayerObject : public GameObject
{
public:
	PlayerObject();

	void init() override;
};
