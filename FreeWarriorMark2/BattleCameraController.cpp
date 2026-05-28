#include "MyAccessHub.h"
#include "BattleCameraController.h"
#include "FBXCharacterData.h"
#include "CameraComponent.h"
#include "GameObject.h"

#include <D3D12Helper.h>

void BattleCameraController::InitAction()
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	CharacterData* chData = GetGameObject()->GetCharacterData();

	m_BattleFieldManager = MyAccessHub::GetBFManager();

	engine->InitCameraConstantBuffer(chData);

	chData->setPosition(0.0f, 0.0f, 0.0f);

	XMMATRIX view;

	XMMATRIX proj;

	XMVECTOR Eye; //初期位置
	XMVECTOR At; //見る場所
	XMVECTOR Up;

	switch (m_BattleCameraState)	//SceneManagerでカメラの状態を設定している　その状態に応じてカメラの位置を変える
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
				XMConvertToRadians(45.0f),		//カメラの画角
				(FLOAT)engine->GetWidth(),		//描画エリアの横幅
				(FLOAT)engine->GetHeight() / 2,	//ExecuteRenderで描画領域を縦方向に半分にしているため、縦の描画範囲も半分にする
				0.01f,							//描画エリアの手前
				1000.0f							//描画エリアの奥
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
				XMConvertToRadians(45.0f),		//カメラの画角
				(FLOAT)engine->GetWidth(),		//描画エリアの横幅
				(FLOAT)engine->GetHeight() / 2,	//ExecuteRenderで描画領域を縦方向に半分にしているため、描画範囲も半分にする
				0.01f,							//描画エリアの手前
				1000.0f							//描画エリアの奥
			)
		);
		break;
	case BattleCameraType::ScoutingCamera:	//左上部のキャラの顔を映すカメラ
		Eye = XMVectorSet(0.0f, 10.0f, 5.0f, 0.0f);
		At = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

		proj = XMMatrixTranspose(
			MakePerspectiveProjectionMatrix(
				XMConvertToRadians(45.0f),  //カメラの画角
				160.0f,						//描画エリアの横幅
				120.0f,						//描画エリアの縦幅
				0.01f,						//描画エリアの手前
				5.0f						//映るのは顔あたりでいいので描画距離を短く
			)
		);
		break;
	case BattleCameraType::AttackerCameraForHUD:	//左上部のキャラの顔を映すカメラ
		Eye = XMVectorSet(-5.0f, 0.0f, 5.0f, 0.0f);
		At = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

		proj = XMMatrixTranspose(
			MakePerspectiveProjectionMatrix(
				XMConvertToRadians(30.0f),  //カメラの画角
				200.0f,						//描画エリアの横幅
				200.0f,						//描画エリアの縦幅
				0.01f,						//描画エリアの手前
				5.0f						//映るのは顔あたりでいいので描画距離を短く
			)
		);
		break;
	case BattleCameraType::DefenderCameraForHUD:	//左上部のキャラの顔を映すカメラ
		Eye = XMVectorSet(5.0f, 0.0f, 5.0f, 0.0f);
		At = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

		proj = XMMatrixTranspose(
			MakePerspectiveProjectionMatrix(
				XMConvertToRadians(30.0f),  //カメラの画角
				200.0f,						//描画エリアの横幅
				200.0f,						//描画エリアの縦幅
				0.01f,						//描画エリアの手前
				5.0f						//映るのは顔あたりでいいので描画距離を短く
			)
		);
		break;
	}

	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));
}


bool BattleCameraController::FrameAction()
{
	Squares* selectSquare = m_BattleFieldManager->GetFieldSquaresList()[m_BattleFieldManager->GetSelectID()];
	Squares* targetSquare = m_BattleFieldManager->GetFieldSquaresList()[m_BattleFieldManager->GetTargetID()];
	FBXCharacterData* selectFbx = nullptr;
	FBXCharacterData* targetFbx = nullptr;


	if (selectSquare != nullptr)
	{
		selectFbx = selectSquare->fbxD;
	}

	if (selectSquare != nullptr)
	{
		targetFbx = targetSquare->fbxD;
	}

	if (m_BattleCameraState == BattleCameraType::ScoutingCamera && selectFbx != nullptr)	//ヌルチェック
	{
		switch (selectSquare->chara->GetSoldiersType())
		{
		case SoldiersType::infantry:
		case SoldiersType::machinegunner:
			UpdateCameraPositionFromObject(selectFbx->GetRotation().y, selectFbx->GetPosition(), 1.0f, 4.5f);
			break;
		case SoldiersType::scout:
			UpdateCameraPositionFromObject(selectFbx->GetRotation().y, selectFbx->GetPosition(), 2.0f, 2.5f);
			break;
		case SoldiersType::artillery:
		case SoldiersType::armored:
			UpdateCameraPositionFromObject(selectFbx->GetRotation().y, selectFbx->GetPosition(), 4.0f, 2.0f);
			break;
		}
	}
	else if (m_BattleCameraState == BattleCameraType::AttackerCameraForHUD && selectFbx != nullptr)	//HUD用の攻撃用カメラ
	{
		switch (selectSquare->chara->GetSoldiersType())
		{
		case SoldiersType::infantry:
		case SoldiersType::machinegunner:
			UpdateCameraPositionFromObject(selectFbx->GetRotation().y + 45.0f, selectFbx->GetPosition(), 2.0f, 4.5f);	//右前から映るよう補正
			break;
		case SoldiersType::scout:
			UpdateCameraPositionFromObject(selectFbx->GetRotation().y + 30.0f, selectFbx->GetPosition(), 3.0f, 2.5f);	//右前から映るよう補正
			break;
		case SoldiersType::artillery:
		case SoldiersType::armored:
			UpdateCameraPositionFromObject(selectFbx->GetRotation().y + 30.0f, selectFbx->GetPosition(), 4.0f, 2.0f);	//右前から映るよう補正
			break;
		}
	}
	else if (m_BattleCameraState == BattleCameraType::DefenderCameraForHUD && targetFbx != nullptr)	//ヌルチェック
	{
		switch (targetSquare->chara->GetSoldiersType())
		{
		case SoldiersType::infantry:
		case SoldiersType::machinegunner:
			UpdateCameraPositionFromObject(targetFbx->GetRotation().y - 45.0f, targetFbx->GetPosition(), 2.0f, 4.5f);	//左前から映るよう補正
			break;
		case SoldiersType::scout:
			UpdateCameraPositionFromObject(targetFbx->GetRotation().y - 30.0f, targetFbx->GetPosition(), 3.0f, 2.5f);	//左前から映るよう補正
			break;
		case SoldiersType::artillery:
		case SoldiersType::armored:
			UpdateCameraPositionFromObject(targetFbx->GetRotation().y - 30.0f, targetFbx->GetPosition(), 4.0f, 2.0f);	//左前から映るよう補正
			break;
		}
	}

    return true;
}

void BattleCameraController::FinishAction()
{
    // 何か終了処理が必要な場合はここに記述
}

void BattleCameraController::UpdateCamera(XMVECTOR camera, XMVECTOR viewPoint)
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	CharacterData* chData = GetGameObject()->GetCharacterData();

	XMVECTOR Eye = camera;
	XMVECTOR At = viewPoint;
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));

	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
}

void BattleCameraController::UpdateCameraPositionFromObject(float rotateY, XMFLOAT3 basedPosition, float distance, float height)
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	CharacterData* chData = GetGameObject()->GetCharacterData();

	XMVECTOR objectPos = XMLoadFloat3(&basedPosition);

	float radY = rotateY * DEG_TO_RAD;	//弧度法をラジアンに変換
	float camX = sinf(radY) * distance;	//角度からカメラのX座標を設定
	float camZ = cosf(radY) * distance;	//角度からカメラのZ座標を設定

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
