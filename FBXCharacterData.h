#pragma once
#include <Windows.h>
#include <MyAccessHub.h>
#include <CharacterData.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <wrl/client.h>

#include <fbxsdk.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "HitShapes.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
namespace fs = std::experimental::filesystem;

class FBXDataContainer;

struct FbxVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

//ToDo: SkinAnime 01
//スキンアニメ用頂点データ構造体追加
struct FbxSkinAnimeParams
{
	//SkinMesh Unityちゃんは５点以上の影響ボーンがあるクラスタがあるので二つ必要
	XMUINT4 indices0;	//この頂点に影響を与えるボーンのインデックス値 0～3
	XMUINT4 indices1;	//この頂点に影響を与えるボーンのインデックス値 4～7
	XMFLOAT4 weight0;	//この頂点に影響を与えるボーンの影響値 0～3用
	XMFLOAT4 weight1;	//この頂点に影響を与えるボーンの影響値4～7用
};
struct FbxSkinAnimeVertex	//スタティックメッシュに追加データを加えた物
{
	FbxVertex vertex;	//スタティックメッシュの頂点データ
	FbxSkinAnimeParams skinvalues;	//スケルタルメッシュの追加データ
};
//ToDo: ここまで

enum class FBX_TEXTURE_TYPE
{
	FBX_DIFFUSE,
	FBX_NORMAL,
	FBX_SPECUAR,
	FBX_FALLOFF,
	FBX_REFLECTIONMAP,

	FBX_UNKNOWN
};

class MeshContainer
{
protected:

	FbxMesh* m_mesh;
	FBXDataContainer* m_animeFbxCont;

	bool m_uniqueMesh = false;

	const char* m_meshNodeName;
	int	m_parentNodeId;

	//ToDo: SkinAnime 02
	//スキンアニメ用メンバ変数追加
	// FbxAMatrix m_IBaseMatrix;
	FbxAMatrix m_IBaseMatrix;

	UINT m_skinCount = 0;
	//ToDo: ここまで

public:
	std::wstring m_MaterialId = L"";
	std::wstring m_MeshId = L"";

	UINT	m_vertexCount = 0;

	std::vector<FbxVertex>	m_vertexData;
	std::vector<ULONG>		m_indexData;

	//頂点データの最大値と最小値　これは判定作成時に使う
	XMFLOAT3				m_vtxMin;
	XMFLOAT3				m_vtxMax;

	~MeshContainer();

	void SetFbxMesh(FbxMesh* mesh);

	const char* GetMeshNodeName()
	{
		return m_meshNodeName;
	}

	FbxMesh* GetFbxMesh()
	{
		return m_mesh;
	}

	void SetUniqueFlag(bool mesh)
	{
		m_uniqueMesh = mesh;
	}

	//ToDo: SkinAnime 03
	//スキンアニメ用メソッド追加
	UINT GetSkinCount()
	{
		return m_skinCount;
	}

	//スキンデータ初期化。といっても今はカウントを保存するだけ。
	void InitSkinList(int skinCount);

	//アニメを切り替えた時にメッシュのID対応リストを切り替える
	void SetBoneIdList(FBXDataContainer* animeFbxCont);
	//ToDo: ここまで

};

class MaterialContainer
{
private:
	bool	m_uniqueTextures = false;

public:
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float alpha;
	std::vector<std::wstring>	m_diffuseTextures;
	std::vector<std::wstring>	m_normalTextures;
	std::vector<std::wstring>	m_specularTextures;
	std::vector<std::wstring>	m_falloffTextures;
	std::vector<std::wstring>	m_reflectionMapTextures;

	MaterialContainer()
	{
		for (int i = 0; i < 4; i++)
		{
			ambient[i] = 1.0f;
			diffuse[i] = 1.0f;
			specular[i] = 1.0f;
		}

		alpha = 1.0f;

		m_diffuseTextures.clear();
		m_normalTextures.clear();
		m_specularTextures.clear();
		m_falloffTextures.clear();
		m_reflectionMapTextures.clear();
	}

	void SetUniqueTextureFlag(bool flg)
	{
		m_uniqueTextures = flg;
	}

	~MaterialContainer();

	void SetAmbient(float r, float g, float b, float factor)
	{
		ambient[0] = r;
		ambient[1] = g;
		ambient[2] = b;
		ambient[3] = factor;
	}

	void SetDiffuse(float r, float g, float b, float factor)
	{
		diffuse[0] = r;
		diffuse[1] = g;
		diffuse[2] = b;
		diffuse[3] = factor;
	}

	void SetSpecular(float r, float g, float b, float factor)
	{
		specular[0] = r;
		specular[1] = g;
		specular[2] = b;
		specular[3] = factor;
	}

};

class FBXDataContainer
{
private:

	//ToDo: SkinAnime04
	//スキンアニメ用メンバ変数追加

	FbxScene* m_pFbxScene;	//FBXファイルデータ本体。アニメ更新に必要。
	
	LONG m_animeFrames;		//アニメフレーム数
	
	double m_startTime;		//アニメ開始時間。doubleなので注意
	double m_endTime;		//アニメ終了時間
	double m_timePeriod;	//１フレームの時間。今回は60fpsで固定
	
	FbxAnimStack* m_animeStack;		//FbxSdkの構造体。アニメーションレイヤーデータ
	
	int m_clusterCount;		//ボーンノード（関節点）の数
	int m_cbuffIndex;		//アニメデータ用定数バッファインデックス
	
	FBXDataContainer* m_currentAnimeCont;	//現在使用しているアニメ用FbxDataContainerのポインタ
	
	std::vector<const char*> m_boneNameList;	//ボーン名の配列
	std::vector<int> m_boneIdList;			//ボーンID値の配列
	std::vector<FbxAMatrix> m_IboneMatrix;	//計算用元初期ボーンの逆行列
	std::vector<XMFLOAT4X4> m_F4X4Matrix;	//アニメーションのupdateで更新されるDirect3D用マトリクス
	//ToDo: ここまで

	std::vector<const char*> m_nodeNameList;
	std::vector<unique_ptr<MeshContainer>> m_pMeshContainer;
	std::unordered_map<std::wstring, unique_ptr<MaterialContainer>> m_pMaterialContainer;

	//頂点データの最大値と最小値　このオブジェクトに読み込んだ全メッシュの中での総合値
	XMFLOAT3	m_vtxTotalMin;
	XMFLOAT3	m_vtxTotalMax;

	FBX_TEXTURE_TYPE GetTextureType(const fbxsdk::FbxBindingTableEntry& entryTable);

	HRESULT ReadFbxToMeshContainer(const std::wstring id, FbxMesh* pMesh);
	HRESULT LoadMaterial(const std::wstring id, FbxSurfaceMaterial* material);

	HRESULT LoadTextureFromMaterial(const std::wstring matName, const std::wstring id, FBX_TEXTURE_TYPE texType, const FbxProperty* fbxProp);

	//ToDo: SkinAnime05
	//スキンアニメ用クラスター数アクセサ追記
	int GetClusterId(FbxCluster* pCluster);
	int GetClusterId(FbxNode* pNode);
	//ToDo: ここまで

	FbxTime m_FbxTime;

public:
	~FBXDataContainer()
	{
		//ToDo: SkinAnime06
		//スキンアニメ用メンバ削除処理
		if (m_pFbxScene != nullptr)
		{
			m_pFbxScene->Destroy();
			m_pFbxScene = nullptr;
		}

		m_boneNameList.clear();
		m_IboneMatrix.clear();
		m_boneIdList.clear();
		m_F4X4Matrix.clear();
		//ToDo: ここまで

		m_pMeshContainer.clear();
		m_pMaterialContainer.clear();
	}

	HRESULT LoadFBX(const std::wstring fileName, const std::wstring id);

	XMFLOAT3 GetFbxMin()
	{
		return m_vtxTotalMin;
	}

	XMFLOAT3 GetFbxMax()
	{
		return m_vtxTotalMax;
	}

	MeshContainer* GetMeshContainer(int index)
	{
		if (m_pMeshContainer.size() <= index)
		{
			return nullptr;
		}

		return m_pMeshContainer[index].get();
	}

	int GetMeshCount()
	{
		return m_pMeshContainer.size();
	}

	MaterialContainer* GetMaterialContainer(const std::wstring& matName)
	{
		if (m_pMaterialContainer[matName] != nullptr)
			return m_pMaterialContainer[matName].get();

		return nullptr;
	}

	void SetMeshUniqueFlag(bool meshFlag, bool materialFlag);
	void SetTextureUniqueFlag(bool texFlag);

	int GetNodeId(const char* nodeName);

	int GetMeshId(const char* meshName);
	FbxNode* GetMeshNode(int id);

	fbxsdk::FbxManager* GetFbxManager()
	{
		static fbxsdk::FbxManager* m_spFbxManager = nullptr;

		if (m_spFbxManager == nullptr)
		{
			m_spFbxManager = fbxsdk::FbxManager::Create();
		}

		return m_spFbxManager;
	}

	//ToDo: SkinAnime07
	//スキンアニメ用メソッド追加
	void SetAnimationFbx(FBXDataContainer* animeCont); //アニメデータFBXセット
	void UpdateAnimation(const FbxTime& animeTime); //アニメFBXボーンマトリクス更新
	
	const XMFLOAT4X4* GetAnimatedMatrix(); //アニメFBXボーンマトリクス取得
	
	//アニメフレームデータを持っているのがFbxScene
	FbxScene* GetFbxScene() { return m_pFbxScene; }
	
	//時間系メンバ取得
	double GetStartTime() { return m_startTime; }
	double GetEndTime() { return m_endTime; }
	double GetPeriodTime() { return m_timePeriod; }

	//総アニメフレーム数
	LONG GetAnimeFrames() { return m_animeFrames; }

	//アニメレイヤデータ
	FbxAnimStack* GetAnimeStack() { return m_animeStack; }
	int GetClusterCount()
	{
		return m_clusterCount; //FBX内のアニメの数
	}
	void SetCBuffIndex(int index)
	{
		m_cbuffIndex = index;
	}
	int GetCBuffIndex()
	{
		return m_cbuffIndex;
	}
	//ToDo: ここまで

	HRESULT LoadBinary(const fs::path& path);
	HRESULT SaveBinary(const fs::path& path);

	XMMATRIX GetBornMaxrix(const char* bornName);
};

class FBXCharacterData : public CharacterData
{
private:

	std::unique_ptr<FBXDataContainer>	m_mainFbx;

	//スキンアニメ用メンバ追加
	std::unordered_map<std::wstring, std::unique_ptr<FBXDataContainer>> m_animeFbxMap;
	std::wstring m_currentAnimeLabel; //再生中のアニメラベル
	LONG m_animeTime; //再生時間
protected:
	float m_ScaleValue = 0.0f;
public:

	HRESULT LoadMainFBX(const std::wstring fileName, const std::wstring id);

	FBXDataContainer* GetMainFbx()
	{
		return m_mainFbx.get();
	}

	//ToDo; SkinAnime09
	//スキンアニメ用FBX読み込みとアニメ実行メソッド関係
	HRESULT LoadAnimationFBX(const std::wstring fileName, const std::wstring id); //アニメ用FBX読み込み
	FBXDataContainer* GetAnimeFbx(const std::wstring fileName); //アニメ用FBXをラベル名で取得
	
	void SetAnime(std::wstring animeLabel); //再生アニメ指定
	void UpdateAnimation(); //アニメ１フレーム更新
	void UpdateAnimation(int frameCount); //アニメフレーム指定更新
	
	void ClearAnimeFBX() //アニメFBXデータをクリア
	{
		m_animeFbxMap.clear();
	}
	//ToDo: ここまで


	//ToDo: Hit
	//当たり判定関係
	//ToDo: ここまで
	
	GameComponent* playerData = nullptr;
	
	void SetScaleValue(float value)
	{
		m_ScaleValue = value;
	}
	float GetScaleValue()
	{
		return m_ScaleValue;
	}
};
