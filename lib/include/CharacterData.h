#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>

#include <wrl/client.h>

#include <PipeLineManager.h>

#include <cmath>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class GraphicsPipeLineObjectBase;

class CharacterData
{
protected:
	XMFLOAT3	position;			//キャラクタの中心位置
	XMFLOAT3	rotation;
	XMFLOAT3	scale;

	XMMATRIX m_worldMtx; 
	XMMATRIX m_worldMtxInv; 
	bool m_mtxChange;
	bool m_isMatrixAutoUpdate = true;

	XMMATRIX m_customRotationMtx = XMMatrixIdentity();
	bool m_UseCustomRotation = false;

	std::vector<ComPtr<ID3D12Resource>> m_constantBuffers;

	GraphicsPipeLineObjectBase* m_pPipeLine = nullptr;

	UINT m_cbuffCount;

	//=========Camera Change Phase1
	std::vector<std::wstring> m_camera;
	//=========Camera Change Phase1 End

public:
	CharacterData()
	{
		setPosition(0.0f, 0.0f, 0.0f);
		SetRotation(0.0f, 0.0f, 0.0f);
		SetScale(1.0f, 1.0f, 1.0f);

		m_constantBuffers.clear();
		m_cbuffCount = 0;

		//=========Camera Change Phase1
		m_camera.resize(1);

		m_camera[0] = L"MainCamera";
		//=========Camera Change Phase1 End
	}

	void AddConstantBuffer(UINT buffSize, const void* initData);

	ID3D12Resource* GetConstantBuffer(UINT index)
	{
		if (m_cbuffCount > index)
		{
			return m_constantBuffers[index].Get();
		}

		return nullptr;
	}

	void setPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;

		m_mtxChange = true;
	}

	void SetRotation(float x, float y, float z)
	{
		rotation.x = x;
		rotation.y = y;
		rotation.z = z;

		m_mtxChange = true;
	}

	void SetScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;

		m_mtxChange = true;
	}

	const XMFLOAT3 getPosition()
	{
		return position;
	}

	const XMFLOAT3 GetRotation()
	{
		return rotation;
	}

	const XMFLOAT3 getScale()
	{
		return scale;
	}

	void SetGraphicsPipeLine(std::wstring pipelineName);

	GraphicsPipeLineObjectBase* GetPipeline()
	{
		return m_pPipeLine;
	}

	//=========Camera Change Phase1
	void SetCameraLabel(std::wstring label, int index)
	{
		m_camera[index] = label;
	}

	void AddCameraLabel(std::wstring label)
	{
		m_camera.push_back(label);
	}

	// 指定したラベルを持っているか確認する (find的な役割)
	bool HasCameraLabel(const std::wstring& label) const {
		for (const auto& l : m_camera) {
			if (l == label) {
				return true;
			}
		}
		return false;
	}

	// ラベルをクリアする
	void ClearCameraLabels() 
	{
		m_camera.clear();
	}

	std::vector<std::wstring> GetCameraLabelList()
	{
		return m_camera;
	}

	std::wstring GetCameraLabel(int index)
	{
		if (index < 0 || m_camera.size() <= index)
		{
			return L"";
		}
		return m_camera[index];
	}
	//=========Camera Change Phase1 End

	XMMATRIX& GetWorldMatrix();				//アフィン変換行列取得
	XMMATRIX& GetInverseWorldMatrix();		//逆行列取得

	void SetWorldMatrix(const XMMATRIX& xMMatrix);
	void SetMatrixAutoUpdate(bool enable) 
	{
		m_mtxChange = enable; 
		m_isMatrixAutoUpdate = enable;
	}

	void SetRotationMatrix(const XMMATRIX& mtx);
};

