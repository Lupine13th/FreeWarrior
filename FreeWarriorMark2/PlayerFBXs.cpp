#include "PlayerFBXs.h"

void InfantryPlayer::InitAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());

	float scaleValue = 0.03f;

	SetCharacterCameraLabel(chdata, true);

	chdata->SetScaleValue(scaleValue);

	if (m_admin == Admin::Rebel)	//歩兵Fbxの登録　勢力ごとに違うからif分で分岐
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelInfantry.fbx", L"RebelInfantry");
		chdata->SetScale(scaleValue, scaleValue, scaleValue);

		//アニメーションを登録
		HRESULT loadIdleAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Idle.fbx", L"WAIT_REBEL_INF");
		HRESULT loadAttackAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Fire.fbx", L"ATTACK_REBEL_INF");
		HRESULT loadWalkAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Run.fbx", L"WALK_REBEL_INF");
		HRESULT loadDyingAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Dying.fbx", L"DYING_REBEL_INF");
		HRESULT loadDamageAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Damage.fbx", L"DAMAGE_REBEL_INF");

		//初期アニメーション
		if (SUCCEEDED(loadIdleAnim))
		{
			chdata->SetAnime(L"WAIT_REBEL_INF");
		}

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);

		// MeshUniqueFlagの設定
		// 該当するアニメーションがロードされている場合のみ実行
		if (SUCCEEDED(loadIdleAnim)) {
			chdata->GetAnimeFbx(L"WAIT_REBEL_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadAttackAnim)) {
			chdata->GetAnimeFbx(L"ATTACK_REBEL_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadWalkAnim)) {
			chdata->GetAnimeFbx(L"WALK_REBEL_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDyingAnim)) {
			chdata->GetAnimeFbx(L"DYING_REBEL_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDamageAnim)) {
			chdata->GetAnimeFbx(L"DAMAGE_REBEL_INF")->SetMeshUniqueFlag(true, true);
		}
	}
	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialInfantry.fbx", L"ImperialInfantry");
		chdata->SetScale(scaleValue, scaleValue, scaleValue);

		//アニメーションを登録
		HRESULT loadIdleAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Idle.fbx", L"WAIT_IMPER_INF");
		HRESULT loadAttackAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Fire.fbx", L"ATTACK_IMPER_INF");
		HRESULT loadWalkAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Run.fbx", L"WALK_IMPER_INF");
		HRESULT loadDyingAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Dying.fbx", L"DYING_IMPER_INF");
		HRESULT loadDamageAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Rifle_Damage.fbx", L"DAMAGE_IMPER_INF");

		//初期アニメーション
		if (SUCCEEDED(loadIdleAnim))
		{
			chdata->SetAnime(L"WAIT_IMPER_INF");
		}

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);

		// MeshUniqueFlagの設定
		// 該当するアニメーションがロードされている場合のみ実行
		if (SUCCEEDED(loadIdleAnim)) {
			chdata->GetAnimeFbx(L"WAIT_IMPER_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadAttackAnim)) {
			chdata->GetAnimeFbx(L"ATTACK_IMPER_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadWalkAnim)) {
			chdata->GetAnimeFbx(L"WALK_IMPER_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDyingAnim)) {
			chdata->GetAnimeFbx(L"DYING_IMPER_INF")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDamageAnim)) {
			chdata->GetAnimeFbx(L"DAMAGE_IMPER_INF")->SetMeshUniqueFlag(true, true);
		}
	}

}

bool InfantryPlayer::FrameAction()
{
	if (IsAlive)	//マトリクスの座標から装備品の座標を切り替える
	{
		m_chData = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
		m_chData->UpdateAnimation();

		if (m_RightEquipment->GetCharacterData() == nullptr) return false;

		XMMATRIX rightHandMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			rightHandMatrix = m_chData->GetBornMatrix("mixamorig:RightHand");
			break;
		case Admin::Imperial:
			rightHandMatrix = m_chData->GetBornMatrix("mixamorig:RightHand");
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

void InfantryPlayer::FinishAction()
{

}


void ArtilleryPlayer::InitAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());

	SetCharacterCameraLabel(chdata, false);
	
	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelCanon.fbx", L"RebelCanon");
	}

	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialCanon.fbx", L"ImperialCanon");
	}

	float scaleValue = 0.02f;
	chdata->SetScale(scaleValue, scaleValue, scaleValue);
	chdata->SetScaleValue(scaleValue);

	chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
	chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool ArtilleryPlayer::FrameAction()
{
	if (IsAlive)
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

void ArtilleryPlayer::FinishAction()
{
}




void MachinegunnerPlayer::InitAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());

	SetCharacterCameraLabel(chdata, true);

	chdata->LoadMainFBX(L"./Resources/fbx/rebelInfantry.fbx", L"RebelInfantry");
	float scaleValue = 0.03f;
	chdata->SetScale(scaleValue, scaleValue, scaleValue);
	chdata->SetScaleValue(scaleValue);

	//アニメーションを登録
	HRESULT loadIdleAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Machinegun_Idle.fbx", L"WAIT_REBEL_MGN");
	HRESULT loadAttackAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Machinegun_Fire.fbx", L"ATTACK_REBEL_MGN");
	HRESULT loadWalkAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Run.fbx", L"WALK_REBEL_MGN");
	HRESULT loadDyingAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Dying.fbx", L"DYING_REBEL_MGN");
	HRESULT loadDamageAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Rifle_Damage.fbx", L"DAMAGE_REBEL_MGN");

	//初期アニメーション
	if (SUCCEEDED(loadIdleAnim))
	{
		chdata->SetAnime(L"WAIT_REBEL_MGN");
	}

	chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
	chdata->GetMainFbx()->SetTextureUniqueFlag(true);

	// MeshUniqueFlagの設定
	// 該当するアニメーションがロードされている場合のみ実行
	if (SUCCEEDED(loadIdleAnim)) {
		chdata->GetAnimeFbx(L"WAIT_REBEL_MGN")->SetMeshUniqueFlag(true, true);
	}
	if (SUCCEEDED(loadAttackAnim)) {
		chdata->GetAnimeFbx(L"ATTACK_REBEL_MGN")->SetMeshUniqueFlag(true, true);
	}
	if (SUCCEEDED(loadWalkAnim)) {
		chdata->GetAnimeFbx(L"WALK_REBEL_MGN")->SetMeshUniqueFlag(true, true);
	}
	if (SUCCEEDED(loadDyingAnim)) {
		chdata->GetAnimeFbx(L"DYING_REBEL_MGN")->SetMeshUniqueFlag(true, true);
	}
	if (SUCCEEDED(loadDamageAnim)) {
		chdata->GetAnimeFbx(L"DAMAGE_REBEL_MGN")->SetMeshUniqueFlag(true, true);
	}
}

bool MachinegunnerPlayer::FrameAction()
{
	if (IsAlive)		//マトリクスの座標から装備品の座標を切り替える
	{
		m_chData = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
		m_chData->UpdateAnimation();

		if (m_RightEquipment->GetCharacterData() == nullptr) return false;

		XMMATRIX rightHandMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			rightHandMatrix = m_chData->GetBornMatrix("mixamorig:RightHand");
			break;
		case Admin::Imperial:
			rightHandMatrix = m_chData->GetBornMatrix("mixamorig:RightHand");
			break;
		}

		XMMATRIX spineMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			spineMatrix = m_chData->GetBornMatrix("mixamorig:Spine");
			break;
		case Admin::Imperial:
			spineMatrix = m_chData->GetBornMatrix("mixamorig:Spine");
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

void MachinegunnerPlayer::FinishAction()
{
}




void ScoutPlayer::InitAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());

	SetCharacterCameraLabel(chdata, true);

	float scaleValue = 0.03f;

	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelScout.fbx", L"RebelSco");
		
		chdata->SetScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);

		//アニメーションを登録
		HRESULT loadIdleAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Idle.fbx", L"WAIT_REBEL_SCT");
		HRESULT loadAttackAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Fire.fbx", L"ATTACK_REBEL_SCT");
		HRESULT loadWalkAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Run.fbx", L"WALK_REBEL_SCT");
		HRESULT loadDyingAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Dying.fbx", L"DYING_REBEL_SCT");
		HRESULT loadDamageAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/RebelAnime/rebel_Scout_Damage.fbx", L"DAMAGE_REBEL_SCT");

		//初期アニメーション
		if (SUCCEEDED(loadIdleAnim))
		{
			chdata->SetAnime(L"WAIT_REBEL_SCT");
		}

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);

		// MeshUniqueFlagの設定
		// 該当するアニメーションがロードされている場合のみ実行
		if (SUCCEEDED(loadIdleAnim)) {
			chdata->GetAnimeFbx(L"WAIT_REBEL_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadAttackAnim)) {
			chdata->GetAnimeFbx(L"ATTACK_REBEL_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadWalkAnim)) {
			chdata->GetAnimeFbx(L"WALK_REBEL_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDyingAnim)) {
			chdata->GetAnimeFbx(L"DYING_REBEL_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDamageAnim)) {
			chdata->GetAnimeFbx(L"DAMAGE_REBEL_SCT")->SetMeshUniqueFlag(true, true);
		}
	}
	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialSco.fbx", L"ImperialSco");

		chdata->SetScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);

		//アニメーションを登録
		HRESULT loadIdleAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Idle.fbx", L"WAIT_IMPER_SCT");
		HRESULT loadAttackAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Fire.fbx", L"ATTACK_IMPER_SCT");
		HRESULT loadWalkAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Run.fbx", L"WALK_IMPER_SCT");
		HRESULT loadDyingAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Dying.fbx", L"DYING_IMPER_SCT");
		HRESULT loadDamageAnim = chdata->LoadAnimationFBX(L"./Resources/fbx/ImperialAnime/imperial_Scout_Damage.fbx", L"DAMAGE_IMPER_SCT");

		//初期アニメーション
		if (SUCCEEDED(loadIdleAnim))
		{
			chdata->SetAnime(L"WAIT_IMPER_SCT");
		}

		chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
		chdata->GetMainFbx()->SetTextureUniqueFlag(true);

		// MeshUniqueFlagの設定
		// 該当するアニメーションがロードされている場合のみ実行
		if (SUCCEEDED(loadIdleAnim)) {
			chdata->GetAnimeFbx(L"WAIT_IMPER_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadAttackAnim)) {
			chdata->GetAnimeFbx(L"ATTACK_IMPER_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadWalkAnim)) {
			chdata->GetAnimeFbx(L"WALK_IMPER_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDyingAnim)) {
			chdata->GetAnimeFbx(L"DYING_IMPER_SCT")->SetMeshUniqueFlag(true, true);
		}
		if (SUCCEEDED(loadDamageAnim)) {
			chdata->GetAnimeFbx(L"DAMAGE_IMPER_SCT")->SetMeshUniqueFlag(true, true);
		}
	}
}

bool ScoutPlayer::FrameAction()
{
	if (IsAlive)		//マトリクスの座標から装備品の座標を切り替える
	{
		m_chData = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
		m_chData->UpdateAnimation();

		if (m_RightEquipment == nullptr) return false;
		if (m_RightEquipment->GetCharacterData() == nullptr) return false;

		XMMATRIX rightHandMatrix;
		switch (m_admin)
		{
		case Admin::Rebel:
			rightHandMatrix = m_chData->GetBornMatrix("mixamorig:RightHand");
			break;
		case Admin::Imperial:
			rightHandMatrix = m_chData->GetBornMatrix("mixamorig:RightHand");
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

void ScoutPlayer::FinishAction()
{
}




void ArmoredPlayer::InitAction()
{
	FBXCharacterData* chdata = static_cast<FBXCharacterData*>(GetGameObject()->GetCharacterData());
	SetCharacterCameraLabel(chdata, false);

	if (m_admin == Admin::Rebel)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/rebelarm.fbx", L"RebelArm");
		float scaleValue = 3.00f;
		chdata->SetScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);
	}
	else if (m_admin == Admin::Imperial)
	{
		chdata->LoadMainFBX(L"./Resources/fbx/imperialarm.fbx", L"ImperialArm");
		float scaleValue = 0.03f;
		chdata->SetScale(scaleValue, scaleValue, scaleValue);
		chdata->SetScaleValue(scaleValue);
	}
	chdata->GetMainFbx()->SetMeshUniqueFlag(true, true);
	chdata->GetMainFbx()->SetTextureUniqueFlag(true);
}

bool ArmoredPlayer::FrameAction()
{
	if (IsAlive)
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

void ArmoredPlayer::FinishAction()
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
	XMMATRIX gripRotate = XMMatrixRotationRollPitchYaw	//初期値ローテーションをラジアン値に変換
	(
		XMConvertToRadians(rotate.x),
		XMConvertToRadians(rotate.y),
		XMConvertToRadians(rotate.z)
	);

	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);					//初期値スケール
	XMMATRIX gripTranslate = XMMatrixTranslation(position.x, position.y, position.z);	//初期値ポジション
	XMMATRIX gripMatrix = gripRotate * gripTranslate;									//追尾するノードと移動距離を掛け算
	XMMATRIX gripInverse = XMMatrixInverse(nullptr, gripMatrix);						//逆行列化
	XMMATRIX charWorldMatrix = m_chData->GetWorldMatrix();								//ワールド座標のマトリクスを取得
	XMMATRIX finalWeaponMatrix = scaleMatrix * gripInverse * matrix * charWorldMatrix;	//

	equipment->GetCharacterData()->SetMatrixAutoUpdate(false);
	equipment->GetCharacterData()->SetWorldMatrix(finalWeaponMatrix);
}

void PlayerBase::SetCharacterCameraLabel(FBXCharacterData* characterData, bool hasAnime)
{
	if (hasAnime)
	{
		characterData->SetGraphicsPipeLine(L"AnimationFBX");
	}
	else
	{
		characterData->SetGraphicsPipeLine(L"StaticFBX");
	}
	characterData->AddCameraLabel(L"AttackerCamera");
	characterData->AddCameraLabel(L"DefenderCamera");
	characterData->AddCameraLabel(L"ScoutingCamera");
	characterData->AddCameraLabel(L"AttackerCameraForHUD");
	characterData->AddCameraLabel(L"DefenderCameraForHUD");
}
