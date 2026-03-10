#include <MyAccessHub.h>
#include <MyGameEngine.h>
#include "CharacterData.h"

void CharacterData::AddConstantBuffer(UINT buffSize, const void* initData)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	if (m_constantBuffers.size() <= m_cbuffCount)
	{
		m_constantBuffers.resize(m_cbuffCount + 5);
	}
	m_constantBuffers[m_cbuffCount].Reset();
	
	engine->CreateConstantBuffer(m_constantBuffers[m_cbuffCount].GetAddressOf(), initData, buffSize);

	m_cbuffCount++;
}

void CharacterData::SetGraphicsPipeLine(std::wstring pipelineName)
{
	PipeLineManager* plMng = MyAccessHub::getMyGameEngine()->GetPipelineManager();

	m_pPipeLine = plMng->GetPipeLineObject(pipelineName);
}

XMMATRIX& CharacterData::GetWorldMatrix()
{
	if (m_isMatrixAutoUpdate && m_mtxChange)
	{
		m_mtxChange = false;
		XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);

		XMMATRIX rotate_mat;
		if (m_UseCustomRotation) {
			// 外部で計算した行列をそのまま使う
			rotate_mat = m_customRotationMtx;
		}
		else 
		{
			// 従来の角度指定による計算
			XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(rotation.x));
			XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(rotation.y));
			XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(rotation.z));
			rotate_mat = rotate_z * rotate_x * rotate_y;
		}

		m_worldMtx = scale_mat * rotate_mat * translate;
		m_worldMtxInv = XMMatrixInverse(nullptr, m_worldMtx);
	}
	return m_worldMtx;
}

XMMATRIX& CharacterData::GetInverseWorldMatrix()
{
	if (m_isMatrixAutoUpdate && m_mtxChange)
		GetWorldMatrix(); 
	
	return m_worldMtxInv;
}

void CharacterData::SetWorldMatrix(const XMMATRIX& xMMatrix)
{
	m_worldMtx = xMMatrix;
	m_worldMtxInv = XMMatrixInverse(nullptr, xMMatrix);
	m_mtxChange = false;
	m_isMatrixAutoUpdate = false;
}

void CharacterData::SetRotationMatrix(const XMMATRIX& mtx)
{
	m_customRotationMtx = mtx;
	m_UseCustomRotation = true;
	m_mtxChange = true;
}
