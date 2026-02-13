#pragma once
#include "GameObject.h"
#include "FBXCharacterData.h" 

class RiverTerrain :
    public GameComponent
{
private:
    FBXCharacterData* m_chData = nullptr;
public:

    // GameComponent を介して継承されました
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

