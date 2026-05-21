#include <MyAccessHub.h>
#include <MyGameEngine.h>
#include "CameraComponent.h"

//====Change Scene
#include "SceneManager.h"
//====Change Scene End

void CameraComponent::InitAction()
{
	m_Normal.x = 0.0f;
	m_Normal.y = 1.0f;
	m_Normal.z = 0.0f;

	m_Focus.x = 0.0f;
	m_Focus.y = 0.0f;
	m_Focus.z = 10.0f;

	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	CharacterData* chData = GetGameObject()->GetCharacterData();

	m_ViewportX = 0.0f;
	m_ViewportY = 0.0f;
	m_ViewportWidth = 0.0f;
	m_ViewportHeight = 0.0f;

	//MyGameEngineが持つ共通処理、カメラ用定数バッファをCharacterDataに登録
	engine->InitCameraConstantBuffer(chData);

	m_UpdateFlg = true;
}

bool CameraComponent::FrameAction()
{
	if (m_UpdateFlg)
	{
		m_UpdateFlg = false;

		//カメラマトリクスを更新
		MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
		CharacterData* chData = GetGameObject()->GetCharacterData();

		XMFLOAT3 pos = chData->getPosition();

		XMVECTOR Eye = XMVectorSet(pos.x, pos.y, pos.z, 0.0f);					//カメラ座標
		XMVECTOR At = XMVectorSet(m_Focus.x, m_Focus.y, m_Focus.z, 0.0f);		//フォーカスする座標
		XMVECTOR Up = XMVectorSet(m_Normal.x, m_Normal.y, m_Normal.z, 0.0f);	//カメラのロール軸

		// ... カメラマトリクス更新処理 ...
		if (m_ViewportWidth > 0.0f && m_ViewportHeight > 0.0f)
		{
			engine->UpdateCameraMatrixForComponent(m_Fov, Eye, At, Up, m_Width, m_Height, m_Near, m_Far, GetViewport());
		}
		else
		{
			engine->UpdateCameraMatrixForComponent(m_Fov, Eye, At, Up, m_Width, m_Height, m_Near, m_Far);
		}

		XMVECTOR camdir = XMVector3Normalize(XMVectorSet(m_Focus.x - pos.x, m_Focus.y - pos.y, m_Focus.z - pos.z, 0.0f) );
		m_Direction.x = XMVectorGetX(camdir);
		m_Direction.y = XMVectorGetY(camdir);
		m_Direction.z = XMVectorGetZ(camdir);
	}
	return true;
}

void CameraComponent::FinishAction()
{
	//====Change Scene
	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);
	//====Change Scene End
}

//カメラの描画範囲を変更
void CameraComponent::ChangeCameraRatio(float width, float height)
{
	m_Height = height;
	m_Width = width;
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	m_UpdateFlg = true;
}

//カメラの位置を変更
void CameraComponent::ChangeCameraPosition(float x, float y, float z)
{	
	CharacterData* chData = GetGameObject()->GetCharacterData();
	chData->setPosition(x, y, z);

	m_UpdateFlg = true;
}

void CameraComponent::ChangeCameraRotation(float x, float y, float z)
{
	CharacterData* chData = GetGameObject()->GetCharacterData();
	chData->SetRotation(x, y, z);

	m_UpdateFlg = true;
}

void CameraComponent::ChangeCameraFocus(float x, float y, float z)
{
	m_Focus.x = x;
	m_Focus.y = y;
	m_Focus.z = z;

	m_UpdateFlg = true;
}

void CameraComponent::ChangeCameraDepth(float nearZ, float farZ)
{
	m_Near = nearZ;
	m_Far = farZ;
	m_UpdateFlg = true;
}

void CameraComponent::ChangeCameraFOVRadian(float fovRad)
{
	m_Fov = fovRad;
	m_UpdateFlg = true;
}
