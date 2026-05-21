#pragma once
#include <GameObject.h>
#include "FBXCharacterData.h"


class SkyDomeComponent : public GameComponent
{
private:
	CharacterData* centerCharacter;

public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void setCenterCharacter(CharacterData* target);
};
