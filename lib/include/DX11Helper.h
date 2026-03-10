#pragma once

#include <Windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>

HRESULT CreateInputLayoutDescFromVertexShaderSignature(const void* shaderBinary, size_t binarySize, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout);
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
DWORD	readBinaryFileToBuffer(LPCWSTR filename, void** buffer);
