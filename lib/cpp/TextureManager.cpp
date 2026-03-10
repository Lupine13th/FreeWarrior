#include <Windows.h>
#include <vector>
#include "TextureManager.h"
#include <WICTextureLoader.h>	
#include "DXSampleHelper.h"
#include "d3dx12.h"


#pragma comment(lib, "DirectXTK12.lib")
#pragma comment(lib, "DirectXTex.lib")

using namespace std;
using namespace DirectX;


HRESULT TextureManager::InitTextureManager(void)
{
	m_textureDB.clear();

	return S_OK;
}

void TextureManager::ReleaseTexObj(Texture2DContainer* txbuff)
{
	txbuff->m_pTexture.Reset();
	txbuff->m_wicData.reset();
	txbuff->m_pTextureUploadHeap.Reset();

	txbuff->m_uploaded = false;
}

void	TextureManager::DestructTextureManager(void)
{
	ReleaseAllTextures();
}

HRESULT TextureManager::CreateTextureFromFile(ID3D12Device* pD3D, std::wstring labelName, const wchar_t* filename)
{
	HRESULT hr;

	m_textureDB[labelName].reset(new Texture2DContainer());

	hr = CreateTextureFromFile(pD3D, m_textureDB[labelName].get(), filename);

	return hr;
}

HRESULT TextureManager::UploadCreatedTextures(ID3D12Device* pD3D, ID3D12GraphicsCommandList* pCmdList, ID3D12CommandQueue* pCmdQueue)
{
	HRESULT hres = E_FAIL;

	for (auto const &pair : m_textureDB)
	{
		Texture2DContainer* txbuff = pair.second.get();

		if (!txbuff->m_uploaded) continue;

		const UINT subresoucesize = 1;	//DDSの場合は複数同時があるんだけど、WICは一つだけなので

		UpdateSubresources(pCmdList,
			txbuff->m_pTexture.Get(),
			txbuff->m_pTextureUploadHeap.Get(),
			0,
			0,
			subresoucesize,
			&txbuff->m_subresouceData);

		CD3DX12_RESOURCE_BARRIER tra = CD3DX12_RESOURCE_BARRIER::Transition(txbuff->m_pTexture.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		pCmdList->ResourceBarrier(1, &tra);

		pCmdList->DiscardResource(txbuff->m_pTextureUploadHeap.Get(), nullptr);

		txbuff->m_uploaded = false;

	}

	ThrowIfFailed(hres = pCmdList->Close());
	ID3D12CommandList* ppCommandLists[] = { pCmdList };
	pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	return hres;
}

void TextureManager::ReleaseTexture(std::wstring labelName)
{
	if (m_textureDB[labelName] != nullptr)
	{
		ReleaseTexObj(m_textureDB[labelName].get());
		m_textureDB[labelName].reset();
	}
}

void TextureManager::ReleaseAllTextures(void)
{
	for (auto const& pair : m_textureDB)
	{
		ReleaseTexObj(pair.second.get());
	}

	m_textureDB.clear();
}

void TextureManager::CreateTextureSRV(ID3D12Device* pD3D, ID3D12DescriptorHeap* pSrvHeap, UINT slotNo, std::wstring texLabel)
{
	if (m_textureDB[texLabel] != nullptr)
	{
		CreateTextureSRV(pD3D, pSrvHeap, slotNo, m_textureDB[texLabel].get());
	}
}

void TextureManager::CreateTextureSRV(ID3D12Device* pD3D, ID3D12DescriptorHeap* pSrvHeap, UINT slotNo, Texture2DContainer* txbuff)
{
	UINT offset = pD3D->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	size_t startAddr = pSrvHeap->GetCPUDescriptorHandleForHeapStart().ptr;

	D3D12_CPU_DESCRIPTOR_HANDLE heapHandle;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = txbuff->texFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	heapHandle.ptr = startAddr + offset * slotNo;

	pD3D->CreateShaderResourceView(txbuff->m_pTexture.Get(), &srvDesc, heapHandle);//koko
  
}

Texture2DContainer* TextureManager::GetTexture(std::wstring labelName)
{
	if (m_textureDB[labelName] != nullptr)
	{
		return m_textureDB[labelName].get();
	}

	return nullptr;
}

TextureManager::~TextureManager()
{
	DestructTextureManager();
}

HRESULT TextureManager::CreateTextureFromFile(ID3D12Device* pD3D, Texture2DContainer* txbuff, const wchar_t* filename, bool genMipmap)
{
	HRESULT hres = E_FAIL;

	ReleaseTexObj(txbuff);

	enum TexMode
	{
		PNG,
		TGA,
		NONE,
	};

	TexMode texMode = TexMode::NONE;

	if (wcsstr(filename, L".png") != nullptr)		//正常
	{
		hres = LoadWICTextureFromFile(pD3D, filename, (ID3D12Resource**)txbuff->m_pTexture.GetAddressOf(), txbuff->m_wicData, txbuff->m_subresouceData);

		texMode = TexMode::PNG;
	}

	else if (wcsstr(filename, L".tga") != nullptr)
	{
		//TGA読み込みメソッドの実行
		hres = DirectX::LoadFromTGAFile(filename, &txbuff->m_metaData, txbuff->m_scImage);
		if (SUCCEEDED(hres))
		{
			//テクスチャ作成
			hres = DirectX::CreateTexture(pD3D, txbuff->m_metaData, (ID3D12Resource**)txbuff->m_pTexture.GetAddressOf());
		}
		else
		{
			return E_FAIL;
		}
		//Texture2DContainerにデータを登録
		txbuff->m_subresouceData.pData = txbuff->m_scImage.GetPixels();
		txbuff->m_subresouceData.SlicePitch = txbuff->m_scImage.GetPixelsSize();
		txbuff->m_subresouceData.RowPitch = txbuff->m_subresouceData.SlicePitch / txbuff->m_metaData.width;
		texMode = TexMode::TGA;
	}

	if (SUCCEEDED(hres))
	{

		D3D12_RESOURCE_DESC texDesc;
		texDesc = txbuff->m_pTexture->GetDesc();
		txbuff->fWidth = static_cast<float>(texDesc.Width);
		txbuff->fHeight = static_cast<float>(texDesc.Height);
		txbuff->texFormat = texDesc.Format;

		txbuff->m_uploaded = true;

		UINT64 uploadBufferSize;

		switch (texMode)
		{
		case TexMode::PNG:
			uploadBufferSize = txbuff->m_subresouceData.SlicePitch;
			break;
		case TexMode::TGA:
			uploadBufferSize = txbuff->m_scImage.GetPixelsSize();
			break;
		}

		CD3DX12_HEAP_PROPERTIES upHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		ThrowIfFailed(pD3D->CreateCommittedResource(
			&upHeap,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(txbuff->m_pTextureUploadHeap.GetAddressOf())));

	}

	return hres;
}
