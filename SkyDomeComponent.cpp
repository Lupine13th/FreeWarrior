#include <MyAccessHub.h>

#include "SkyDomeComponent.h"
#include "SceneManager.h"

void SkyDomeComponent::initAction()
{
	CharacterData* chdata = getGameObject()->getCharacterData();
	chdata->setScale(10.0f, 10.0f, 10.0f);

	chdata->SetGraphicsPipeLine(L"StaticFBX");
	chdata->AddCameraLabel(L"DefenderCamera");
	chdata->AddCameraLabel(L"AttackerCamera");
	chdata->AddCameraLabel(L"ScoutingCamera");

	FBXCharacterData* fbxChara = static_cast<FBXCharacterData*>(chdata);
	fbxChara->GetMainFbx()->SetMeshUniqueFlag(true, true);
	fbxChara->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool SkyDomeComponent::frameAction()
{
	CharacterData* myData = getGameObject()->getCharacterData();

	if (centerCharacter != nullptr)
	{
		XMFLOAT3 charaPos = centerCharacter->getPosition();

		myData->setPosition(charaPos.x, charaPos.y, charaPos.z);
	}

	myData->GetPipeline()->AddRenderObject(myData);
	return true;
}

void SkyDomeComponent::finishAction()
{
}

void SkyDomeComponent::setCenterCharacter(CharacterData* target)
{
	centerCharacter = target;
}
