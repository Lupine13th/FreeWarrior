#include <d3d12.h>
#include <MyAccessHub.h>
#include "MeshManager.h"
#include "DXSampleHelper.h" //ThrowIfFailed等

void MeshManager::createPresetMeshData()
{
    HRESULT hr;
    //IndexBuffer
    ULONG indices[] =
    {
        1,0,2,
        0,3,2,
    };

    hr = AddIndexBuffer(L"Sprite", indices, sizeof(ULONG), 6);

    //VertexBuffer
    UINT stride = sizeof(SpriteVertex);
    UINT offset = 0;
    SpriteVertex vertices[] =                                               //Sprite用頂点データ作成
    {
        { XMFLOAT3(-0.5f, -0.5f, 1.0f), XMFLOAT2(0, 1) },   //左上
        { XMFLOAT3(0.5f, -0.5f, 1.0f), XMFLOAT2(1, 1) },    //右上
        { XMFLOAT3(0.5f, 0.5f, 1.0f), XMFLOAT2(1, 0) },     //右下
        { XMFLOAT3(-0.5f, 0.5f, 1.0f), XMFLOAT2(0, 0) },    //左下
    };

    hr = AddVertexBuffer(L"Sprite", vertices, sizeof(SpriteVertex), 4);

}

void MeshManager::createCylinderMeshData()
{
    const UINT sliceCount = 16; // 円周の分割数
    const float radius = 0.5f;
    const float height = 1.0f;

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT2 uv;
    };

    std::vector<Vertex> vertices;
    std::vector<ULONG> indices;

    // ==== 上面中心点 ====
    vertices.push_back({ XMFLOAT3(0.0f, height / 2, 0.0f), XMFLOAT2(0.5f, 0.5f) });

    // ==== 上面 円周 ====
    for (UINT i = 0; i <= sliceCount; ++i)
    {
        float angle = XM_2PI * i / sliceCount;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        vertices.push_back({ XMFLOAT3(x, height / 2, z), XMFLOAT2((x + 1.0f) * 0.5f, (z + 1.0f) * 0.5f) });
    }

    // ==== 底面中心点 ====
    UINT bottomCenterIndex = (UINT)vertices.size();
    vertices.push_back({ XMFLOAT3(0.0f, -height / 2, 0.0f), XMFLOAT2(0.5f, 0.5f) });

    // ==== 底面 円周 ====
    UINT bottomStartIndex = (UINT)vertices.size();
    for (UINT i = 0; i <= sliceCount; ++i)
    {
        float angle = XM_2PI * i / sliceCount;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        vertices.push_back({ XMFLOAT3(x, -height / 2, z), XMFLOAT2((x + 1.0f) * 0.5f, (z + 1.0f) * 0.5f) });
    }

    // ==== 側面 ====
    UINT sideStartIndex = (UINT)vertices.size();
    for (UINT i = 0; i <= sliceCount; ++i)
    {
        float angle = XM_2PI * i / sliceCount;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);

        // 上側
        vertices.push_back({ XMFLOAT3(x, height / 2, z), XMFLOAT2((float)i / sliceCount, 0.0f) });

        // 下側
        vertices.push_back({ XMFLOAT3(x, -height / 2, z), XMFLOAT2((float)i / sliceCount, 1.0f) });
    }

    // ==== 上面インデックス ====
    for (UINT i = 1; i <= sliceCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // ==== 底面インデックス ====
    for (UINT i = 1; i <= sliceCount; ++i)
    {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomStartIndex + i + 1);
        indices.push_back(bottomStartIndex + i);
    }

    // ==== 側面インデックス ====
    UINT sideBase = sideStartIndex;
    for (UINT i = 0; i < sliceCount; ++i)
    {
        UINT top1 = sideBase + i * 2;
        UINT bottom1 = sideBase + i * 2 + 1;
        UINT top2 = sideBase + i * 2 + 2;
        UINT bottom2 = sideBase + i * 2 + 3;

        // 三角形1
        indices.push_back(top1);
        indices.push_back(bottom1);
        indices.push_back(top2);

        // 三角形2
        indices.push_back(top2);
        indices.push_back(bottom1);
        indices.push_back(bottom2);
    }

    // ==== DirectX12用バッファ登録 ====
    AddVertexBuffer(L"Cylinder", vertices.data(), sizeof(Vertex), (UINT)vertices.size());
    AddIndexBuffer(L"Cylinder", indices.data(), sizeof(ULONG), (UINT)indices.size());
}

void MeshManager::SetVertexBuffer(ID3D12GraphicsCommandList* m_cmdList, const std::wstring idName)
{
    if (m_crVertex == idName)
    {
        return;
    }

    //IDを保存
    m_crVertex = idName;

    m_cmdList->IASetVertexBuffers(0, 1, &m_ViewContainers[idName]->vbView);
}

void MeshManager::SetIndexBuffer(ID3D12GraphicsCommandList* m_cmdList, const std::wstring idName)
{
    if (m_crIndex == idName)
    {
        return;
    }

    //IDを保存
    m_crIndex = idName;

    m_cmdList->IASetIndexBuffer(&m_ViewContainers[idName]->ibView);
}

HRESULT MeshManager::AddVertexBuffer(const std::wstring idName, const void* initBuff, UINT vertexSize, UINT vertexCount)
{

    m_VertexBuffers[idName].reset();
    m_VertexBuffers[idName] = make_unique<BufferContainer>();
    m_VertexBuffers[idName]->dataCount = vertexCount;
    m_VertexBuffers[idName]->dataSize = vertexSize;
    m_VertexBuffers[idName]->pBuffer.Reset();

	HRESULT hr = MyAccessHub::GetMyGameEngine()->CreateVertexBuffer(m_VertexBuffers[idName]->pBuffer.GetAddressOf(), initBuff, vertexSize, vertexCount);

    if (FAILED(hr))
        return hr;

    m_VertexBuffers[idName]->pBuffer->SetName(idName.c_str());

    if (m_ViewContainers.find(idName) == m_ViewContainers.end())
    {
        m_ViewContainers[idName].reset();
        m_ViewContainers[idName] = make_unique<ViewContainer>();
    }

    m_ViewContainers[idName]->vbView.BufferLocation = m_VertexBuffers[idName]->pBuffer->GetGPUVirtualAddress();
    m_ViewContainers[idName]->vbView.SizeInBytes = vertexSize * vertexCount;
    m_ViewContainers[idName]->vbView.StrideInBytes = vertexSize;

    return hr;

}

HRESULT MeshManager::AddIndexBuffer(const std::wstring idName, const void* initBuff, UINT valueSize, UINT indexCount)
{
    m_IndexBuffers[idName].reset();
    m_IndexBuffers[idName] = make_unique<BufferContainer>();
    m_IndexBuffers[idName]->dataSize = valueSize;
    m_IndexBuffers[idName]->dataCount = indexCount;
    m_IndexBuffers[idName]->pBuffer.Reset();

    HRESULT hr = MyAccessHub::GetMyGameEngine()->CreateIndexBuffer(m_IndexBuffers[idName]->pBuffer.GetAddressOf(), initBuff, valueSize, indexCount);
    if (FAILED(hr))
        return hr;

    m_IndexBuffers[idName]->pBuffer->SetName(idName.c_str());

    if (m_ViewContainers.find(idName) == m_ViewContainers.end())
    {
        m_ViewContainers[idName].reset();
        m_ViewContainers[idName] = make_unique<ViewContainer>();
    }
    m_ViewContainers[idName]->ibView.BufferLocation = m_IndexBuffers[idName]->pBuffer->GetGPUVirtualAddress();
    m_ViewContainers[idName]->ibView.SizeInBytes = valueSize * indexCount;

    switch (valueSize)
    {
    case 4:
        m_ViewContainers[idName]->ibView.Format = DXGI_FORMAT_R32_UINT;
        break;

    default:
        m_ViewContainers[idName]->ibView.Format = DXGI_FORMAT_R16_UINT;
        break;
    }

    return hr;
}

D3D12_VERTEX_BUFFER_VIEW* MeshManager::GetVertexBufferView(const std::wstring idName)
{
	return &m_ViewContainers[idName]->vbView;
}

D3D12_INDEX_BUFFER_VIEW* MeshManager::GetIndexBufferView(const std::wstring idName)
{
	return &m_ViewContainers[idName]->ibView;
}

void MeshManager::resetMesh()
{
    m_crIndex = L"";
    m_crVertex = L"";
}

void MeshManager::removeVertexBuffer(const std::wstring id, bool withIndex)
{
    m_VertexBuffers.erase(id);

    if (withIndex)
        removeIndexBuffer(id);
}

void MeshManager::removeIndexBuffer(const std::wstring id)
{
    m_IndexBuffers.erase(id);

    m_ViewContainers.erase(id);
}
