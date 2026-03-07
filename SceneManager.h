#pragma once

#include <memory>
#include <vector>
#include <GameObject.h>
#include <SceneController.h>
#include <unordered_map>
#include "FieldCharacter.h"
#include "TerrainComponent.h"
#include "CharacterEquipment.h"
#include "PlayerFBXs.h"

//=========Scene Change
class SceneManager;
class FieldCharacter;
class Squares;
class CharacterEquipment;
class PlayerBase;
enum class SoldiersType;

class SceneObjectDeleter : public AbstractGameObjectDeleter
{
private:
	SceneManager* myScene;
public:
	SceneObjectDeleter(SceneManager* scene)
	{
		myScene = scene;
	}
	// AbstractGameObjectDeleter を介して継承されました
	virtual void ExecuteDeleter(GameObject* go) override;
};
//=========Scene Change END

enum class PlayStates
{
	Debug,
	Release
};

class SceneManager : public SceneController
{
private:
	std::unique_ptr<GameObject> m_SystemObject;
	GameComponent* m_KeyComponent;
	std::unordered_map<std::wstring, GameComponent*> m_CameraComponents;
	std::vector<TerrainComponent*> m_terrains;
	std::list<std::unique_ptr<GameObject>> m_SceneObjects;
	std::unique_ptr<SceneObjectDeleter> m_pObjDeleter;
	void ClearSceneObjects();
	void SetCharaToSquares(FieldCharacter* chara, Squares* square);
	bool IsFirstTime = true;
public:
	HRESULT initSceneController() override;
	HRESULT changeGameScene(UINT scene) override;

	const PlayStates kPlayStates = PlayStates::Release;	//デバッグかリリースか

	GameComponent* getKeyComponent()
	{
		return m_KeyComponent;
	}

	GameComponent* getCameraComponent(const std::wstring cameraName)
	{
		auto it = m_CameraComponents.find(cameraName); // マップからカメラを探す
		if (it != m_CameraComponents.end()) // カメラが見つかった場合
		{
			GameComponent* cam = it->second;
			// カメラが存在し、かつ isActive() (m_activeFlg) が true の場合にのみ返す
			if (cam != nullptr && cam->isActive())
			{
				return cam;
			}
		}
		return nullptr; // カメラが見つからないか、非アクティブな場合はnullptrを返す
	}

	void SetActiveCameraCompornent(const std::wstring cameraName, bool isActive)
	{
		auto camera = m_CameraComponents.find(cameraName);
		if (camera != m_CameraComponents.end()) // カメラが見つかった場合
		{
			GameComponent* cameraCompornent = camera->second;
			cameraCompornent->setActive(isActive);
		}
	}

	//====CameraFix
	void RemoveCamera(GameComponent* gc);
	void RemoveCamera(std::wstring label);
	//====CameraFix End

	//=========Scene Change
	void AddSceneObject(GameObject* obj);
	void DeleteSceneObject(GameObject* obj);
	//=========Scene Change End

	//TerrainComponent取得
	TerrainComponent* GetTerrainComponent(int index)
	{
		if (m_terrains.size() <= index)
			return nullptr;
		return m_terrains[index];
	}

	void ClearTerrains()
	{
		m_terrains.clear();
	}

	float SpposX = -25.0f;
	float SpposY = 0;

	int SquareCount = 0;

	vector<FieldCharacter*> ally = { nullptr ,nullptr, nullptr, nullptr, nullptr };
	vector<FieldCharacter*> enemy = { nullptr ,nullptr, nullptr, nullptr, nullptr };

	void GenerateEquipment(SoldiersType type, PlayerBase* playerBase, Admin admin);
};
