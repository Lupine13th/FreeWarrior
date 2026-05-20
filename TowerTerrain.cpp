#include "TowerTerrain.h"
#include "FBXCharacterData.h" 

void TowerTerrain::InitAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX");
    chdata->LoadMainFBX(L"./Resources/fbx/TowerTerrain.fbx", L"TowerTer");
    //chdata->setPosition(0.0f, 0.0f, 0.0f); 
    chdata->SetScale(0.01f, 0.01f, 0.01f);

    //=======Change Scene
    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
    //=======Change Scene End
}

bool TowerTerrain::FrameAction()
{
    CharacterData* SqData = GetGameObject()->GetCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);
    return true;
}

void TowerTerrain::FinishAction()
{
}
