#pragma once
#include <GameObject.h>
#include "CameraComponent.h"
#include "Squares.h"
#include "TimeManager.h"

class Squares;

class FlyingCameraController : public GameComponent
{
private:
	CameraComponent* m_camera;
	XMFLOAT3		m_targetPos;
	XMFLOAT3 m_MovingValue = {};

	TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

	const float kMaxMovingValue = 1.0f;

	float m_ZoomPercent = 1.0f;

	const float kZoomSpeed = 0.005f;
	const float kMaxZoom = 0.1f;
	const float kMinZoom = 1.0f;

	const XMFLOAT2 kMaxCameraPos = {25.0f, 60.0f};
	const XMFLOAT2 kMinCameraPos = {-25.0f, -10.0f};

	XMFLOAT3 m_AttackingCharacterPos = {};
	XMFLOAT3 m_AttackedCharacterPos = {};

	void StartGame();
public:
	void initAction() override;		//コンポーネント初期化時に呼ばれる処理
	bool frameAction() override;	//毎フレーム呼ばれる処理
	void finishAction() override;	//終了時に呼ばれる処理

	void ChangeCameraPosition();
	void MoveCamera();
	void SetBattleCam(Squares* attaking, Squares* attacked);
	void SetCameraFocus(XMFLOAT3 focusPos);

	bool Opening = true;

	float OpeningCount = 0.0f;
};
