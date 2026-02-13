#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "MyAccessHub.h"
#include "BattleReadyScene.h"

#include <vector>
#include <unordered_map>

#include <memory>

class ReadyCharacterHUD : public GameComponent
{
private:
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprite;

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	BattleReadyScene* BRScene = MyAccessHub::GetBRScene();
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

