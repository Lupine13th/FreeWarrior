#include <MyAccessHub.h>
#include <MyGameEngine.h>
#include "CameraComponent.h"

//====Change Scene
#include "SceneManager.h"
//====Change Scene End

void CameraComponent::initAction()
{
	m_normal.x = 0.0f;
	m_normal.y = 1.0f;
	m_normal.z = 0.0f;

	m_focus.x = 0.0f;
	m_focus.y = 0.0f;
	m_focus.z = 10.0f;

	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	m_viewportX = 0.0f;
	m_viewportY = 0.0f;
	m_viewportWidth = 0.0f;
	m_viewportHeight = 0.0f;

	//MyGameEngineが持つ共通処理、カメラ用定数バッファをCharacterDataに登録
	engine->InitCameraConstantBuffer(chData);

	updateFlg = true;
}

bool CameraComponent::frameAction()
{
	if (updateFlg)
	{
		updateFlg = false;

		//カメラマトリクスを更新
		MyGameEngine* engine = MyAccessHub::getMyGameEngine();
		CharacterData* chData = getGameObject()->getCharacterData();

		XMFLOAT3 pos = chData->getPosition();

		XMVECTOR Eye = XMVectorSet(pos.x, pos.y, pos.z, 0.0f);					//カメラ座標
		XMVECTOR At = XMVectorSet(m_focus.x, m_focus.y, m_focus.z, 0.0f);		//フォーカスする座標
		XMVECTOR Up = XMVectorSet(m_normal.x, m_normal.y, m_normal.z, 0.0f);	//カメラのロール軸

		// ... カメラマトリクス更新処理 ...
		if (m_viewportWidth > 0.0f && m_viewportHeight > 0.0f)
		{
			engine->UpdateCameraMatrixForComponent(m_fov, Eye, At, Up, m_width, m_height, m_near, m_far, GetViewport());
		}
		else
		{
			engine->UpdateCameraMatrixForComponent(m_fov, Eye, At, Up, m_width, m_height, m_near, m_far);
		}

		XMVECTOR camdir = XMVector3Normalize(XMVectorSet(m_focus.x - pos.x, m_focus.y - pos.y, m_focus.z - pos.z, 0.0f) );
		m_direction.x = XMVectorGetX(camdir);
		m_direction.y = XMVectorGetY(camdir);
		m_direction.z = XMVectorGetZ(camdir);
	}
	return true;
}

void CameraComponent::finishAction()
{
	//====Change Scene
	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);
	//====Change Scene End
}

void CameraComponent::changeCameraRatio(float width, float height)
{
	m_height = height;
	m_width = width;
	m_viewportWidth = width;
	m_viewportHeight = height;

	updateFlg = true;
}

void CameraComponent::changeCameraPosition(float x, float y, float z)
{	
	CharacterData* chData = getGameObject()->getCharacterData();
	chData->setPosition(x, y, z);

	updateFlg = true;
}

void CameraComponent::changeCameraRotation(float x, float y, float z)
{
	CharacterData* chData = getGameObject()->getCharacterData();
	chData->setRotation(x, y, z);

	updateFlg = true;
}

void CameraComponent::changeCameraFocus(float x, float y, float z)
{
	m_focus.x = x;
	m_focus.y = y;
	m_focus.z = z;

	updateFlg = true;
}

void CameraComponent::changeCameraDepth(float nearZ, float farZ)
{
	m_near = nearZ;
	m_far = farZ;
	updateFlg = true;
}

void CameraComponent::changeCameraFOVRadian(float fovRad)
{
	m_fov = fovRad;
	updateFlg = true;
}
