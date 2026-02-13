#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleReadyScene.h"


class InFieldCharacterMenuImage : public GameComponent
{
private:
    std::unique_ptr<SpriteCharacter> m_Sprite;
    BattleReadyScene* BRScene = MyAccessHub::GetBRScene();
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

