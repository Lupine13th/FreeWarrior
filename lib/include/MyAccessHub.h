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
public:

	static void setMyGameEnegine(MyGameEngine* eng)
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
};
