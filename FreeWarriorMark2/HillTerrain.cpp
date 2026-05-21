#include "HillTerrain.h"
#include "FBXCharacterData.h" 

void HillTerrain::InitAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX");
    chdata->LoadMainFBX(L"./Resources/fbx/HillTerrain.fbx", L"HillsTer");
    chdata->SetScale(0.001f, 0.001f, 0.001f);

    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool HillTerrain::FrameAction()
{
    CharacterData* SqData = GetGameObject()->GetCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);

    return true;
}

void HillTerrain::FinishAction()
{
}
