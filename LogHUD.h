#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"

#include <vector>
#include <unordered_map>

#include <memory>

class LogHUD : public GameComponent
{
private:
    //std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
public:
	//int LogCount = 0;   
    std::vector<std::unique_ptr<SpriteCharacter>> m_sprites;

    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

