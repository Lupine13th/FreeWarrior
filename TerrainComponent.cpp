#include <MyAccessHub.h>
#include <HitManager.h>
#include "TerrainComponent.h"

#include "FBXCharacterData.h"

void TerrainComponent::initAction()
{
	FBXCharacterData* fbxChara = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
	fbxChara->SetGraphicsPipeLine(L"StaticFBX");	//アニメなしFBXモード
	fbxChara->AddCameraLabel(L"DefenderCamera");
	fbxChara->AddCameraLabel(L"AttackerCamera");

	fbxChara->GetMainFbx()->SetMeshUniqueFlag(true, true); //第二引数がDX12版では無意味に
	fbxChara->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool TerrainComponent::frameAction()
{
	getGameObject()->getCharacterData()->GetPipeline()->AddRenderObject(getGameObject()->getCharacterData());
	return true;
}

void TerrainComponent::finishAction()
{
}
