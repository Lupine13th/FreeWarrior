#include "CharacterEquipment.h"

void CharacterEquipment::InitAction()
{
	m_chData = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
	m_chData->SetGraphicsPipeLine(L"StaticFBX");
	m_chData->AddCameraLabel(L"AttackerCamera");
	m_chData->AddCameraLabel(L"DefenderCamera");
	m_chData->AddCameraLabel(L"AttackerCameraForHUD");
	m_chData->AddCameraLabel(L"DefenderCameraForHUD");

	m_chData->GetMainFbx()->SetMeshUniqueFlag(true, true);
	m_chData->GetMainFbx()->SetTextureUniqueFlag(true);

	InInit(m_chData);
}

bool CharacterEquipment::FrameAction()
{
	return InFrame();
}

void CharacterEquipment::FinishAction()
{
	InFinish();
}

void RebelAssaltRifle::InInit(FBXCharacterData* chData)
{
	chData->LoadMainFBX(L"./Resources/fbx/RebelAssaltRifle.fbx", L"RebelAssaltRifle");
	chData->SetScale(1.0f, 1.0f, 1.0f);
	chData->SetPosition(0.0f, 0.0f, 0.0f);
	chData->SetRotation(0.0f, 0.0f, 0.0f);
}

bool RebelAssaltRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = GetGameObject()->GetCharacterData();
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
	chData->SetScale(1.0f, 1.0f, 1.0f);
	chData->SetPosition(0.0f, 0.0f, 0.0f);
	chData->SetRotation(0.0f, 0.0f, 0.0f);
}

bool ImperialAssaltRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = GetGameObject()->GetCharacterData();
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
	chData->SetScale(1.0f, 1.0f, 1.0f);
	chData->SetPosition(0.0f, 0.0f, 0.0f);
	chData->SetRotation(0.0f, 0.0f, 0.0f);
}

bool RebelMachinegun::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = GetGameObject()->GetCharacterData();
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
	chData->SetScale(1.0f, 1.0f, 1.0f);
	chData->SetPosition(0.0f, 0.0f, 0.0f);
	chData->SetRotation(0.0f, 0.0f, 0.0f);
}

bool RebelScoutRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = GetGameObject()->GetCharacterData();
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
	chData->SetScale(1.0f, 1.0f, 1.0f);
	chData->SetPosition(0.0f, 0.0f, 0.0f);
	chData->SetRotation(0.0f, 0.0f, 0.0f);
}

bool ImperialScoutRifle::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = GetGameObject()->GetCharacterData();
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
	chData->SetScale(1.0f, 1.0f, 1.0f);
	chData->SetPosition(0.0f, 0.0f, 0.0f);
	chData->SetRotation(0.0f, 0.0f, 0.0f);
}

bool Backpack::InFrame()
{
	if (m_IsAlive)
	{
		CharacterData* SqData = GetGameObject()->GetCharacterData();
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

