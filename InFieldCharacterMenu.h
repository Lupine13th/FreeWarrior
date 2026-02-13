#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleReadyScene.h"
#include <MyAccessHub.h>

class InFieldCharacterMenu : public GameComponent
{
private:
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprite;
    BattleReadyScene* BRScene = MyAccessHub::GetBRScene();
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

