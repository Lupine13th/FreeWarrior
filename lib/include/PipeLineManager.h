#pragma once

#include "MyAccessHub.h"
#include "GameObject.h"
#include <dxcapi.h>

#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <string>

using Microsoft::WRL::ComPtr;
using namespace std;

class GameObject;
class CharacterData;

class GraphicsPipeLineObjectBase
{

protected:
	ComPtr<ID3D12CommandQueue> m_cmdQueue;
	ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
	std::vector <ComPtr<ID3D12GraphicsCommandList>> m_cmdLists;

	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pipeLineState;

	std::list<CharacterData*> m_renderList;

	//texturelist
	std::unordered_map<std::wstring, int> m_srvTexList;
	UINT m_numOfTex = 0;

public:
	virtual HRESULT InitPipeLineStateObject(ID3D12Device2* d3dDev) = 0;

	virtual ID3D12GraphicsCommandList* ExecuteRender() = 0;

	virtual void AddRenderObject(CharacterData* obj)
	{
		m_renderList.push_back(obj);
	}

	void ClearRenderList()
	{
		m_renderList.clear();
	}

	ID3D12CommandQueue* GetCommandQueue()
	{
		return m_cmdQueue.Get();
	}

	ID3D12CommandAllocator* GetCommandAllocator()
	{
		return m_cmdAllocator.Get();
	}

	ID3D12GraphicsCommandList* GetCommandList(int index)
	{
		if (index < 0 || m_cmdLists.size() <= index)
		{
			return nullptr;
		}

		return m_cmdLists[index].Get();
	}

	ID3D12RootSignature* GetRootSignature()
	{
		return m_rootSignature.Get();
	}

	ID3D12PipelineState* GetPipelineState()
	{
		return m_pipeLineState.Get();
	}
};

class PipeLineManager
{

private:
	unordered_map<std::wstring, std::unique_ptr<GraphicsPipeLineObjectBase>>	m_pipeLineDB;
	vector<ID3D12CommandList*> m_renderCommandList;

public:
	GraphicsPipeLineObjectBase* GetPipeLineObject(const std::wstring labelName)
	{
		if (m_pipeLineDB[labelName] != nullptr)
			return m_pipeLineDB[labelName].get();

		return nullptr;
	}

	void AddPipeLineObject(const std::wstring labelName, GraphicsPipeLineObjectBase* obj);

	ID3D12CommandList* const* GetCommandList()
	{
		return m_renderCommandList.data();
	}

	UINT Render();
};
