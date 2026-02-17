#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <cmath> 

#include "EnemyTypes.h"
#include "Abilities.h"
#include "AbilityType.h"
#include "Admin.h"
#include "GameObject.h"
#include "MyAccessHub.h"
#include "FlyingCameraController.h"
#include "Squares.h"
#include "PlayerFBXs.h"

class BattleFieldManager;
class Squares;
enum class EnemyMove;

enum class SoldiersType 
{
	infantry,
	machinegunner,
	artillery,
	scout,
	armored
};

enum class AIroutine
{
	None,
	Attack,
	Defence,
	Scout
};


class FieldCharacter : public GameComponent
{
public:
	float CharaID = -1;
	float CharaSoldiers = -1;	//部隊の兵数
	float CharaMaxSoldiers = -1;//部隊の最大兵数
	float CharaMorales = -1;	//部隊の士気
	float CharaMaxMorales = -1;	//部隊の最大士気
	int CharaPos = -1;			//部隊の座標
	float CharaRenge = -1;		//攻撃可能範囲
	float CharaMoveRenge = -1;		//移動可能範囲
	float CharaPower = -1;		//攻撃力
	float CharaDiffence = -1;	//防御力

	GameObject* CharaObj = nullptr;	//キャラクターそのもの

	SoldiersType CharaKind = SoldiersType::infantry;	//兵種

	Admin CharaAdmin = Admin::None;	//部隊の派閥

	AIroutine CharaAI = AIroutine::None;	//敵部隊のAI行動パターン

	std::wstring CharaName = L"";	//部隊の名前

	bool Moved = false;	//部隊が行動済みかどうか

	bool Detected = false;	//敵に発覚されたかどうか

	bool Dead = false;	//部隊が全滅したかどうか

	bool Selected = false;	//部隊が選択されているかどうか

	EnemyMove AIMove = EnemyMove::None;	//敵部隊のAI行動

	int moveAISquareID = -1;	//敵部隊の移動先マスID
	int targetAICharacterID = -1;	//攻撃する敵部隊のID
	Squares* targetAISquare = nullptr;	//攻撃する敵部隊のマス
	Squares* m_NearestEnemySquare = nullptr;	//最も近い敵部隊のマス

	std::vector<AbilityType> Abilities = //アビリティのリスト
	{
		AbilityType::None,
		AbilityType::None,
		AbilityType::None
	};	

	virtual void initAction() override;
	virtual bool frameAction() override;
	virtual void finishAction() override;

	void SetPlayerBase(PlayerBase* playerBase)
	{
		m_PlayerBase = playerBase;
	};

	PlayerBase* GetPlayerBase()
	{
		return m_PlayerBase;
	}

private:
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	PlayerBase* m_PlayerBase = nullptr;
};

class Infantry : public FieldCharacter
{
public:
	Infantry()
	{
		CharaKind = SoldiersType::infantry;
		CharaRenge = 2.0f;
		CharaMoveRenge = 5.0f;
		CharaPower = 30.0f;
		CharaDiffence = 0.0f;
		CharaSoldiers = 100.0f;
		CharaMaxSoldiers = 100.0f;
		CharaMorales = 50.0f;
		CharaMaxMorales = 100.0f;
		Abilities[0] = AbilityType::BayonetCharge;
		Abilities[1] = AbilityType::Scout;
	}
};

class MachineGunner : public FieldCharacter
{
public:
	MachineGunner()
	{
		CharaKind = SoldiersType::machinegunner;
		CharaRenge = 3.0f;
		CharaMoveRenge = 4.0f;
		CharaPower = 45.0f;
		CharaDiffence = 0.0f;
		CharaSoldiers = 80.0f;
		CharaMaxSoldiers = 80.0f;
		CharaMorales = 60.0f;
		CharaMaxMorales = 100.0f;
		Abilities[0] = AbilityType::ConcentratedFire;
		Abilities[1] = AbilityType::Scout;
	}
};

class Artillery : public FieldCharacter
{
public:
	Artillery()
	{
		CharaKind = SoldiersType::artillery;
		CharaRenge = 5.0f;
		CharaMoveRenge = 3.0f;
		CharaPower = 40.0f;
		CharaDiffence = 0.0f;
		CharaSoldiers = 30.0f;
		CharaMaxSoldiers = 30.0f;
		CharaMorales = 20.0f;
		CharaMaxMorales = 100.0f;
		Abilities[0] = AbilityType::ConcentratedFire;
	}
};

class Scout : public FieldCharacter
{
public:
	Scout()
	{
		CharaKind = SoldiersType::scout;
		CharaRenge = 5.0f;
		CharaMoveRenge = 6.0f;
		CharaPower = 20.0f;
		CharaDiffence = 0.0f;
		CharaSoldiers = 50.0f;
		CharaMaxSoldiers = 50.0f;
		CharaMorales = 50.0f;
		CharaMaxMorales = 100.0f;
		Abilities[0] = AbilityType::Scout;
	}
};

class Armored : public FieldCharacter
{
public:
	Armored()
	{
		CharaKind = SoldiersType::armored;
		CharaRenge = 4.0f;
		CharaMoveRenge = 6.0f;
		CharaPower = 40.0f;
		CharaDiffence = 75.0f;
		CharaSoldiers = 25.0f;
		CharaMaxSoldiers = 25.0f;
		CharaMorales = 50.0f;
		CharaMaxMorales = 100.0f;
		Abilities[0] = AbilityType::ConcentratedFire;
		Abilities[1] = AbilityType::Scout;
	}
};


