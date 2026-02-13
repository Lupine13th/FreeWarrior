#include "TowerTerrain.h"
#include "FBXCharacterData.h" 

void TowerTerrain::initAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX");
    chdata->LoadMainFBX(L"./Resources/fbx/TowerTerrain.fbx", L"TowerTer");
    //chdata->setPosition(0.0f, 0.0f, 0.0f); 
    chdata->setScale(0.01f, 0.01f, 0.01f);

    //=======Change Scene
    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
    //=======Change Scene End
}

bool TowerTerrain::frameAction()
{
    CharacterData* SqData = getGameObject()->getCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);
    return true;
}

void TowerTerrain::finishAction()
{
}
