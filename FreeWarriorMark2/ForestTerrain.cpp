#include "ForestTerrain.h"
#include "FBXCharacterData.h" 

void ForestTerrain::InitAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX"); 
    chdata->LoadMainFBX(L"./Resources/fbx/ForestTerrain.fbx", L"ForestTer");
    chdata->SetScale(0.05f, 0.05f, 0.05f);

    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool ForestTerrain::FrameAction()
{
    CharacterData* SqData = GetGameObject()->GetCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);
    return true;
}

void ForestTerrain::FinishAction()
{
}
