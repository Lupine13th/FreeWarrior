#include "RiverTerrain.h"
#include "FBXCharacterData.h" 

void RiverTerrain::initAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX");
    chdata->LoadMainFBX(L"./Resources/fbx/RiverTerrain.fbx", L"RiverTer");
    chdata->setScale(3.0f, 12.0f, 3.0f);

    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool RiverTerrain::frameAction()
{
    CharacterData* SqData = getGameObject()->getCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);
    return true;
}

void RiverTerrain::finishAction()
{
}
