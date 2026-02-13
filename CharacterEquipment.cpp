#include "CharacterEquipment.h"

void CharacterEquipment::initAction()
{
	m_chData = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
	m_chData->SetGraphicsPipeLine(L"StaticFBX");
	m_chData->AddCameraLabel(L"AttackerCamera");
	m_chData->AddCameraLabel(L"DefenderCamera");

	m_chData->GetMainFbx()->SetMeshUniqueFlag(true, true);
	m_chData->GetMainFbx()->SetTextureUniqueFlag(true);

	InInit(m_chData);
}

bool CharacterEquipment::frameAction()
{
	return InFrame();
}

void CharacterEquipment::finishAction()
{
	InFinish();
}

void RebelAssaltRifle::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/RebelAssaltRifle.fbx", L"RebelAssaltRifle");
	chData->setScale(1.0f, 1.0f, 1.0f);
	chData->setPosition(0.0f, 0.0f, 0.0f);
	chData->setRotation(0.0f, 0.0f, 0.0f);
}

bool RebelAssaltRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = getGameObject()->getCharacterData();
		SqData->GetPipeline()->AddRenderObject(SqData);
		return true;
	}
	else
	{
		return false;
	}
}

void RebelAssaltRifle::InFinish()
{
}

void CharacterEquipment::SetActive(bool active)
{
	m_IsAlive = active;
}

void ImperialAssaltRifle::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/ImperialAssaltRifle.fbx", L"ImperialAssaltRifle");
	chData->setScale(1.0f, 1.0f, 1.0f);
	chData->setPosition(0.0f, 0.0f, 0.0f);
	chData->setRotation(0.0f, 0.0f, 0.0f);
}

bool ImperialAssaltRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = getGameObject()->getCharacterData();
		SqData->GetPipeline()->AddRenderObject(SqData);
		return true;
	}
	else
	{
		return false;
	}
}

void ImperialAssaltRifle::InFinish()
{
}

void RebelMachinegun::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/RebelMachinegun.fbx", L"RebelMachinegun");
	chData->setScale(1.0f, 1.0f, 1.0f);
	chData->setPosition(0.0f, 0.0f, 0.0f);
	chData->setRotation(0.0f, 0.0f, 0.0f);
}

bool RebelMachinegun::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = getGameObject()->getCharacterData();
		SqData->GetPipeline()->AddRenderObject(SqData);
		return true;
	}
	else
	{
		return false;
	}
}

void RebelMachinegun::InFinish()
{
}

void RebelScoutRifle::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/RebelScoutRifle.fbx", L"RebelScoutRifle");
	chData->setScale(1.0f, 1.0f, 1.0f);
	chData->setPosition(0.0f, 0.0f, 0.0f);
	chData->setRotation(0.0f, 0.0f, 0.0f);
}

bool RebelScoutRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = getGameObject()->getCharacterData();
		SqData->GetPipeline()->AddRenderObject(SqData);
		return true;
	}
	else
	{
		return false;
	}
}

void RebelScoutRifle::InFinish()
{
}

void ImperialScoutRifle::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/ImperialScoutRifle.fbx", L"ImperialScoutRifle");
	chData->setScale(1.0f, 1.0f, 1.0f);
	chData->setPosition(0.0f, 0.0f, 0.0f);
	chData->setRotation(0.0f, 0.0f, 0.0f);
}

bool ImperialScoutRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = getGameObject()->getCharacterData();
		SqData->GetPipeline()->AddRenderObject(SqData);
		return true;
	}
	else
	{
		return false;
	}
}

void ImperialScoutRifle::InFinish()
{
}


void Backpack::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/backpackObject.fbx", L"backpackObject");
	chData->setScale(1.0f, 1.0f, 1.0f);
	chData->setPosition(0.0f, 0.0f, 0.0f);
	chData->setRotation(0.0f, 0.0f, 0.0f);
}

bool Backpack::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = getGameObject()->getCharacterData();
		SqData->GetPipeline()->AddRenderObject(SqData);
		return true;
	}
	else
	{
		return false;
	}
}

void Backpack::InFinish()
{
}

