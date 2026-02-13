#include "HillTerrain.h"
#include "FBXCharacterData.h" 

void HillTerrain::initAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
    chdata->SetGraphicsPipeLine(L"StaticFBX");
    chdata->LoadMainFBX(L"./Resources/fbx/HillTerrain.fbx", L"HillsTer");
    chdata->setScale(0.001f, 0.001f, 0.001f);

    chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
    chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool HillTerrain::frameAction()
{

    CharacterData* SqData = getGameObject()->getCharacterData();
    SqData->GetPipeline()->AddRenderObject(SqData);

    return true;
}

void HillTerrain::finishAction()
{
}
