#pragma once

#include <vector>

#include "MyGameEngine.h"

class MyGameEngine;
class BattleFieldManager;
class EnemyAIManager;
class BattleReadyScene;
class FlyingCameraController;
class TimeManager;
class TitleScene;
class HUDManager;
class EffectGenerator;
class FBXResourceManager;
class LoadAnimationHUD;
class SceneManager;

class MyAccessHub
{
private:
	static MyGameEngine* m_engine;
	static BattleFieldManager* m_BattleFieldManager;
	static EnemyAIManager* m_EnemyAIManager;
	static BattleReadyScene* m_BattleReadyScene;
	static FlyingCameraController* m_FlyingCameraController;
	static TimeManager* m_TimeManager;
	static TitleScene* m_TitleScene;
	static HUDManager* m_HUDManager;
	static EffectGenerator* m_EffectGenerator;
	static FBXResourceManager* m_FBXResourceManager;
	static LoadAnimationHUD* m_LoadAnimationHUD;
	static SceneManager* m_SceneManager;
public:

	static void SetMyGameEnegine(MyGameEngine* eng)
	{
		m_engine = eng;
	}

	static void SetBattleFieldManager(BattleFieldManager* bfmng)
	{
		m_BattleFieldManager = bfmng;
	}

	static void SetEnemyAIManager(EnemyAIManager* aimng)
	{
		m_EnemyAIManager = aimng;
	}

	static void SetBattleReadyScene(BattleReadyScene* brscene)
	{
		m_BattleReadyScene = brscene;
	}

	static void SetFlyingCameraComtroller(FlyingCameraController* fcam)
	{
		m_FlyingCameraController = fcam;
	}

	static void SetTimeManager(TimeManager* timemanager)
	{
		m_TimeManager = timemanager;
	}

	static void SetTitleScene(TitleScene* titleScene)
	{
		m_TitleScene = titleScene;
	}

	static void SetHUDManager(HUDManager* hudmanager)
	{
		m_HUDManager = hudmanager;
	}

	static void SetEffectGenerator(EffectGenerator* effectGenerator)
	{
		m_EffectGenerator = effectGenerator;
	}

	static void SetFBXResourceManager(FBXResourceManager* fbxResourceManager)
	{
		m_FBXResourceManager = fbxResourceManager;
	}

	static void SetLoadAnimationHUD(LoadAnimationHUD* loadAnimationHUD)
	{
		m_LoadAnimationHUD = loadAnimationHUD;
	}

	static void SetSceneManager(SceneManager* sceneManager)
	{
		m_SceneManager = sceneManager;
	}

	static MyGameEngine* GetMyGameEngine()
	{
		return m_engine;
	}

	static BattleFieldManager* GetBFManager()
	{
		return m_BattleFieldManager;
	}

	static EnemyAIManager* GetAIManager()
	{
		return m_EnemyAIManager;
	}

	static BattleReadyScene* GetBRScene()
	{
		return m_BattleReadyScene;
	}

	static FlyingCameraController* GetFlyingCameraController()
	{
		return m_FlyingCameraController;
	}

	static TimeManager* GetTimeManager()
	{
		return m_TimeManager;
	}

	static TitleScene* GetTitleScene()
	{
		return m_TitleScene;
	}

	static HUDManager* GetHUDManager()
	{
		return m_HUDManager;
	}

	static EffectGenerator* GetEffectGenerator()
	{
		return m_EffectGenerator;
	}

	static FBXResourceManager* GetFBXResourceManager()
	{
		return m_FBXResourceManager;
	}

	static LoadAnimationHUD* GetLoadAnimationHUD()
	{
		return m_LoadAnimationHUD;
	}

	static SceneManager* GetSceneManager()
	{
		return m_SceneManager;
	}
};
