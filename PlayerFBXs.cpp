#include "PlayerFBXs.h"

void InfantryPlayer::initAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());

	float scaleValue = 0.03f;

	chdata->SetGraphicsPipeLine(L"AnimationFBX");
	chdata->AddCameraLabel(L"AttackerCamera");
	chdata->AddCameraLabel(L"DefenderCamera");
	chdata->AddCameraLabel(L"ScoutingCamera");
	chdata->SetScaleValue(scaleValue);

	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelInfantry.fbx", L"RebelInfantry");
		chdata->setScale(scaleValue, scaleValue, scaleValue);

		//･｢･ﾋ･皈ﾇ｡ｼ･ｿ､ﾎﾅﾐﾏｿ
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Idle.fbx", L"WAIT01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Fire.fbx", L"ATTACK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Run.fbx", L"WALK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Dying.fbx", L"DYING01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Damage.fbx", L"DAMAGE01");
		chdata->SetAnime(L"WAIT01");

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);
		chdata->GetAnimeFbx(L"WAIT01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"ATTACK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"WALK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"DYING01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"DAMAGE01")->SetMeshUniqueFlag(true, true);
	}
	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialInfantry.fbx", L"ImperialInfantry");
		chdata->setScale(0.03f, 0.03f, 0.03f);

		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Idle.fbx", L"WAIT01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Fire.fbx", L"ATTACK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Run.fbx", L"WALK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Dying.fbx", L"DYING01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Damage.fbx", L"DAMAGE01");
		chdata->SetAnime(L"WAIT01");

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);
		chdata->GetAnimeFbx(L"WAIT01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"ATTACK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"WALK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"DYING01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"DAMAGE01")->SetMeshUniqueFlag(true, true);
	}

}

bool InfantryPlayer::frameAction()
{
	if (IsAlive)
	{
		m_chData = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
		m_chData->UpdateAnimation();

		if (m_RightEquipment->GetCharacterData() == nullptr) return false;

		XMMATRIX rightHandMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			rightHandMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:RightHand");
			break;
		case Admin::Imperial:
			rightHandMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:RightHand");
			break;
		}

		if (m_RightEquipment != nullptr)
		{
			switch (m_admin)
			{
			case Admin::Rebel:
				SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -90.0f, -90.0f), XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			case Admin::Imperial:
				SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -100.0f, -90.0f), XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			}
		}

		m_chData->GetPipeline()->AddRenderObject(m_chData);
		return true;
	}
	else
	{
		return false;
	}
}

void InfantryPlayer::finishAction()
{

}


void ArtilleryPlayer::initAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
	chdata->SetGraphicsPipeLine(L"StaticFBX");
	chdata->AddCameraLabel(L"AttackerCamera");
	chdata->AddCameraLabel(L"DefenderCamera");
	chdata->AddCameraLabel(L"ScoutingCamera");
	

	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelCanon.fbx", L"RebelCanon");
	}

	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialCanon.fbx", L"ImperialCanon");
	}

	float scaleValue = 0.02f;
	chdata->setScale(scaleValue, scaleValue, scaleValue);
	chdata->SetScaleValue(scaleValue);

	chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
	chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool ArtilleryPlayer::frameAction()
{
	if (IsAlive)
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

void ArtilleryPlayer::finishAction()
{
}




void MachinegunnerPlayer::initAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
	chdata->SetGraphicsPipeLine(L"AnimationFBX");
	chdata->AddCameraLabel(L"AttackerCamera");
	chdata->AddCameraLabel(L"DefenderCamera");
	chdata->AddCameraLabel(L"ScoutingCamera");

	chdata->LoadMainFBX(L"./Resources/fbx/rebelInfantry.fbx", L"RebelInfantry");
	float scaleValue = 0.03f;
	chdata->setScale(scaleValue, scaleValue, scaleValue);
	chdata->SetScaleValue(scaleValue);

	//･｢･ﾋ･皈ﾇ｡ｼ･ｿ､ﾎﾅﾐﾏｿ
	chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Machinegun_Idle.fbx", L"WAIT01");
	chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Machinegun_Fire.fbx", L"ATTACK01");
	chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Run.fbx", L"WALK01");
	chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Dying.fbx", L"DYING01");
	chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Damage.fbx", L"DAMAGE01");
	chdata->SetAnime(L"WAIT01"); //ｺﾆﾀｸｳｫｻﾏ

	chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
	chdata->GetMainFbx()->SetTextureUniqueFlag(true);
	chdata->GetAnimeFbx(L"WAIT01")->SetMeshUniqueFlag(true, true);
	chdata->GetAnimeFbx(L"ATTACK01")->SetMeshUniqueFlag(true, true);
	chdata->GetAnimeFbx(L"WALK01")->SetMeshUniqueFlag(true, true);
	chdata->GetAnimeFbx(L"DYING01")->SetMeshUniqueFlag(true, true);
	chdata->GetAnimeFbx(L"DAMAGE01")->SetMeshUniqueFlag(true, true);
}

bool MachinegunnerPlayer::frameAction()
{
	if (IsAlive)
	{
		m_chData = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
		m_chData->UpdateAnimation();

		if (m_RightEquipment->GetCharacterData() == nullptr) return false;

		XMMATRIX rightHandMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			rightHandMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:RightHand");
			break;
		case Admin::Imperial:
			rightHandMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:RightHand");
			break;
		}

		XMMATRIX spineMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			spineMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:Spine");
			break;
		case Admin::Imperial:
			spineMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:Spine");
			break;
		}

		if (m_RightEquipment != nullptr)
		{
			switch (m_admin)
			{
			case Admin::Rebel:
				SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -75.0f, -100.0f), XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			case Admin::Imperial:
				//SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -100.0f, -90.0f), XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			}
		}
		if (m_BackEquipment != nullptr)
		{
			switch (m_admin)
			{
			case Admin::Rebel:
				SetMatrixForEquipment(m_BackEquipment, spineMatrix, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -40.0f, 10.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			case Admin::Imperial:
				//SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -100.0f, -90.0f), XMFLOAT3(5.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			}
		}

		m_chData->GetPipeline()->AddRenderObject(m_chData);
		return true;
	}
	else
	{
		return false;
	}
}

void MachinegunnerPlayer::finishAction()
{
}




void ScoutPlayer::initAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());

	chdata->SetGraphicsPipeLine(L"AnimationFBX");
	chdata->AddCameraLabel(L"AttackerCamera");
	chdata->AddCameraLabel(L"DefenderCamera");
	chdata->AddCameraLabel(L"ScoutingCamera");

	float scaleValue = 0.03f;

	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelScout.fbx", L"RebelSco");
		
		chdata->setScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);

		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Idle.fbx", L"WAIT01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Fire.fbx", L"ATTACK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Run.fbx", L"WALK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Dying.fbx", L"DYING01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Damage.fbx", L"DAMAGE01");
		chdata->SetAnime(L"WAIT01");

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);
		chdata->GetAnimeFbx(L"WAIT01")->SetMeshUniqueFlag(true, true); 
		chdata->GetAnimeFbx(L"ATTACK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"WALK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"DYING01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"DAMAGE01")->SetMeshUniqueFlag(true, true);
	}

	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialSco.fbx", L"ImperialSco");

		chdata->setScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);

		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Idle.fbx", L"WAIT01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Fire.fbx", L"ATTACK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Run.fbx", L"WALK01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Dying.fbx", L"DYING01");
		chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Damage.fbx", L"DAMAGE01");
		chdata->SetAnime(L"WAIT01");

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);
		chdata->GetAnimeFbx(L"WAIT01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"ATTACK01")->SetMeshUniqueFlag(true, true);
		chdata->GetAnimeFbx(L"WALK01")->SetMeshUniqueFlag(true, true);
	}
}

bool ScoutPlayer::frameAction()
{
	if (IsAlive)
	{
		m_chData = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
		m_chData->UpdateAnimation();

		if (m_RightEquipment == nullptr) return false;
		if (m_RightEquipment->GetCharacterData() == nullptr) return false;

		XMMATRIX rightHandMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			rightHandMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:RightHand");
			break;
		case Admin::Imperial:
			rightHandMatrix = m_chData->GetMainFbx()->GetBornMaxrix("mixamorig:RightHand");
			break;
		}

		if (m_RightEquipment != nullptr)
		{
			switch (m_admin)
			{
			case Admin::Rebel:
				SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -80.0f, -90.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			case Admin::Imperial:
				SetMatrixForEquipment(m_RightEquipment, rightHandMatrix, XMFLOAT3(0.0f, -80.0f, -90.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
				break;
			}
		}

		m_chData->GetPipeline()->AddRenderObject(m_chData);
		return true;
	}
	else
	{
		return false;
	}
}

void ScoutPlayer::finishAction()
{
}




void ArmoredPlayer::initAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
	chdata->SetGraphicsPipeLine(L"StaticFBX");
	chdata->AddCameraLabel(L"AttackerCamera");
	chdata->AddCameraLabel(L"DefenderCamera");
	chdata->AddCameraLabel(L"ScoutingCamera");

	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelarm.fbx", L"RebelArm");
		float scaleValue = 3.00f;
		chdata->setScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);
	}
	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialarm.fbx", L"ImperialArm");
		float scaleValue = 0.03f;
		chdata->setScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);
	}
	chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
	chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool ArmoredPlayer::frameAction()
{
	if (IsAlive)
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

void ArmoredPlayer::finishAction()
{
}

void PlayerBase::SetHandEquipment(CharacterEquipment* equipment)
{
	m_RightEquipment = equipment;
}

void PlayerBase::SetBackEquipment(CharacterEquipment* equipment)
{
	m_BackEquipment = equipment;
}

void PlayerBase::SetMatrixForEquipment(CharacterEquipment* equipment, XMMATRIX matrix, XMFLOAT3 rotate, XMFLOAT3 position, XMFLOAT3 scale)
{
	XMMATRIX gripRotate = XMMatrixRotationRollPitchYaw
	(
		XMConvertToRadians(rotate.x),
		XMConvertToRadians(rotate.y),
		XMConvertToRadians(rotate.z)
	);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX gripTranslate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX gripMatrix = gripRotate * gripTranslate;
	XMMATRIX gripInverse = XMMatrixInverse(nullptr, gripMatrix);
	XMMATRIX charWorldMatrix = m_chData->GetWorldMatrix();
	XMMATRIX finalWeaponMatrix = scaleMatrix * gripInverse * matrix * charWorldMatrix;

	equipment->GetCharacterData()->SetMatrixAutoUpdate(false);
	equipment->GetCharacterData()->SetWorldMatrix(finalWeaponMatrix);
}
