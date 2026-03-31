#include "FBXResourceManager.h"

std::shared_ptr<FBXDataContainer> FBXResourceManager::GetorLoadFbx(const std::wstring& fileName, const std::wstring& id)	//ファイル名とIDからFBXDataContainerを入手。キャッシュがあれば入手、なければロードして入手
{
	auto cache = m_FbxCache.find(id);																	//IDでキャッシュを検索
	if (cache != m_FbxCache.end())
	{
		return cache->second;																			//キャッシュがあれば共有
	}

	auto fbxContainer = std::make_shared<FBXDataContainer>();											//キャッシュがなければ新規作成
	if (SUCCEEDED(fbxContainer->LoadFBX(fileName, id)))
	{
		m_FbxCache[id] = fbxContainer;																	//ロード成功ならキャッシュに登録
		return fbxContainer;
	}

	return nullptr;																						//ロード失敗ならnullptr
}
