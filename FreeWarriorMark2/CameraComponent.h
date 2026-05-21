#pragma once

#include <GameObject.h>

using namespace DirectX;

class CameraComponent : public GameComponent
{
private:
	XMFLOAT3	m_Normal;
	XMFLOAT3	m_Focus;
	XMFLOAT3	m_Direction;

	float		m_Near;
	float		m_Far;
	float		m_Fov;

	float		m_Width;
	float		m_Height;

	// Viewport
	float m_ViewportX = 0.0f;
	float m_ViewportY = 0.0f;
	float m_ViewportWidth = 0.0f;
	float m_ViewportHeight = 0.0f;

	bool m_UpdateFlg;


public:

	void InitAction() override;		//コンポーネント初期化時に呼ばれる処理
	bool FrameAction() override;		//毎フレーム呼ばれる処理　falseを返すとこのコンポーネントは終了し削除される
	void FinishAction() override;		//終了時に呼ばれる処理

	void ChangeCameraRatio(float width, float height);
	void ChangeCameraPosition(float x, float y, float z);
	void ChangeCameraRotation(float x, float y, float z);
	void ChangeCameraFocus(float x, float y, float z);
	void ChangeCameraDepth(float nearZ, float farZ);
	void ChangeCameraFOVRadian(float fovRad);

	XMFLOAT3 GetCameraNormal()
	{
		return m_Normal;
	}

	XMFLOAT3 GetCameraDirection()
	{
		return m_Direction;
	}

	XMFLOAT3 GetCameraFocus()
	{
		return m_Focus;
	}

	float GetViewRatio()
	{
		return m_Width / m_Height;
	}

	float GetSetWidth()
	{
		return m_Width;
	}

	float GetSetHeight()
	{
		return m_Height;
	}

	float GetCameraFOVRad()
	{
		return m_Fov;
	}

	XMFLOAT2 GetCameraRange()
	{
		return XMFLOAT2(m_Near, m_Far);
	}

	void SetViewport(float x, float y, float width, float height)
	{
		m_ViewportX = x;
		m_ViewportY = y;
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_UpdateFlg = true;
	}

	XMFLOAT4 GetViewport() const
	{
		return XMFLOAT4(m_ViewportX, m_ViewportY, m_ViewportWidth, m_ViewportHeight);
	}

};
