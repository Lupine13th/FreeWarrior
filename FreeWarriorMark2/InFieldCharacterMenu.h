#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleReadyScene.h"
#include <MyAccessHub.h>

class InFieldCharacterMenu : public GameComponent
{
private:
    std::vector<std::unique_ptr<SpriteCharacter>> m_Sprite;
    BattleReadyScene* BRScene = MyAccessHub::GetBRScene();
public:
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};

