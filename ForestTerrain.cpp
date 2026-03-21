#include "ForestTerrain.h"
#include "FBXCharacterData.h" 

void ForestTerrain::InitAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX"); 
    chdata->LoadMainFBX(L"./Resources/fbx/ForestTerrain.fbx", L"ForestTer");
    chdata->setScale(0.05f, 0.05f, 0.05f);

    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool ForestTerrain::FrameAction()
{
    CharacterData* SqData = getGameObject()->getCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);
    return true;
}

void ForestTerrain::FinishAction()
{
}
