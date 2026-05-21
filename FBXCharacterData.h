#pragma once
#include <Windows.h>
#include <MyAccessHub.h>
#include <CharacterData.h>
#include "FieldCharacter.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <wrl/client.h>
#include <fstream>
#include <fbxsdk.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
namespace fs = std::experimental::filesystem;

class FBXDataContainer;

class FieldCharacter;

struct FbxVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

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

	std::string m_meshNodeName;
	int	m_parentNodeId;

	//スキンアニメ用メンバ変数
	FbxAMatrix m_IBaseMatrix;

	UINT m_skinCount = 0;

public:
	std::wstring m_MaterialId = L"";
	std::wstring m_MeshId = L"";

	UINT	m_vertexCount = 0;

	std::vector<FbxVertex>	m_vertexData;
	std::vector<ULONG>		m_indexData;
	std::vector<FbxSkinAnimeParams> m_skinParams; // キャッシュ保存用：スキン情報

	//頂点データの最大値と最小値　これは判定作成時に使う
	XMFLOAT3				m_vtxMin;
	XMFLOAT3				m_vtxMax;

	~MeshContainer();

	void SetFbxMesh(FbxMesh* mesh);

	std::string& GetMeshNodeName()
	{
		return m_meshNodeName;
	}

	int& GetParentNodeId()
	{
		return m_parentNodeId;
	}

	FbxMesh* GetFbxMesh()
	{
		return m_mesh;
	}

	FbxAMatrix& GetIBaseMatrix()
	{
		return m_IBaseMatrix;
	}

	void SetIBaseMatrix(FbxAMatrix& aMatrix)
	{
		m_IBaseMatrix = aMatrix;
	}

	void SetUniqueFlag(bool mesh)
	{
		m_uniqueMesh = mesh;
	}

	//スキンアニメ用メソッド追加
	UINT& GetSkinCount()
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

	//テクスチャの名前を保存
	std::vector<std::wstring>	m_DiffuseTextures;
	std::vector<std::wstring>	m_NormalTextures;
	std::vector<std::wstring>	m_SpecularTextures;
	std::vector<std::wstring>	m_FalloffTextures;
	std::vector<std::wstring>	m_ReflectionMapTextures;

	//テクスチャのパスを保存（SaveBinaryに利用する）
	std::vector<std::wstring>	m_DiffusePath;
	std::vector<std::wstring>	m_NormalPath;
	std::vector<std::wstring>	m_SpecularPath;
	std::vector<std::wstring>	m_FalloffPath;
	std::vector<std::wstring>	m_ReflectionMapPath;

	MaterialContainer()
	{
		for (int i = 0; i < 4; i++)
		{
			ambient[i] = 1.0f;
			diffuse[i] = 1.0f;
			specular[i] = 1.0f;
		}

		alpha = 1.0f;

		m_DiffuseTextures.clear();
		m_NormalTextures.clear();
		m_SpecularTextures.clear();
		m_FalloffTextures.clear();
		m_ReflectionMapTextures.clear();
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

	//スキンアニメ用メンバ変数追加

	FbxScene* m_FbxScene;	//FBXファイルデータ本体。アニメ更新に必要。
	
	LONG m_AnimeFrames;		//アニメフレーム数
	
	double m_StartTime;		//アニメ開始時間。doubleなので注意
	double m_EndTime;		//アニメ終了時間
	double m_TimePeriod;	//１フレームの時間。今回は60fpsで固定
	
	FbxAnimStack* m_AnimeStack;		//FbxSdkの構造体。アニメーションレイヤーデータ
	
	int m_ClusterCount;		//ボーンノード（関節点）の数
	int m_ConstantBufferIndex;		//アニメデータ用定数バッファインデックス　アニメ付きのモデルの行列データを分別するための値。シェーダー側でこの値を元に行列データの格納場所を分ける
	
	FBXDataContainer* m_CurrentAnimeCont;	//現在使用しているアニメ用FbxDataContainerのポインタ
	
	std::vector<std::string> m_BoneNameList;	//ボーン名の配列
	std::vector<int> m_BoneIdList;			//ボーンID値の配列
	std::vector<XMFLOAT4X4> m_IBoneMatrix;	//計算用元初期ボーンの逆行列
	std::vector<XMFLOAT4X4> m_F4X4Matrix;	//アニメーションのupdateで更新されるDirect3D用マトリクス

	std::vector<std::string> m_NodeNameList;
	std::vector<unique_ptr<MeshContainer>> m_MeshContainer;
	std::unordered_map<std::wstring, unique_ptr<MaterialContainer>> m_MaterialContainer;

	//頂点データの最大値と最小値　このオブジェクトに読み込んだ全メッシュの中での総合値
	XMFLOAT3	m_VertexTotalMin;
	XMFLOAT3	m_VertexTotalMax;

	FBX_TEXTURE_TYPE GetTextureType(const fbxsdk::FbxBindingTableEntry& entryTable);

	HRESULT ReadFbxToMeshContainer(const std::wstring id, FbxMesh* pMesh);
	HRESULT LoadMaterial(const std::wstring id, FbxSurfaceMaterial* material);

	HRESULT LoadTextureFromMaterial(const std::wstring matName, const std::wstring id, FBX_TEXTURE_TYPE texType, const FbxProperty* fbxProp);

	//スキンアニメ用クラスター数アクセサ追記
	int GetClusterId(FbxCluster* pCluster);
	int GetClusterId(FbxNode* pNode);

	FbxTime m_FbxTime;

public:
	~FBXDataContainer()
	{
		//スキンアニメ用メンバ削除処理
		if (m_FbxScene != nullptr)
		{
			m_FbxScene->Destroy();
			m_FbxScene = nullptr;
		}

		m_BoneNameList.clear();
		m_IBoneMatrix.clear();
		m_BoneIdList.clear();
		m_F4X4Matrix.clear();

		m_MeshContainer.clear();
		m_MaterialContainer.clear();
	}

	HRESULT LoadFBX(const std::wstring fileName, const std::wstring id);

	XMFLOAT3 GetFbxMin()
	{
		return m_VertexTotalMin;
	}

	XMFLOAT3 GetFbxMax()
	{
		return m_VertexTotalMax;
	}

	MeshContainer* GetMeshContainer(int index)
	{
		if (m_MeshContainer.size() <= index)
		{
			return nullptr;
		}

		return m_MeshContainer[index].get();
	}

	int GetMeshCount()
	{
		return m_MeshContainer.size();
	}

	MaterialContainer* GetMaterialContainer(const std::wstring& matName)
	{
		if (m_MaterialContainer[matName] != nullptr)
			return m_MaterialContainer[matName].get();

		return nullptr;
	}

	vector<string> GetBoneNameList()
	{
		return m_BoneNameList;
	}

	int GetBornIndex(const char* boneName)
	{
		for (int i = 0; i < m_BoneNameList.size(); i++)
		{
			if (strcmp(m_BoneNameList[i].c_str(), boneName) == 0)
			{
				return i;
			}
		}

		return -1;
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

	//スキンアニメ用メソッド追加
	void SetAnimationFbx(FBXDataContainer* animeCont); //アニメデータFBXセット
	void UpdateAnimation(FBXDataContainer* animeCont, const FbxTime& animeTime, vector<XMFLOAT4X4>& chDataMatrix, vector<XMFLOAT4X4>& nodeMatrices, const vector<int>& bornIdList); //アニメFBXボーンマトリクス更新　※仕様変更04/17　FbxCharacterDataが行列を独自で持つようになったためそれを宣言するように。
	
	const XMFLOAT4X4* GetAnimatedMatrix(); //アニメFBXボーンマトリクス取得
	
	//アニメフレームデータを持っているのがFbxScene
	FbxScene* GetFbxScene() { return m_FbxScene; }
	
	//時間系メンバ取得
	double GetStartTime() { return m_StartTime; }
	double GetEndTime() { return m_EndTime; }
	double GetPeriodTime() { return m_TimePeriod; }

	//総アニメフレーム数
	LONG GetAnimeFrames() { return m_AnimeFrames; }

	//アニメレイヤデータ
	FbxAnimStack* GetAnimeStack() { return m_AnimeStack; }
	int GetClusterCount()
	{
		return m_ClusterCount; //FBX内のアニメの数
	}
	void SetCBuffIndex(int index)
	{
		m_ConstantBufferIndex = index;
	}
	int GetCBuffIndex()
	{
		return m_ConstantBufferIndex;
	}
	FBXDataContainer* GetCurrentAnimCont()
	{
		return m_CurrentAnimeCont;
	}

	HRESULT LoadBinary(const fs::path& path);
	HRESULT SaveBinary(const fs::path& path);

	XMMATRIX GetBornMatrix(const char* bornName);
};

class FBXCharacterData : public CharacterData
{
private:
	std::shared_ptr<FBXDataContainer>	m_MainFbx;	//データを共有可能にして複数のキャラクターデータで同じFBXデータを共有できるようにする。

	std::vector<int> m_BoneIdList;	//このデータ用のボーンID配列。アニメ更新で更新される。スタティックメッシュの頂点変換に使用する。

	int m_CBuffIndex;	//アニメーション用定数バッファインデックス

	//スキンアニメ用メンバ追加
	std::unordered_map<std::wstring, std::unique_ptr<FBXDataContainer>> m_AnimeFbxMap;
	std::wstring m_CurrentAnimeLabel; //再生中のアニメラベル
	LONG m_AnimeTime; //再生時間

	std::vector<XMFLOAT4X4> m_AnimatedMatrix;	//キャッシュ処理の過程で追加。m_MainFbxをSharedポインタに変更したため、データごとに行列バッファを持つように。
	std::vector<XMFLOAT4X4> m_NodeMatrices;		//何かを持たせる時に使う行列
protected:
	float m_ScaleValue = 0.0f;
public:

	HRESULT LoadMainFBX(const std::wstring fileName, const std::wstring id);	//メインFBX読み込み

	FBXDataContainer* GetMainFbx()
	{
		return m_MainFbx.get();
	}

	//スキンアニメ用FBX読み込みとアニメ実行メソッド関係
	HRESULT LoadAnimationFBX(const std::wstring fileName, const std::wstring id); //アニメ用FBX読み込み
	FBXDataContainer* GetAnimeFbx(const std::wstring fileName); //アニメ用FBXをラベル名で取得
	
	void SetAnime(std::wstring animeLabel); //再生アニメ指定
	void UpdateAnimation(); //アニメ１フレーム更新
	void UpdateAnimation(int frameCount); //アニメフレーム指定更新
	
	void ClearAnimeFBX() //アニメFBXデータをクリア
	{
		m_AnimeFbxMap.clear();
	}
	
	GameComponent* playerData = nullptr;
	
	void SetScaleValue(float value)
	{
		m_ScaleValue = value;
	}
	float GetScaleValue()
	{
		return m_ScaleValue;
	}

	int GetCBuffIndex()
	{
		return m_CBuffIndex;
	}

	const XMFLOAT4X4* GetAnimatedMatrix()
	{
		return m_AnimatedMatrix.data();
	}

	XMMATRIX GetBornMatrix(const char* bornName);

	void SetAnimeInit(std::wstring initAnimeLabel, FieldCharacter* chara);
};

//バイナリー書き込み
template<typename T>
void WriteBinary(std::ofstream& ofs, const T& data)
{
	ofs.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

//Vector書き込み
template<typename T>
void WriteVector(std::ofstream& ofs, const std::vector<T>& vec)
{
	size_t size = vec.size();
	WriteBinary(ofs, size);
	if (size > 0)
	{
		ofs.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * size);
	}
}

//String書き込み
inline void WriteString(std::ofstream& ofs, const std::string& str)
{
	size_t size = str.size();
	WriteBinary(ofs, size);
	if (size > 0)
	{
		ofs.write(str.c_str(), size);
	}
}

//WString書き込み
inline void WriteWString(std::ofstream& ofs, const std::wstring& str)
{
	size_t size = str.size();
	WriteBinary(ofs, size);
	if (size > 0)
	{
		ofs.write(reinterpret_cast<const char*>(str.c_str()), sizeof(wchar_t) * size);
	}
}

//バイナリ読み込みヘルパー
template<typename T>
void ReadBinary(std::ifstream& ifs, T& data)
{
	ifs.read(reinterpret_cast<char*>(&data), sizeof(T));
}

//vector読み込み
template<typename T>
void ReadVector(std::ifstream& ifs, std::vector<T>& vec)
{
	size_t size;
	ReadBinary(ifs, size);
	vec.resize(size);
	if (size > 0)
	{
		ifs.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * size);
	}
}

//string読み込み
inline void ReadString(std::ifstream& ifs, std::string& str)
{
	size_t size;
	ReadBinary(ifs, size);
	str.resize(size);
	if (size > 0)
	{
		ifs.read(&str[0], size);
	}
}

//wstring読み込み
inline void ReadWString(std::ifstream& ifs, std::wstring& str)
{
	size_t size;
	ReadBinary(ifs, size);
	str.resize(size);
	if (size > 0)
	{
		ifs.read(reinterpret_cast<char*>(&str[0]), sizeof(wchar_t) * size);
	}
}
