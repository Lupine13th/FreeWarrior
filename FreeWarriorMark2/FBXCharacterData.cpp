#include <algorithm>
#include <DirectXMath.h>

#include <MyAccessHub.h>
#include <MyGameEngine.h>
#include "FBXCharacterData.h"
#include "FBXResourceManager.h"
#include <memory>
#include <string>
#include <unordered_map>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <Windows.h>


using namespace fbxsdk;
using namespace DirectX;

class FBXResourceManager;

//FbxAMatrixをXMFLOAT4x4に変換 右手系から左手系への変換
void ConvertFbxAMatrixToXMFLOAT4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			xmfloat4x4.m[r][c] = static_cast<float>(fbxamatrix.Get(r, c));
		}
	}
}

//FbxAMatrixをXMMATRIXに変換
void ConvertFbxAMatrixToXMMATRIX(const FbxAMatrix& fbxAMatrix, DirectX::XMMATRIX& xMMATRIX)
{
	xMMATRIX = DirectX::XMMATRIX(
		(float)fbxAMatrix.Get(0, 0), (float)fbxAMatrix.Get(0, 1), (float)fbxAMatrix.Get(0, 2), (float)fbxAMatrix.Get(0, 3),
		(float)fbxAMatrix.Get(1, 0), (float)fbxAMatrix.Get(1, 1), (float)fbxAMatrix.Get(1, 2), (float)fbxAMatrix.Get(1, 3),
		(float)fbxAMatrix.Get(2, 0), (float)fbxAMatrix.Get(2, 1), (float)fbxAMatrix.Get(2, 2), (float)fbxAMatrix.Get(2, 3),
		(float)fbxAMatrix.Get(3, 0), (float)fbxAMatrix.Get(3, 1), (float)fbxAMatrix.Get(3, 2), (float)fbxAMatrix.Get(3, 3)
	);
}

//Fbxデータ内に含まれているテクスチャの情報を入手
FBX_TEXTURE_TYPE FBXDataContainer::GetTextureType(const fbxsdk::FbxBindingTableEntry& entryTable)
{
	std::string texStr = entryTable.GetSource();

	if (texStr == "Maya|DiffuseTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_DIFFUSE;
	}
	else if (texStr == "Maya|NormalTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_NORMAL;
	}
	else if (texStr == "Maya|SpecularTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_SPECUAR;
	}
	else if (texStr == "Maya|FalloffTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_FALLOFF;
	}
	else if (texStr == "Maya|ReflectionMapTexture")
	{
		return FBX_TEXTURE_TYPE::FBX_REFLECTIONMAP;
	}

	return FBX_TEXTURE_TYPE::FBX_UNKNOWN;
}

//Fbxロードに使ったメモリを解放
void FinishFBXLoad(fbxsdk::FbxManager** man, fbxsdk::FbxImporter** imp, fbxsdk::FbxScene** sc)
{

	if ((*imp) != nullptr)
	{
		(*imp)->Destroy();
		*imp = nullptr;
	}

	if ((*sc) != nullptr)
	{
		(*sc)->Destroy();
		*sc = nullptr;
	}

	if ((*man) != nullptr)
	{
		(*man)->Destroy();
		*man = nullptr;
	}

}

//Fbxデータを読み込む
HRESULT FBXDataContainer::ReadFbxToMeshContainer(const std::wstring id, FbxMesh * pMesh)
{
	HRESULT hr = S_OK;

	unique_ptr<MeshContainer> meshCont = make_unique<MeshContainer>();

	//ノード数
	int nodecount = pMesh->GetNodeCount();
	
	//MeshContainerに元データポインタ保存
	meshCont->SetFbxMesh(pMesh);

	//頂点データ本体(ボーン点も含む)
	FbxVector4* controllPoints = nullptr;

	// インデックスデータ
	int* indices = nullptr;

	// 頂点数
	int vertexCount;
	// コントロールポイント数
	int contCount;

	// VertexBuffer
	controllPoints = pMesh->GetControlPoints();
	// IndexBuffer
	indices = pMesh->GetPolygonVertices();
	// 頂点数
	vertexCount = pMesh->GetPolygonVertexCount();
	// コントロールポイント数
	contCount = pMesh->GetControlPointsCount();

	meshCont->m_vertexData.clear();
	meshCont->m_vertexData.resize(vertexCount);

	//メッシュごとの当たり判定エリア値初期化
	meshCont->m_vtxMin.x = FLT_MAX;
	meshCont->m_vtxMin.y = FLT_MAX;
	meshCont->m_vtxMin.z = FLT_MAX;

	meshCont->m_vtxMax.x = FLT_MIN;
	meshCont->m_vtxMax.y = FLT_MIN;
	meshCont->m_vtxMax.z = FLT_MIN;

	for (int i = 0; i < vertexCount; i++)
	{
		// Vertexデータを初期化
		ZeroMemory(&meshCont->m_vertexData[i], sizeof(FbxVertex));

		// インデックスバッファから頂点番号を取得
		int index = indices[i];
		// 頂点座標リストから座標を取得する
		meshCont->m_vertexData[i].position.x = (float)controllPoints[index][0];
		meshCont->m_vertexData[i].position.y = (float)controllPoints[index][1];
		meshCont->m_vertexData[i].position.z = (float)controllPoints[index][2];

		if (meshCont->m_vertexData[i].position.x < meshCont->m_vtxMin.x)
		{
			meshCont->m_vtxMin.x = meshCont->m_vertexData[i].position.x;
		}
		else if (meshCont->m_vertexData[i].position.x > meshCont->m_vtxMax.x)
		{
			meshCont->m_vtxMax.x = meshCont->m_vertexData[i].position.x;
		}

		if (meshCont->m_vertexData[i].position.y < meshCont->m_vtxMin.y)
		{
			meshCont->m_vtxMin.y = meshCont->m_vertexData[i].position.y;
		}
		else if (meshCont->m_vertexData[i].position.y > meshCont->m_vtxMax.y)
		{
			meshCont->m_vtxMax.y = meshCont->m_vertexData[i].position.y;
		}

		if (meshCont->m_vertexData[i].position.z < meshCont->m_vtxMin.z)
		{
			meshCont->m_vtxMin.z = meshCont->m_vertexData[i].position.z;
		}
		else if (meshCont->m_vertexData[i].position.z > meshCont->m_vtxMax.z)
		{
			meshCont->m_vtxMax.z = meshCont->m_vertexData[i].position.z;
		}
	}

	//保存したminとmaxを全体のmin、maxと比較して更新
	if (m_VertexTotalMax.x < meshCont->m_vtxMax.x) m_VertexTotalMax.x = meshCont->m_vtxMax.x;
	if (m_VertexTotalMax.y < meshCont->m_vtxMax.y) m_VertexTotalMax.y = meshCont->m_vtxMax.y;
	if (m_VertexTotalMax.z < meshCont->m_vtxMax.z) m_VertexTotalMax.z = meshCont->m_vtxMax.z;

	if (m_VertexTotalMin.x > meshCont->m_vtxMin.x) m_VertexTotalMin.x = meshCont->m_vtxMin.x;
	if (m_VertexTotalMin.y > meshCont->m_vtxMin.y) m_VertexTotalMin.y = meshCont->m_vtxMin.y;
	if (m_VertexTotalMin.z > meshCont->m_vtxMin.z) m_VertexTotalMin.z = meshCont->m_vtxMin.z;

	FbxStringList uvset_names;
	// UVSetの名前リストを取得
	pMesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;

	// UVSetの名前からUVSetを取得する
	pMesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		FbxVector2& uv = uv_buffer[i];

		//UVデータを自前頂点データに反映 
		meshCont->m_vertexData[i].uv.x = (float)uv[0];
		meshCont->m_vertexData[i].uv.y = (float)(1.0 - uv[1]);
	}


	//indexの作成
	meshCont->m_indexData.clear();
	meshCont->m_indexData.resize(vertexCount);

	//ポリゴン数を取得
	int length = pMesh->GetPolygonCount();
	
	//ポリゴン数でループ
	for (int i = 0; i < length; i++)
	{	
		//インデックスデータの作成 
		int baseIndex = i * 3;
		
		meshCont->m_indexData[baseIndex] = static_cast<ULONG>(baseIndex);
		meshCont->m_indexData[baseIndex + 1] = static_cast<ULONG>(baseIndex + 1);
		meshCont->m_indexData[baseIndex + 2] = static_cast<ULONG>(baseIndex + 2);
	}
	
	//作成したインデックスデータでシェーダリソースを作成
	MyAccessHub::GetMyGameEngine()->GetMeshManager()->AddIndexBuffer(id, meshCont->m_indexData.data(), sizeof(ULONG), vertexCount);

	//法線
	FbxArray<FbxVector4> normals;
	// 法線リストの取得
	pMesh->GetPolygonVertexNormals(normals);

	//法線登録
	for (int i = 0; i < normals.Size(); i++)
	{
		meshCont->m_vertexData[i].normal.x = normals[i][0];
		meshCont->m_vertexData[i].normal.y = normals[i][1];
		meshCont->m_vertexData[i].normal.z = normals[i][2];
	}

	//頂点カラー
	{

		// 頂点カラーデータの数を確認
		int color_count = pMesh->GetElementVertexColorCount();

		// 頂点カラーデータの取得
		FbxGeometryElementVertexColor* vertex_colors = pMesh->GetElementVertexColor(0);

		if (color_count == 0 || vertex_colors == nullptr)
		{
			for (int i = 0; i < vertexCount; i++)
			{
				meshCont->m_vertexData[i].color.x = 1.0f;
				meshCont->m_vertexData[i].color.y = 1.0f;
				meshCont->m_vertexData[i].color.z = 1.0f;
				meshCont->m_vertexData[i].color.w = 1.0f;
			}
		}
		else
		{
			FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
			FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();

			if (mapping_mode == FbxLayerElement::eByPolygonVertex)
			{
				if (reference_mode == FbxLayerElement::eIndexToDirect)
				{
					// 頂点カラーバッファ取得
					FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
					// 頂点カラーインデックスバッファ取得
					FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
					for (int i = 0; i < indeces.GetCount(); i++)
					{
						int id = indeces.GetAt(i);
						FbxColor color = colors.GetAt(id);
						meshCont->m_vertexData[i].color.x = (float)color.mAlpha;
						meshCont->m_vertexData[i].color.y = (float)color.mRed;
						meshCont->m_vertexData[i].color.z = (float)color.mGreen;
						meshCont->m_vertexData[i].color.w = (float)color.mBlue;
					}
				}
			}
		}

	}

	meshCont->m_MeshId = id;

	FbxLayerElementMaterial* elMat = pMesh->GetElementMaterial(0);
	int matIndex = elMat->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* srfMat = pMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(matIndex);
	if (srfMat != nullptr)
	{
		wchar_t namebuff[64] = {};
		auto mtname = srfMat->GetName();
		size_t conv = 0;
		mbstowcs_s(&conv, namebuff, mtname, strlen(mtname));
		meshCont->m_MaterialId = std::wstring(namebuff);
	}
	else
	{
		meshCont->m_MaterialId = L"";
	}

	//頂点数保存
	meshCont->m_vertexCount = vertexCount;
	
	//ボーン、スキンの読み込み
	int skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	if (skinCount > 0)
	{
		meshCont->InitSkinList(skinCount);
		std::vector<std::vector< std::pair<UINT, float>>> tempWeightVect;
		std::vector<std::vector<FbxSkinAnimeParams>> skinWeights;
		skinWeights.resize(skinCount);

		//スキンの数だけループ
		for (int skinloop = 0; skinloop < skinCount; skinloop++)
		{
			//スキンのウェイトデータ保存配列初期化
			tempWeightVect.clear();
			tempWeightVect.resize(contCount); //全コントロールポイント分の影響値スロットを確保

			//スキンを取得
			FbxSkin* pSkin = (FbxSkin*)pMesh->GetDeformer(skinloop, FbxDeformer::eSkin);

			skinWeights[skinloop].clear();
			skinWeights[skinloop].resize(contCount); //WeightデータはVertexではなくControllPoint用

			//スキンの中に幾つのクラスターがあるのかを取得。間接点とその影響点の集合を「クラスター」と呼ぶぞ。
			int clusterCount = pSkin->GetClusterCount();

			//クラスターの数だけループ
			for (int clusterloop = 0; clusterloop < clusterCount; clusterloop++)
			{
				FbxCluster* pCluster = pSkin->GetCluster(clusterloop); //クラスターを一つ取得

				//間接点のID番号を取得。GetClusterId内で新しい間接点だった場合は初期状態の逆行列も保存。
				int clIndex = GetClusterId(pCluster);

				//この間接点が影響を与えるコントロールポイント（頂点）の数
				int pointCnt = pCluster->GetControlPointIndicesCount();

				//この間接点が影響を与えるコントロールポイントのインデックス配列
				int* pPointArray = pCluster->GetControlPointIndices();

				//この間接点が影響を与えるコントロールポイントのウェイト配列、インデックスと組。
				double* pPointWeights = pCluster->GetControlPointWeights();

				//頂点ボーン影響パラメータ読み込み
				for (int pointloop = 0; pointloop < pointCnt; pointloop++)
				{
					//コントロールポイントのインデックス番号
					int ctrlIdx = pPointArray[pointloop];
					//そのコントロールポイントに与えるウェイト値
					double boneWeight = pPointWeights[pointloop];

					tempWeightVect[ctrlIdx].push_back({ clIndex, static_cast<float>(boneWeight) });
				}
			}

			//収集したControllPointごとのWeightデータを頂点用データに成形
			for (int cntLoop = 0; cntLoop < contCount; cntLoop++)
			{
				int wCnt = tempWeightVect[cntLoop].size();						//この頂点に登録された間接点データの数
				FbxSkinAnimeParams* skinparam = &skinWeights[skinloop][cntLoop];//頂点データ用影響度バッファ
				for (; wCnt < 8; wCnt++)
				{
					tempWeightVect[cntLoop].push_back({ 0, 0.0f });
				}
				skinparam->indices0 = { tempWeightVect[cntLoop][0].first, tempWeightVect[cntLoop][1].first,
				tempWeightVect[cntLoop][2].first, tempWeightVect[cntLoop][3].first };
				skinparam->indices1 = { tempWeightVect[cntLoop][4].first, tempWeightVect[cntLoop][5].first,
				tempWeightVect[cntLoop][6].first, tempWeightVect[cntLoop][7].first };
				skinparam->weight0 = { tempWeightVect[cntLoop][0].second, tempWeightVect[cntLoop][1].second,
				tempWeightVect[cntLoop][2].second, tempWeightVect[cntLoop][3].second };
				skinparam->weight1 = { tempWeightVect[cntLoop][4].second, tempWeightVect[cntLoop][5].second,
				tempWeightVect[cntLoop][6].second, tempWeightVect[cntLoop][7].second };
			} 
		}

		//結合したvertexデータを作成
		std::vector<FbxSkinAnimeVertex> skinVertex;
		skinVertex.clear();
		skinVertex.resize(vertexCount);
		meshCont->m_skinParams.resize(vertexCount);	//キャッシュ保存用に領域確保

		for (int i = 0; i < vertexCount; i++)
		{
			int index = indices[i];
			skinVertex[i].vertex = meshCont->m_vertexData[i];
			skinVertex[i].skinvalues = skinWeights[0][index];
			meshCont->m_skinParams[i] = skinWeights[0][index];	//キャッシュ保存用に領域確保
		}
		MyAccessHub::GetMyGameEngine()->GetMeshManager()->
			AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
	}
	else
	{
		if (pMesh->GetScene()->GetSrcObjectCount<FbxMesh>() > 1)
		{
			std::vector<FbxSkinAnimeVertex> skinVertex;
			skinVertex.clear();
			skinVertex.resize(vertexCount);

			UINT boneId = GetClusterId(pMesh->GetNode());
			for (int i = 0; i < vertexCount; i++)
			{
				skinVertex[i].vertex = meshCont->m_vertexData[i];
				skinVertex[i].skinvalues.indices0 = { boneId, 0, 0, 0 };
				skinVertex[i].skinvalues.indices1 = { 0, 0, 0, 0 };
				skinVertex[i].skinvalues.weight0 = { 1.0f, 0, 0, 0 };
				skinVertex[i].skinvalues.weight1 = { 0, 0, 0, 0 };
			}
			MyAccessHub::GetMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
		}
		else
		{
			MyAccessHub::GetMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, meshCont->m_vertexData.data(), sizeof(FbxVertex), vertexCount);
		}
	}

	m_MeshContainer.push_back(move(meshCont));


	return hr;
}

HRESULT FBXDataContainer::LoadMaterial(const std::wstring id, FbxSurfaceMaterial* material)
{
	HRESULT hr = S_OK;

	enum class MaterialOrder
	{
		Ambient,
		Diffuse,
		Specular,
		MaxOrder,
	};

	const fbxsdk::FbxImplementation* implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);

	wchar_t namebuff[64] = {};
	auto mtname = material->GetName();
	size_t conv = 0;
	mbstowcs_s(&conv, namebuff, mtname, strlen(mtname));
	std::wstring matName = namebuff;
	m_MaterialContainer[matName] = make_unique<MaterialContainer>();

	FbxDouble3 colors[(int)MaterialOrder::MaxOrder];
	FbxDouble factors[(int)MaterialOrder::MaxOrder];
	FbxProperty fbxProp = material->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		const char* element_check_list[] =
		{
			FbxSurfaceMaterial::sAmbient,
			FbxSurfaceMaterial::sDiffuse,
		};

		const char* factor_check_list[] =
		{
			FbxSurfaceMaterial::sAmbientFactor,
			FbxSurfaceMaterial::sDiffuseFactor,
		};

		for (int i = 0; i < 2; i++)
		{
			fbxProp = material->FindProperty(element_check_list[i]);
			if (fbxProp.IsValid())
			{
				colors[i] = fbxProp.Get<FbxDouble3>();

			}
			else
			{
				colors[i] = FbxDouble3(1.0, 1.0, 1.0);
			}

			fbxProp = material->FindProperty(factor_check_list[i]);
			if (fbxProp.IsValid())
			{
				factors[i] = fbxProp.Get<FbxDouble>();
			}
			else
			{
				factors[i] = 1.0;
			}
		}
	}

	FbxDouble3 color = colors[(int)MaterialOrder::Ambient];
	FbxDouble factor = factors[(int)MaterialOrder::Ambient];
	m_MaterialContainer[matName]->SetAmbient((float)color[0], (float)color[1], (float)color[2], (float)factor);

	color = colors[(int)MaterialOrder::Diffuse];
	factor = factors[(int)MaterialOrder::Diffuse];
	m_MaterialContainer[matName]->SetDiffuse((float)color[0], (float)color[1], (float)color[2], (float)factor);

	// テクスチャ読み込み(シングル対応)
	// マルチテクスチャはシェーダから別。今回はそこら辺は未対応

	if (implementation == nullptr)
	{
		fbxProp = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		LoadTextureFromMaterial(matName, id, FBX_TEXTURE_TYPE::FBX_DIFFUSE, &fbxProp);
	}
	else
	{
		const FbxBindingTable* rootTable = implementation->GetRootTable();
		size_t entryCount = rootTable->GetEntryCount();

		for (int ent = 0; ent < entryCount; ent++)
		{
			const FbxBindingTableEntry entry = rootTable->GetEntry(ent);

			fbxProp = material->FindPropertyHierarchical(entry.GetSource());
			if (!fbxProp.IsValid())
			{
				fbxProp = material->RootProperty.FindHierarchical(entry.GetSource());
			}

			LoadTextureFromMaterial(matName, id, GetTextureType(entry), &fbxProp);
		}
	}

	return hr;
}

//マテリアルからテクスチャを読み込む
HRESULT FBXDataContainer::LoadTextureFromMaterial(const std::wstring matName, const std::wstring id, FBX_TEXTURE_TYPE texType, const fbxsdk::FbxProperty* fbxProp)
{
	HRESULT hr = S_OK;
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();

	fbxsdk::FbxFileTexture* texture = nullptr;
	std::string keyword;
	int numOfTex = fbxProp->GetSrcObjectCount<FbxFileTexture>();
	if (numOfTex > 0)
	{
		numOfTex = 1;
	}
	else
	{
		numOfTex = fbxProp->GetSrcObjectCount<FbxLayeredTexture>();
	}

	TextureManager* texMng = MyAccessHub::GetMyGameEngine()->GetTextureManager();
	for (int i = 0; i < numOfTex; i++)
	{
		texture = fbxProp->GetSrcObject<FbxFileTexture>(i);
		if (texture != nullptr)
		{
			std::string filePath = texture->GetRelativeFileName();

			std::string::size_type position(filePath.find('\\'));
			std::string fileName;

			while (position != std::string::npos)
			{
				filePath.replace(position, 1, "/");
				position = filePath.find('\\', position + 1);
			}

			std::string::size_type offset = std::string::size_type(0);
			position = filePath.find('/');

			while (position != std::string::npos)
			{
				offset = position + 1;
				position = filePath.find('/', offset);
			}

			fileName = filePath.substr(offset);

			
			position = fileName.find(".psd");
			if (position != std::string::npos)
			{
				while (position != std::string::npos)
				{
					offset = position;
					position = fileName.find(".psd", position + 4);
				}
				fileName.replace(offset, 4, ".tga");
			}

			wchar_t namebuff[64] = {};
			wchar_t texturePath[128];
			size_t conv = 0;
			mbstowcs_s(&conv, namebuff, fileName.c_str(), fileName.length());

			wsprintfW(texturePath, L"Resources/textures/%ls/%ls", id.c_str(), namebuff);	//Resources/textures/3Dモデルの名前/テクスチャの名前

			wchar_t idName[128];
			wsprintfW(idName, L"%ls_%ls", id.c_str(), namebuff);
			hr = texMng->CreateTextureFromFile(engine->GetDirect3DDevice(), idName, texturePath);

			if (FAILED(hr))
			{
				break;
			}

			switch (texType)
			{
			case FBX_TEXTURE_TYPE::FBX_DIFFUSE:
				m_MaterialContainer[matName]->m_DiffuseTextures.push_back(idName);
				m_MaterialContainer[matName]->m_DiffusePath.push_back(texturePath);
				break;

			case FBX_TEXTURE_TYPE::FBX_NORMAL:
				m_MaterialContainer[matName]->m_NormalTextures.push_back(idName);
				m_MaterialContainer[matName]->m_NormalPath.push_back(texturePath);
				break;

			case FBX_TEXTURE_TYPE::FBX_SPECUAR:
				m_MaterialContainer[matName]->m_SpecularTextures.push_back(idName);
				m_MaterialContainer[matName]->m_SpecularPath.push_back(texturePath);
				break;

			case FBX_TEXTURE_TYPE::FBX_FALLOFF:
				m_MaterialContainer[matName]->m_FalloffTextures.push_back(idName);
				m_MaterialContainer[matName]->m_FalloffPath.push_back(texturePath);
				break;

			case FBX_TEXTURE_TYPE::FBX_REFLECTIONMAP:
				m_MaterialContainer[matName]->m_ReflectionMapTextures.push_back(idName);
				m_MaterialContainer[matName]->m_ReflectionMapPath.push_back(texturePath);
				break;

			default:	//Unknown
				break;
			}
		}
	}

	return hr;
}

//Fbx読み込み
HRESULT FBXDataContainer::LoadFBX(const std::wstring fileName, const std::wstring id)
{
	HRESULT hr = S_OK;

	fs::path cacheDir = L"Cache";					
	fs::create_directories(cacheDir);				//キャッシュがあるかを検索するファイルを指定
	fs::path cachePath = cacheDir / (id + L".bin");	//キャッシュの名前をIDから指定

	//==========FbxSDKの初期化とインポート　Fbxファイルを開くための準備==========

	fbxsdk::FbxManager* fbx_manager = nullptr;
	FbxScene* fbx_scene = nullptr;
	FbxImporter* fbx_importer = nullptr;
	FbxNode* fbx_node = nullptr;

	char* c_filename = nullptr;
	int wcSize = sizeof(wchar_t) * wcslen(fileName.c_str()) + 1;

	c_filename = new char[wcSize];									//FbxSDKが受け取れるよう、char*型に変換

	size_t retVal = 0;
	wcstombs_s(&retVal, c_filename, wcSize, fileName.c_str(), wcSize);	//c_filenameにfileNameをchar*型で保存

	fbx_manager = FBXDataContainer::GetFbxManager();					//マネージャ初期化

	fbx_importer = FbxImporter::Create(fbx_manager, c_filename);		//インポータ初期化

	if (fbx_importer == nullptr)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}

	fbx_scene = FbxScene::Create(fbx_manager, c_filename);				//シーン初期化

	if (fbx_scene == nullptr)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}

	bool res = fbx_importer->Initialize(c_filename);					//ファイル名のファイルが存在しているか

	delete[] c_filename;												//メモリ開放

	if (res == false)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}

	if (fbx_importer->Import(fbx_scene) == false)
	{
		hr = E_FAIL;
		FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
		return hr;
	}

	//==========FbxSDKの初期化とインポート　Fbxファイルを開くための準備==========End


	m_FbxScene = fbx_scene; // シーンを保持

	auto AnimationInfo = [&](FbxScene* scene, FbxImporter* importer)
	{
		int animStackCount = importer->GetAnimStackCount();				//アニメーションの種類を取得

		if (animStackCount > 0)											//アニメーションがある場合
		{
			FbxAnimStack* stack = scene->GetCurrentAnimationStack();	//シーンに含まれる現在のアニメスタック取得
			if (stack == nullptr)
			{
				stack = scene->GetSrcObject<FbxAnimStack>(0);			//もし現在のスタックが定義されてなかったら最初のスタックを取得	
			}

			if (stack)
			{
				m_AnimeStack = stack;
				scene->SetCurrentAnimationStack(m_AnimeStack);			//Fbxシーンでのアニメスタックをセット

				FbxTimeSpan timeSpan = stack->GetLocalTimeSpan();		//アニメ時間の取得
				m_StartTime = timeSpan.GetStart().GetSecondDouble();	//開始時間
				m_EndTime = timeSpan.GetStop().GetSecondDouble();		//終了時間

				if (m_EndTime <= 0.001 || (m_StartTime == 0.0 && m_EndTime == 5.0))		//アニメタイムが初期値、もしくは標準の5秒だった場合、もう一度取得
				{
					FbxTakeInfo* takeInfo = scene->GetTakeInfo(stack->GetName());
					if (takeInfo) {
						m_StartTime = takeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
						m_EndTime = takeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
					}
				}

				m_TimePeriod = 1.0f / 60.0f;							
				m_AnimeFrames = floorl((m_EndTime - m_StartTime) / m_TimePeriod);	//1フレームの長さを1/60秒に設定

				if (m_AnimeFrames <= 0)	
				{
					m_AnimeFrames = 1;
				}
			}
		}
	};

	AnimationInfo(fbx_scene, fbx_importer);

	FbxAxisSystem dx = FbxAxisSystem::DirectX;		//軸変換のため軸を初期化

	if (fs::exists(cachePath))						//Cacheフォルダ内にid.binが存在するか確認
	{
		HRESULT cacheRes = LoadBinary(cachePath);
		if (SUCCEEDED(cacheRes))					//キャッシュロード成功。
		{
			if (m_AnimeStack == nullptr)			//アニメーションスタックが正しく取得できているか再確認
			{
				m_AnimeStack = fbx_scene->GetSrcObject<FbxAnimStack>(0);
				fbx_scene->SetCurrentAnimationStack(m_AnimeStack);
				m_CurrentAnimeCont = this;
			}

			if (dx != fbx_scene->GetGlobalSettings().GetAxisSystem())	//キャッシュがある時用の座標転換
			{
				dx.DeepConvertScene(fbx_scene);
			}

			//ここでインポータを消すと、シーン内のオブジェクトが不安定になる場合があるため
			//解析フェーズをスキップして終了
			fbx_importer->Destroy();

			return S_OK;
		}
		else
		{
			fs::remove(cachePath); //壊れているか古い場合は削除
		}
	}


	//==========DirectXに適応するために座標系を変換==========

	
	if (dx != fbx_scene->GetGlobalSettings().GetAxisSystem())
	{
		dx.DeepConvertScene(fbx_scene);
	}

	//==========DirectXに適応するために座標系を変換==========End


	//==========ノード情報を収集&ポリゴンの最適化==========

	fbx_node = fbx_scene->GetRootNode();
	
	if (fbx_node != nullptr)
	{
		int nodes = fbx_scene->GetNodeCount();
		m_NodeNameList.clear();
		m_NodeNameList.resize(nodes);
		for (int i = 0; i < nodes; i++)
		{
			m_NodeNameList[i] = fbx_scene->GetNode(i)->GetName();
		}

		FbxGeometryConverter converter(fbx_manager);
		converter.SplitMeshesPerMaterial(fbx_scene, true);
		converter.Triangulate(fbx_scene, true);

		//==========アニメーション情報の取得===========

		if (fbx_importer->GetAnimStackCount() > 0)
		{
			auto stack = fbx_scene->GetCurrentAnimationStack();
			auto name = stack->GetName();
			auto memberCount = stack->GetMemberCount<FbxAnimCurveNode>();
			auto fbxAnimeLayer = stack->GetMember<FbxAnimLayer>(0);

			m_StartTime = stack->GetLocalTimeSpan().GetStart().GetSecondDouble();
			m_EndTime = stack->GetLocalTimeSpan().GetStop().GetSecondDouble();
			m_TimePeriod = 1.0 / 60.0;
			m_AnimeFrames = floorl((m_EndTime - m_StartTime) / m_TimePeriod);

			m_AnimeStack = stack; 
			fbx_scene->SetCurrentAnimationStack(m_AnimeStack);
		}
		else
		{
			m_AnimeStack = nullptr;
			m_AnimeFrames = 0;
		}

		//==========アニメーション情報の取得===========End



		//==========マテリアル・メッシュの読み込む==========

		m_MeshContainer.clear();

		int materialCnt = fbx_scene->GetMaterialCount();

		for (int i = 0; i < materialCnt; i++)
		{
			LoadMaterial(id, fbx_scene->GetSrcObject<FbxSurfaceMaterial>(i));
		}

		//メッシュ読み込み処理
		int meshCount = fbx_scene->GetSrcObjectCount<FbxMesh>();

		m_VertexTotalMax.x = FLT_MIN;
		m_VertexTotalMax.y = FLT_MIN;
		m_VertexTotalMax.z = FLT_MIN;

		m_VertexTotalMin.x = FLT_MAX;
		m_VertexTotalMin.y = FLT_MAX;
		m_VertexTotalMin.z = FLT_MAX;

		wchar_t idName[128];
		for (int i = 0; i < meshCount; i++)
		{
			wsprintfW(idName, L"%ls_%02d", id.c_str(), i);
			if ( FAILED( ReadFbxToMeshContainer(idName, fbx_scene->GetSrcObject<FbxMesh>(i)) ) )
			{
				hr = E_FAIL;

				FinishFBXLoad(&fbx_manager, &fbx_importer, &fbx_scene);
				return hr;
			}
		}

		//==========マテリアル・メッシュの読み込む==========End


		//==========ボーン(スケルトン)の準備==========

		m_ClusterCount = m_BoneNameList.size();
		if (m_ClusterCount > 0)
		{
			m_BoneIdList.resize(m_ClusterCount); 
			m_F4X4Matrix.resize(m_ClusterCount);
		}

		//==========ボーン(スケルトン)の準備==========End


		m_FbxScene = fbx_scene;

		SaveBinary(cachePath);
	}

	fbx_importer->Destroy();

	if (SUCCEEDED(hr))
	{
		MyAccessHub::GetMyGameEngine()->UploadCreatedTextures();
	}

	return hr;
}

//==========FBXのリソース（頂点バッファなど）をインスタンス固有のものとして扱い、そのインスタンスが破棄される際に自動的にグラフィックスメモリ上のリソースも削除するように設定する==========
void FBXDataContainer::SetMeshUniqueFlag(bool meshFlag, bool materialFlag)
{
	int length = m_MeshContainer.size();

	for (int id = 0; id < length; id++)
	{
		if (m_MeshContainer[id] != nullptr)
		{
			m_MeshContainer[id]->SetUniqueFlag(meshFlag);
		}
	}
}

//==========FBXが使用しているテクスチャリソースをインスタンス固有のものとして扱い、そのインスタンスが破棄される際に自動的にテクスチャをメモリから削除するように設定する==========
void FBXDataContainer::SetTextureUniqueFlag(bool texFlag)
{
	for (auto ite = m_MaterialContainer.begin(); ite != m_MaterialContainer.end(); ite++)
	{
		ite->second->SetUniqueTextureFlag(texFlag);
	}
}

//==========Fbxのノード名から入手==========
int FBXDataContainer::GetNodeId(const char* nodeName)
{
	int length = m_NodeNameList.size();
	for (int i = 0; i < length; i++)
	{
		if (strcmp(m_NodeNameList[i].c_str(), nodeName) == 0)
		{
			return i;
		}
	}

	std::string prefix = "mixamorig:";
	std::string baseTarget = nodeName;
	if (baseTarget.find(prefix) == 0)
	{
		baseTarget = baseTarget.substr(prefix.length());
	}

	for (int i = 0; i < length; i++) 
	{
		std::string baseCurrent = m_NodeNameList[i];
		if (baseCurrent.find(prefix) == 0) baseCurrent = baseCurrent.substr(prefix.length());
		if (baseTarget == baseCurrent) return i;
	}
	
	return -1;
}

//==========Fbxのメッシュ名から入手==========
int FBXDataContainer::GetMeshId(const char* meshName)
{
	int len = m_MeshContainer.size();

	for (int i = 0; i < len; i++)
	{
		if (strcmp(m_MeshContainer[i]->GetMeshNodeName().c_str(), meshName) == 0)	//string型に変更したため修正
		{
			return i;
		}
	}

	return -1;
}

//==========FbxのIDからノードを入手==========
FbxNode* FBXDataContainer::GetMeshNode(int id)
{
	if (id < m_MeshContainer.size())
	{
		return m_MeshContainer[id]->GetFbxMesh()->GetNode();
	}
	return nullptr;
}

//==========Fbxをファイル名からロード(アニメ無し)==========
HRESULT FBXCharacterData::LoadMainFBX(const std::wstring fileName, const std::wstring id)
{
	HRESULT hr = MyAccessHub::GetFBXResourceManager()->GetorLoadFbx(fileName, id, m_MainFbx);
	if (SUCCEEDED(hr))
	{
		m_constantBuffers.clear();
		m_cbuffCount = 0;
		AddConstantBuffer(sizeof(XMMATRIX), nullptr);

		if (m_MainFbx->GetClusterCount() > 0) //スキンアニメ有り
		{
			int curCbuff = m_cbuffCount;													//AddConstantBufferの中でm_cbuffCountが加算されるので先に
			AddConstantBuffer(sizeof(XMFLOAT4X4) * m_MainFbx->GetClusterCount(), nullptr);
			m_MainFbx->SetCBuffIndex(curCbuff);												//アニメ用コンスタントバッファのインデックスを登録
		}

	}

	return hr;
}

//==========Fbxをファイル名からロード(アニメ有り)==========
HRESULT FBXCharacterData::LoadAnimationFBX(const std::wstring fileName, const std::wstring id)
{
	//このエンジン用のデータクラスであるFBXDataContainerを作成
	std::unique_ptr<FBXDataContainer> fbxCon = make_unique<FBXDataContainer>();
	HRESULT res = fbxCon->LoadFBX(fileName, id); //FBXを読み込む。ここで分解等は完了
	if (SUCCEEDED(res))
	{
		if (m_AnimeFbxMap[id] != nullptr) //IDで管理するのでもうデータが入っている場合は前のデータを削除
		{
			m_AnimeFbxMap[id].release();
		}
		m_AnimeFbxMap[id] = move(fbxCon);
	}
	return res;
}

//ラベル名からアニメ用FBXDataContainerを取得
FBXDataContainer* FBXCharacterData::GetAnimeFbx(const std::wstring fileName)
{
	if (m_AnimeFbxMap.find(fileName) == m_AnimeFbxMap.end())
		return nullptr;
	return m_AnimeFbxMap[fileName].get();
}

void FBXCharacterData::SetAnime(std::wstring animeLabel) {
	if (animeLabel != m_CurrentAnimeLabel) {
		if (m_AnimeFbxMap[animeLabel] != nullptr) {
			FBXDataContainer* animeCont = m_AnimeFbxMap[animeLabel].get(); //アニメFBX
			FBXDataContainer* mainCont = m_MainFbx.get(); //メインFBX
			MeshContainer* meshCont = nullptr; //メインからのMeshContainer取り出し用
			int boneCount = mainCont->GetBoneNameList().size();

			m_BoneIdList.resize(boneCount);		//インスタンスごとのボーンリストを初期化

			for (int i = 0; i < boneCount; i++)
			{
				m_BoneIdList[i] = animeCont->GetNodeId(mainCont->GetBoneNameList()[i].c_str());
			}

			for (int i = 0; (meshCont = mainCont->GetMeshContainer(i)) != nullptr; i++) 
			{
				meshCont->SetBoneIdList(animeCont); //メインFBXの全メッシュのボーンIDリスト更新
			}

			m_CurrentAnimeLabel = animeLabel; //二重再生防止用。アニメラベル登録

			m_AnimeTime = 0; //再生時間０

			//FbxSceneにアニメFBXから取り出したFbxAnimeStackをセット。再生可能状態に。
			animeCont->GetFbxScene()->SetCurrentAnimationStack(animeCont->GetAnimeStack());

			mainCont->SetAnimationFbx(animeCont); //メインFBXにアニメーションFBXをセット

			UpdateAnimation(0); //アニメーションを先頭フレームに
		}
	}
}

//アニメーション更新
void FBXCharacterData::UpdateAnimation()
{
	FBXDataContainer* animeCont = m_AnimeFbxMap[m_CurrentAnimeLabel].get(); //再生中のFBXDataContainer取得
	assert(animeCont);							//animeContがnullptrならエラー
	int frames = animeCont->GetAnimeFrames();	//再生位置のフレーム取得
	if (m_AnimeTime < frames)					//まだ終了位置でないなら
	{
		m_AnimeTime++; //フレーム加算
	}
	else
	{
		m_AnimeTime -= frames;
	}
	UpdateAnimation(m_AnimeTime);
}

void FBXCharacterData::UpdateAnimation(int frameCount)
{
	FBXDataContainer* animeCont = m_AnimeFbxMap[m_CurrentAnimeLabel].get();
	FBXDataContainer* mainCont = m_MainFbx.get();

	double nowTime = animeCont->GetPeriodTime() * frameCount; //フレーム数から実時間を計算
	if (nowTime > animeCont->GetEndTime())
	{
		nowTime = nowTime - animeCont->GetEndTime();
	}
	FbxTime currentTime; 
	currentTime.SetSecondDouble(nowTime); 

	mainCont->UpdateAnimation(animeCont, currentTime, m_AnimatedMatrix, m_NodeMatrices, m_BoneIdList); 
}

//アニメーションを呼ぶwstringを統一するためにSetAnimeを呼ぶ前段階
void FBXCharacterData::SetAnimeInit(std::wstring initAnimeLabel, FieldCharacter* chara)
{
	std::wstring adminString = (chara->CharaAdmin == Admin::Rebel) ? L"_REBEL" : L"_IMPER";
	std::wstring typeString = L"";

	switch (chara->CharaKind)
	{
	default:
		break;
	case SoldiersType::infantry:
		typeString = L"_INF";
		break;
	case SoldiersType::machinegunner:
		typeString = L"_MGN";
		break;
	case SoldiersType::scout:
		typeString = L"_SCT";
		break;
	}

	if (!typeString.empty())
	{
		SetAnime(initAnimeLabel + adminString + typeString);
	}
}

XMMATRIX FBXDataContainer::GetBornMatrix(const char* bornName)
{
	if (m_CurrentAnimeCont == nullptr)
	{
		return DirectX::XMMatrixIdentity();
	}
	FbxScene* animeScene = m_CurrentAnimeCont->GetFbxScene();
	XMMATRIX xMMatrix;
	
	// アニメーション側のシーンでのノードIDを取得する必要がある
	int bornId = m_CurrentAnimeCont->GetNodeId(bornName);
	if (bornId != -1)
	{
		auto fbxNode = animeScene->GetNode(bornId);
		FbxAMatrix matrix = fbxNode->EvaluateGlobalTransform(m_FbxTime);
		ConvertFbxAMatrixToXMMATRIX(matrix, xMMatrix);
		return xMMatrix;
	}

	return DirectX::XMMatrixIdentity();
}

MeshContainer::~MeshContainer()
{
	m_vertexData.clear();
	m_indexData.clear();
	
	m_skinCount = 0;

	if (m_uniqueMesh)
	{
		auto meshMng = MyAccessHub::GetMyGameEngine()->GetMeshManager();
		meshMng->removeVertexBuffer(m_MeshId, true);
	}
}

void MeshContainer::SetFbxMesh(FbxMesh* mesh)
{
	m_mesh = mesh;
	m_meshNodeName = m_mesh->GetNode(0)->GetName();
	
	FbxTime timeZero;
	timeZero.SetFrame(0, FbxTime::EMode::eFrames60);
	
	m_meshNodeName = m_mesh->GetNode(0)->GetName();

	m_IBaseMatrix = m_mesh->GetNode(0)->EvaluateGlobalTransform(timeZero).Inverse();
}

void MeshContainer::InitSkinList(int skinCount)
{
	m_skinCount = skinCount;
}
void MeshContainer::SetBoneIdList(FBXDataContainer* animeFbxCont)
{
	m_animeFbxCont = animeFbxCont;
	m_parentNodeId = m_animeFbxCont->GetMeshId(m_meshNodeName.c_str());		//string型に変更したため修正
}


MaterialContainer::~MaterialContainer()
{
	if (m_uniqueTextures)
	{
		TextureManager* texMng = MyAccessHub::GetMyGameEngine()->GetTextureManager();
		for (auto id : m_DiffuseTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_NormalTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_SpecularTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_FalloffTextures)
		{
			texMng->ReleaseTexture(id);
		}

		for (auto id : m_ReflectionMapTextures)
		{
			texMng->ReleaseTexture(id);
		}

	}

	m_DiffuseTextures.clear();
	m_NormalTextures.clear();
	m_SpecularTextures.clear();
	m_FalloffTextures.clear();
	m_ReflectionMapTextures.clear();
}

int FBXDataContainer::GetClusterId(FbxCluster* pCluster)
{
	return GetClusterId(pCluster->GetLink());
}

int FBXDataContainer::GetClusterId(FbxNode* pNode)
{
	int size = m_BoneNameList.size();			//今のクラスターIDリストのサイズ取得
	const char* nodeName = pNode->GetName();	//ノード名取得
	for (int id = 0; id < size; id++)
	{
		if (strcmp(nodeName, m_BoneNameList[id].c_str()) == 0)
			return id;							//ノード名がリストに登録されていればそれがID
	}
	
	m_BoneNameList.push_back(nodeName);

	FbxAMatrix inv = pNode->EvaluateGlobalTransform().Inverse();	//一旦行列を保持
	XMFLOAT4X4 xmInv;	//型変更用

	ConvertFbxAMatrixToXMFLOAT4x4(inv, xmInv);

	m_IBoneMatrix.push_back(xmInv);

	return size; 
}

void FBXDataContainer::SetAnimationFbx(FBXDataContainer* animeCont)
{
	if (m_CurrentAnimeCont != animeCont) 
	{
		m_CurrentAnimeCont = animeCont;
		
		int len = m_BoneNameList.size();
		for (int i = 0; i < len; i++)
		{
			m_BoneIdList[i] = m_CurrentAnimeCont->GetNodeId(m_BoneNameList[i].c_str());
		}
	}
}

void FBXDataContainer::UpdateAnimation(FBXDataContainer* animeCont, const FbxTime& animeTime, vector<XMFLOAT4X4>& chDataMatrix, vector<XMFLOAT4X4>& nodeMatrices, const vector<int>& bornIdList)	//アニメデータの更新処理　※※仕様変更04/17　FbxCharacterDataが行列を独自で持つようになったためそれを宣言するように。
{
	m_FbxTime = animeTime;
	int boneCount = m_BoneNameList.size();
	FbxScene* animeScene = animeCont->GetFbxScene();

	//インスタンスごとに違うシーンを呼ぶために自身のシーンを付与
	if (animeScene->GetCurrentAnimationStack() != animeCont->GetAnimeStack())
	{
		animeScene->SetCurrentAnimationStack(animeCont->GetAnimeStack());
	}

	//各キャラクターのFbxCharacterDataが持つ変数とContainer側の変数のサイズを合わせる
	if (boneCount != chDataMatrix.size())
	{
		chDataMatrix.resize(boneCount);
	}

	//ノードのサイズも同様に
	if (boneCount != nodeMatrices.size())
	{
		nodeMatrices.resize(boneCount);
	}

	for (int i = 0; i < boneCount; i++)
	{
		int animeNodeId = bornIdList[i];

		//このフレームでのアニメーションボーンを取得し、XMMatrixに変換
		if (animeNodeId != -1)
		{
			FbxNode* fbxNode = animeScene->GetNode(animeNodeId);					//ノード取得
			FbxAMatrix fbxAMatrix = fbxNode->EvaluateGlobalTransform(animeTime);	//animeTimeにおけるNodeの位置情報を取得

			FbxAMatrix fbxInvBind;													//AMatrixにバインド
			for (int r = 0; r < 4; r++)
			{
				for (int c = 0; c < 4; c++)
				{
					fbxInvBind[r][c] = m_IBoneMatrix[i].m[r][c];
				}
			}

			ConvertFbxAMatrixToXMFLOAT4x4(fbxAMatrix, nodeMatrices[i]);					//モノを持たせる為に行列を挿入

			FbxAMatrix finalFbxMatrix = fbxAMatrix * fbxInvBind;						//逆行列を掛け算

			ConvertFbxAMatrixToXMFLOAT4x4(finalFbxMatrix, chDataMatrix[i]);				//XMMatrixに変換し、DirectXで使用可能に
		}
		else
		{
			//見つからない場合は単位行列・下半身がねじれる現象を解決
			XMStoreFloat4x4(&chDataMatrix[i], XMMatrixIdentity());
			XMStoreFloat4x4(&nodeMatrices[i], XMMatrixIdentity());
		}
	}
}

const XMFLOAT4X4* FBXDataContainer::GetAnimatedMatrix()
{
	return m_F4X4Matrix.data();
}

HRESULT FBXDataContainer::LoadBinary(const fs::path& path)
{
	std::ifstream ifs(path.wstring().c_str(), std::ios::binary);
	if (!ifs) return E_FAIL;

	int version = 0;
	ReadBinary(ifs, version);
	if (version != 2) return E_FAIL; //バージョン不一致ならキャッシュ破棄・再生成へ

	//頂点データの最大値・最小値
	ReadBinary(ifs, m_VertexTotalMin);
	ReadBinary(ifs, m_VertexTotalMax);

	//アニメーション時間設定
	ReadBinary(ifs, m_AnimeFrames);
	ReadBinary(ifs, m_StartTime);
	ReadBinary(ifs, m_EndTime);
	ReadBinary(ifs, m_TimePeriod);

	//クラスター/ボーン情報
	ReadBinary(ifs, m_ClusterCount);
	ReadBinary(ifs, m_ConstantBufferIndex);

	size_t boneCnt;
	ReadBinary(ifs, boneCnt);
	m_BoneNameList.resize(boneCnt);
	for (auto& name : m_BoneNameList) ReadString(ifs, name);

	ReadVector(ifs, m_BoneIdList);

	ReadVector(ifs, m_IBoneMatrix);

	m_F4X4Matrix.resize(m_IBoneMatrix.size());

	size_t nodeCnt;
	ReadBinary(ifs, nodeCnt);
	m_NodeNameList.resize(nodeCnt);
	for (auto& name : m_NodeNameList) ReadString(ifs, name);

	// マテリアル
	size_t matCnt;
	ReadBinary(ifs, matCnt);
	for (size_t i = 0; i < matCnt; i++)
	{
		std::wstring matId;
		ReadWString(ifs, matId);
		auto matCont = make_unique<MaterialContainer>();
		ifs.read(reinterpret_cast<char*>(matCont->ambient), sizeof(float) * 4);
		ifs.read(reinterpret_cast<char*>(matCont->diffuse), sizeof(float) * 4);
		ifs.read(reinterpret_cast<char*>(matCont->specular), sizeof(float) * 4);
		ReadBinary(ifs, matCont->alpha);

		// テクスチャ名リストを読み込み
		auto ReadTexureNameList = [&](std::vector<std::wstring>& list)
		{
			size_t size;
			ReadBinary(ifs, size);
			list.resize(size);
			for (auto& s : list) ReadWString(ifs, s);
		};
		ReadTexureNameList(matCont->m_DiffuseTextures);
		ReadTexureNameList(matCont->m_NormalTextures);
		ReadTexureNameList(matCont->m_SpecularTextures);
		ReadTexureNameList(matCont->m_FalloffTextures);
		ReadTexureNameList(matCont->m_ReflectionMapTextures);

		// テクスチャパスリストを読み込み
		auto ReadTexturePathList = [&](std::vector<std::wstring>& list)
		{
			size_t size;
			ReadBinary(ifs, size);
			list.resize(size);
			for (auto& s : list) ReadWString(ifs, s);
		};
		ReadTexturePathList(matCont->m_DiffusePath);
		ReadTexturePathList(matCont->m_NormalPath);
		ReadTexturePathList(matCont->m_SpecularPath);
		ReadTexturePathList(matCont->m_FalloffPath);
		ReadTexturePathList(matCont->m_ReflectionMapPath);

		for (size_t i = 0; i < matCont->m_DiffuseTextures.size(); i++)
		{
			auto device = MyAccessHub::GetMyGameEngine()->GetDirect3DDevice();
			MyAccessHub::GetMyGameEngine()->GetTextureManager()->CreateTextureFromFile(device, matCont->m_DiffuseTextures[i].c_str(), matCont->m_DiffusePath[i].c_str());
		}

		for (size_t i = 0; i < matCont->m_NormalTextures.size(); i++)
		{
			auto device = MyAccessHub::GetMyGameEngine()->GetDirect3DDevice();
			MyAccessHub::GetMyGameEngine()->GetTextureManager()->CreateTextureFromFile(device, matCont->m_NormalTextures[i].c_str(), matCont->m_NormalPath[i].c_str());
		}

		for (size_t i = 0; i < matCont->m_SpecularTextures.size(); i++)
		{
			auto device = MyAccessHub::GetMyGameEngine()->GetDirect3DDevice();
			MyAccessHub::GetMyGameEngine()->GetTextureManager()->CreateTextureFromFile(device, matCont->m_SpecularTextures[i].c_str(), matCont->m_SpecularPath[i].c_str());
		}

		for (size_t i = 0; i < matCont->m_FalloffTextures.size(); i++)
		{
			auto device = MyAccessHub::GetMyGameEngine()->GetDirect3DDevice();
			MyAccessHub::GetMyGameEngine()->GetTextureManager()->CreateTextureFromFile(device, matCont->m_FalloffTextures[i].c_str(), matCont->m_FalloffPath[i].c_str());
		}

		for (size_t i = 0; i < matCont->m_ReflectionMapTextures.size(); i++)
		{
			auto device = MyAccessHub::GetMyGameEngine()->GetDirect3DDevice();
			MyAccessHub::GetMyGameEngine()->GetTextureManager()->CreateTextureFromFile(device, matCont->m_ReflectionMapTextures[i].c_str(), matCont->m_ReflectionMapPath[i].c_str());
		}

		m_MaterialContainer[matId] = move(matCont);
	}

	// メッシュ
	size_t meshCnt;
	ReadBinary(ifs, meshCnt);
	for (size_t i = 0; i < meshCnt; i++)
	{
		auto meshCont = make_unique<MeshContainer>();
		ReadString(ifs, meshCont->GetMeshNodeName());
		ReadBinary(ifs, meshCont->GetParentNodeId());

		XMFLOAT4X4 xmBase;
		ReadBinary(ifs, xmBase);
		FbxAMatrix baseMatrix;
		for (int raw = 0; raw < 4; raw++)
		{
			for (int column = 0; column < 4; column++)
			{
				baseMatrix[raw][column] = xmBase.m[raw][column];	//SDKクラスに復元
			}
		}

		meshCont->SetIBaseMatrix(baseMatrix);

		ReadBinary(ifs, meshCont->GetSkinCount());
		ReadWString(ifs, meshCont->m_MaterialId);
		ReadWString(ifs, meshCont->m_MeshId);
		ReadBinary(ifs, meshCont->m_vertexCount);
		ReadVector(ifs, meshCont->m_vertexData);
		ReadVector(ifs, meshCont->m_indexData);
		ReadVector(ifs, meshCont->m_skinParams); // スキン情報ロード
		ReadBinary(ifs, meshCont->m_vtxMin);
		ReadBinary(ifs, meshCont->m_vtxMax);
		ReadBinary(ifs, meshCont->GetIBaseMatrix());

		// メッシュリソースをGPUに登録 (LoadFBX内で行っているのと同様)
		MyAccessHub::GetMyGameEngine()->GetMeshManager()->AddIndexBuffer(meshCont->m_MeshId, meshCont->m_indexData.data(), sizeof(ULONG), (UINT)meshCont->m_indexData.size());

		if (meshCont->GetSkinCount() > 0 || meshCont->m_skinParams.size() > 0)
		{
			// スキンアニメーション用頂点バッファの構築
			std::vector<FbxSkinAnimeVertex> skinVertex(meshCont->m_vertexCount);
			for (UINT v = 0; v < meshCont->m_vertexCount; ++v)
			{
				skinVertex[v].vertex = meshCont->m_vertexData[v];
				if (meshCont->m_skinParams.size() > v)
				{
					skinVertex[v].skinvalues = meshCont->m_skinParams[v];
				}
			}
			MyAccessHub::GetMyGameEngine()->GetMeshManager()->AddVertexBuffer(meshCont->m_MeshId, skinVertex.data(), sizeof(FbxSkinAnimeVertex), meshCont->m_vertexCount);
		}
		else
		{
			MyAccessHub::GetMyGameEngine()->GetMeshManager()->AddVertexBuffer(meshCont->m_MeshId, meshCont->m_vertexData.data(), sizeof(FbxVertex), meshCont->m_vertexCount);
		}

		m_MeshContainer.push_back(move(meshCont));
	}

	return S_OK;
}

HRESULT FBXDataContainer::SaveBinary(const fs::path& path)
{
	std::ofstream ofs(path.wstring().c_str(), std::ios::binary);
	if (!ofs) return E_FAIL;

	//バージョン管理による安全性確保
	int version = 2;
	WriteBinary(ofs, version);

	WriteBinary(ofs, m_VertexTotalMin);
	WriteBinary(ofs, m_VertexTotalMax);

	WriteBinary(ofs, m_AnimeFrames);
	WriteBinary(ofs, m_StartTime);
	WriteBinary(ofs, m_EndTime);
	WriteBinary(ofs, m_TimePeriod);

	WriteBinary(ofs, m_ClusterCount);
	WriteBinary(ofs, m_ConstantBufferIndex);

	size_t boneCnt = m_BoneNameList.size();
	WriteBinary(ofs, boneCnt);

	for (const auto& name : m_BoneNameList) WriteString(ofs, name);

	WriteVector(ofs, m_BoneIdList);

	WriteVector(ofs, m_IBoneMatrix);

	size_t nodeCnt = m_NodeNameList.size();
	WriteBinary(ofs, nodeCnt);

	for (const auto& name : m_NodeNameList)
	{
		WriteString(ofs, name);
	}

	//マテリアル
	size_t matCnt = m_MaterialContainer.size();
	WriteBinary(ofs, matCnt);
	for (auto& pair : m_MaterialContainer)
	{
		WriteWString(ofs, pair.first);
		auto& matCont = pair.second;
		ofs.write(reinterpret_cast<const char*>(matCont->ambient), sizeof(float) * 4);
		ofs.write(reinterpret_cast<const char*>(matCont->diffuse), sizeof(float) * 4);
		ofs.write(reinterpret_cast<const char*>(matCont->specular), sizeof(float) * 4);
		WriteBinary(ofs, matCont->alpha);

		// テクスチャ名リストを保存
		auto WriteTextureNameList = [&](const std::vector<std::wstring>& list) 
		{
			size_t size = list.size();
			WriteBinary(ofs, size);
			for (const auto& s : list) WriteWString(ofs, s);
		};
		WriteTextureNameList(matCont->m_DiffuseTextures);
		WriteTextureNameList(matCont->m_NormalTextures);
		WriteTextureNameList(matCont->m_SpecularTextures);
		WriteTextureNameList(matCont->m_FalloffTextures);
		WriteTextureNameList(matCont->m_ReflectionMapTextures);

		// テクスチャパスリストを保存
		auto WriteTexturePathList = [&](const std::vector<std::wstring>& list) 
		{
			size_t size = list.size();
			WriteBinary(ofs, size);
			for (const auto& s : list) WriteWString(ofs, s);
		};
		WriteTexturePathList(matCont->m_DiffusePath);
		WriteTexturePathList(matCont->m_NormalPath);
		WriteTexturePathList(matCont->m_SpecularPath);
		WriteTexturePathList(matCont->m_FalloffPath);
		WriteTexturePathList(matCont->m_ReflectionMapPath);
	}

	//メッシュ
	size_t meshCnt = m_MeshContainer.size();
	WriteBinary(ofs, meshCnt);
	for (auto& meshCont : m_MeshContainer)
	{
		FbxAMatrix matrix = meshCont->GetIBaseMatrix();
		WriteString(ofs, meshCont->GetMeshNodeName());
		WriteBinary(ofs, meshCont->GetParentNodeId());

		XMFLOAT4X4 xmBase;
		ConvertFbxAMatrixToXMFLOAT4x4(meshCont->GetIBaseMatrix(), xmBase);
		WriteBinary(ofs, xmBase);

		WriteBinary(ofs, meshCont->GetSkinCount());
		WriteWString(ofs, meshCont->m_MaterialId);
		WriteWString(ofs, meshCont->m_MeshId);
		WriteBinary(ofs, meshCont->m_vertexCount);
		WriteVector(ofs, meshCont->m_vertexData);
		WriteVector(ofs, meshCont->m_indexData);
		WriteVector(ofs, meshCont->m_skinParams);
		WriteBinary(ofs, meshCont->m_vtxMin);
		WriteBinary(ofs, meshCont->m_vtxMax);
		WriteBinary(ofs, meshCont->GetIBaseMatrix());

	}

	return S_OK;
}

XMMATRIX FBXCharacterData::GetBornMatrix(const char* bornName)
{
	int bornId = m_MainFbx->GetBornIndex(bornName);

	if (bornId != -1 && bornId < m_NodeMatrices.size())
	{
		return XMLoadFloat4x4(&m_NodeMatrices[bornId]);
	}

	return XMMatrixIdentity();
}
