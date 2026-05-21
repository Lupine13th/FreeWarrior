#include <MyAccessHub.h>
#include "TerrainComponent.h"

#include "FBXCharacterData.h"

void TerrainComponent::InitAction()
{
	FBXCharacterData* fbxChara = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
	fbxChara->SetGraphicsPipeLine(L"StaticFBX");	//アニメなしFBXモード
	fbxChara->AddCameraLabel(L"DefenderCamera");
	fbxChara->AddCameraLabel(L"AttackerCamera");

	fbxChara->GetMainFbx()->SetMeshUniqueFlag(true, true); //第二引数がDX12版では無意味に
	fbxChara->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool TerrainComponent::FrameAction()
{
	GetGameObject()->GetCharacterData()->GetPipeline()->AddRenderObject(GetGameObject()->GetCharacterData());
	return true;
}

void TerrainComponent::FinishAction()
{
}
