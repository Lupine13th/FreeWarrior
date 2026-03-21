#pragma once
#include "GameObject.h"
#include "FBXCharacterData.h" 

class TowerTerrain :
    public GameComponent
{
private:
    FBXCharacterData* m_chData = nullptr;
public:

    // GameComponent を介して継承されました
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};

