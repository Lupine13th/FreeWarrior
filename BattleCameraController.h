#pragma once
#include "GameObject.h"
#include "CameraComponent.h"
#include "BattleFieldManager.h"

enum class BattleCameraType
{
    None,
    AttackerCamera,
	DefenderCamera,
	ScoutingCamera,
};

class BattleCameraController : public GameComponent {
private:
    CameraComponent* m_camera = nullptr;
    BattleCameraType m_BattleCameraState = BattleCameraType::None;
	BattleFieldManager* m_BattleFieldManager = nullptr;

    const float DEG_TO_RAD = 3.1415926535f / 180.0f;
public:
    void initAction() override;
    bool frameAction() override;
    void finishAction() override;

    void UpdateCamera(XMVECTOR camera, XMVECTOR viewPoint);
    void UpdateCameraPositionFromobject(float rotateY, XMFLOAT3 basedPosition, float distance, float height);

	void SetCameraState(BattleCameraType state) 
    {
        m_BattleCameraState = state;
    }
};
