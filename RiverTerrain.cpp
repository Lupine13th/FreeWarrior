#include "RiverTerrain.h"
#include "FBXCharacterData.h" 

void RiverTerrain::InitAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX");
    chdata->LoadMainFBX(L"./Resources/fbx/RiverTerrain.fbx", L"RiverTer");
    chdata->setScale(3.0f, 12.0f, 3.0f);

    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool RiverTerrain::FrameAction()
{
    CharacterData* SqData = GetGameObject()->GetCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);
    return true;
}

void RiverTerrain::FinishAction()
{
}
