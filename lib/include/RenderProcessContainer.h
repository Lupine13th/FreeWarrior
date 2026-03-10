#pragma once

#include "BaseIncludes.h"
#include "MyGameEngine.h"
#include "DXSampleHelper.h"
#include <memory>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class RenderProcessContainer
{
protected:
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FRAME_COUNT];
    std::vector<ComPtr<ID3D12Resource>> m_renderTargets;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;     //RenderTargetView Heap
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;     //DepthStencilView Heap
    //ComPtr<ID3D12DescriptorHeap> m_samHeap;     //SamplerView Heap


public:
    void Init(ID3D12Device* d3dDev, bool depthOn);

    ID3D12Resource* GetRenderTarget(int index)
    {
        if (index < 0 || m_renderTargets[index] == nullptr)
            return nullptr;

        return m_renderTargets[index].Get();
    }

    ID3D12DescriptorHeap* GetRTVDescHeap()
    {
        return m_rtvHeap.Get();
    }

    ID3D12DescriptorHeap* GetDSVDescHeap()
    {
        return m_dsvHeap.Get();
    }

    ID3D12CommandAllocator* GetCommandAllocator(int index)
    {
        if (index < 0 || index >= FRAME_COUNT)
            return nullptr;

        return m_commandAllocators[index].Get();
    }

};
