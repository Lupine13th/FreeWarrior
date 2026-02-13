#include "MyAccessHub.h"
#include "BattleCameraController.h"
#include "CameraComponent.h"
#include "GameObject.h"

#include <D3D12Helper.h>

void BattleCameraController::initAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	m_BattleFieldManager = MyAccessHub::GetBFManager();

	engine->InitCameraConstantBuffer(chData);

	chData->setPosition(0.0f, 0.0f, 0.0f);

	XMMATRIX view;

	XMMATRIX proj;

	XMVECTOR Eye; //初期位置
	XMVECTOR At; //見る場所
	XMVECTOR Up;

	switch (m_BattleCameraState)
	{
	default:
		break;
	case BattleCameraType::AttackerCamera:
		Eye = XMVectorSet(15.0f, 10.0f, -50.0f, 0.0f);
		At = XMVectorSet(40.0f, 8.0f, -50.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

		proj = XMMatrixTranspose(
			MakePerspectiveProjectionMatrix(
				XMConvertToRadians(45.0f),  // FovAngleY (ラジアン)
				(FLOAT)engine->GetWidth(),
				(FLOAT)engine->GetHeight() / 2,
				0.01f,
				1000.0f
			)
		);
		break;
	case BattleCameraType::DefenderCamera:
		Eye = XMVectorSet(15.0f, 10.0f, 50.0f, 0.0f);
		At = XMVectorSet(40.0f, 8.0f, 50.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

		proj = XMMatrixTranspose(
			MakePerspectiveProjectionMatrix(
				XMConvertToRadians(45.0f),  // FovAngleY (ラジアン)
				(FLOAT)engine->GetWidth(),
				(FLOAT)engine->GetHeight() / 2,
				0.01f,
				1000.0f
			)
		);
		break;
	case BattleCameraType::ScoutingCamera:
		Eye = XMVectorSet(0.0f, 10.0f, 5.0f, 0.0f);
		At = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

		proj = XMMatrixTranspose(
			MakePerspectiveProjectionMatrix(
				XMConvertToRadians(45.0f),  // FovAngleY (ラジアン)
				160.0f,
				120.0f,
				0.01f,
				1000.0f
			)
		);
		break;
	}

	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));
}


bool BattleCameraController::frameAction()
{
	Squares* selectSquare = m_BattleFieldManager->GetFieldSquaresList()[m_BattleFieldManager->GetSelectID()];

	if (m_BattleCameraState == BattleCameraType::ScoutingCamera && selectSquare->chara != nullptr)
	{
		switch (selectSquare->chara->CharaKind)
		{
		case SoldiersType::infantry:
		case SoldiersType::machinegunner:
			UpdateCameraPositionFromobject(selectSquare->fbxD->getRotation().y, selectSquare->fbxD->getPosition(), 1.0f, 4.5f);
			break;
		case SoldiersType::scout:
			UpdateCameraPositionFromobject(selectSquare->fbxD->getRotation().y, selectSquare->fbxD->getPosition(), 2.0f, 2.5f);
			break;
		case SoldiersType::artillery:
		case SoldiersType::armored:
			UpdateCameraPositionFromobject(selectSquare->fbxD->getRotation().y, selectSquare->fbxD->getPosition(), 4.0f, 2.0f);
			break;
		}
	}

    return true;
}

void BattleCameraController::finishAction()
{
    // 何か終了処理が必要な場合はここに記述
}

void BattleCameraController::UpdateCamera(XMVECTOR camera, XMVECTOR viewPoint)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	XMVECTOR Eye = camera;
	XMVECTOR At = viewPoint;
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
}

void BattleCameraController::UpdateCameraPositionFromobject(float rotateY, XMFLOAT3 basedPosition, float distance, float height)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	XMVECTOR objectPos = XMLoadFloat3(&basedPosition);

	float radY = rotateY * DEG_TO_RAD;
	float camX = sinf(radY) * distance;
	float camZ = cosf(radY) * distance;

	XMVECTOR Eye = XMVectorSet(
		XMVectorGetX(objectPos) + camX,
		XMVectorGetY(objectPos) + height,
		XMVectorGetZ(objectPos) + camZ,
		0.0f
	);
	XMVECTOR Up = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);

	XMVECTOR At = XMVectorSet(basedPosition.x, basedPosition.y + height, basedPosition.z, 0.0f);

	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
}
