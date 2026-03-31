#pragma once

#include "FBXCharacterData.h"

#include <memory>

class FBXResourceManager
{
private:
	std::unordered_map<std::wstring, std::shared_ptr<FBXDataContainer>> m_FbxCache;							//IDとFBXDataContainerのペアを管理するマップ。IDはファイル名で管理する想定

public:
	std::shared_ptr<FBXDataContainer> GetorLoadFbx(const std::wstring& fileName, const std::wstring& id);
};

