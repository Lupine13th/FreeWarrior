#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <cmath> 

#include "EnemyTypes.h"
#include "AbilityType.h"
#include "Admin.h"
#include "GameObject.h"
#include "MyAccessHub.h"
#include "FlyingCameraController.h"
#include "Squares.h"
#include "PlayerFBXs.h"

class BattleFieldManager;
class Squares;
enum class EnemyActionType;

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


class Platoon : public GameComponent
{
protected:
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	PlayerBase* m_PlayerBase = nullptr;

	int m_PlatoonID = -1;
	int m_Soldiers = -1;	//部隊の兵数
	int m_MaxSoldiers = -1;//部隊の最大兵数
	int m_Morales = -1;	//部隊の士気
	int m_MaxMorales = -1;	//部隊の最大士気
	int m_SquaresPosition = -1;			//部隊の座標
	int m_AttackRenge = -1;		//攻撃可能範囲
	int m_MoveRenge = -1;		//移動可能範囲
	int m_AttackPower = -1;		//攻撃力
	int m_Armor = -1;	//防御力

	SoldiersType m_SoldierType = SoldiersType::infantry;	//兵種

	Admin m_Admin = Admin::None;	//部隊の派閥

	AIroutine m_AIRoutine = AIroutine::None;	//敵部隊のAI行動パターン

	std::wstring m_PlatoonName = L"";	//部隊の名前

	bool m_IsActioned = false;	//部隊が行動済みかどうか

	bool m_IsDetected = false;	//敵に発覚されたかどうか

	bool m_IsDead = false;	//部隊が全滅したかどうか

	bool m_IsSelected = false;	//部隊が選択されているかどうか

	EnemyActionType m_EnemyAction = EnemyActionType::None;	//敵部隊のAI行動

	int m_AIMoveSquareID = -1;	//敵部隊の移動先マスID
	int m_AITargetCharacterID = -1;	//攻撃する敵部隊のID
	Squares* m_TargetAISquare = nullptr;	//攻撃する敵部隊のマス
	Squares* m_NearestEnemySquare = nullptr;	//最も近い敵部隊のマス

	vector<AbilityType> Abilities
	{
		AbilityType::None,
		AbilityType::None,
		AbilityType::None
	};//アビリティのリスト

public:
	virtual void InitAction() override;
	virtual bool FrameAction() override;
	virtual void FinishAction() override;

	void SetPlatoonName(wstring name)
	{
		m_PlatoonName = name;
	};

	void SetPlayerBase(PlayerBase* playerBase)
	{
		m_PlayerBase = playerBase;
	};

	void SetPlatoonId(int platoonID)
	{
		m_PlatoonID = platoonID;
	};

	void SetAdmin(Admin admin)
	{
		m_Admin = admin;
	};

	void SetSoldier(int soldier)
	{
		m_Soldiers = soldier;
	};

	void SetMorale(int morale)
	{
		m_Morales = morale;
	};

	void SetIsActioned(bool isActioned)
	{
		m_IsActioned = isActioned;
	};

	void SetIsDead(bool isDead)
	{
		m_IsDead = isDead;
	};

	void SetCharacterPosOnSquares(int pos)
	{
		m_SquaresPosition = pos;
	};

	void SetAIRoutine(AIroutine routine)
	{
		m_AIRoutine = routine;
	};

	void SetIsSelected(bool isSelected)
	{
		m_IsSelected = isSelected;
	};

	void SetIsDetected(bool isDetected)
	{
		m_IsDetected = isDetected;
	};

	void SetEnemyActionType(EnemyActionType action)
	{
		m_EnemyAction = action;
	};

	void SetTargetAISquare(Squares* square)
	{
		m_TargetAISquare = square;
	};
	
	void SetTargetAISquare(int targetSquareId)
	{
		m_AITargetCharacterID = targetSquareId;
	};

	void SetTargetAICharacterID(int targetAICharacterID)
	{
		m_AITargetCharacterID = targetAICharacterID;
	};

	void SetMoveAISquareID(int moveAISquareID)
	{
		m_AIMoveSquareID = moveAISquareID;
	};

	void SetNearestEnemySquare(Squares* square)
	{
		m_NearestEnemySquare = square;
	};

	PlayerBase* GetPlayerBase()
	{
		return m_PlayerBase;
	}

	std::vector<AbilityType> GetAbilityList()
	{
		return Abilities;
	};

	std::wstring GetPlatoonName()
	{
		return m_PlatoonName;
	}

	int GetPlatoonID()
	{
		return m_PlatoonID;
	}

	int GetSoldiers()
	{
		return m_Soldiers;
	}

	int GetMaxSoldiers()
	{
		return m_MaxSoldiers;
	}

	int GetMorale()
	{
		return m_Morales;
	}

	int GetMaxMorale()
	{
		return m_MaxMorales;
	}

	float GetSoldiersPercent()
	{
		return m_Soldiers / m_MaxSoldiers;
	}

	bool GetIsDetected()
	{
		return m_IsDetected;
	}

	bool GetIsDead()
	{
		return m_IsDead;
	}

	bool GetIsActioned()
	{
		return m_IsActioned;
	}

	bool GetIsSelected()
	{
		return m_IsSelected;
	}

	int GetCharacterPosOnSquares()
	{
		return m_SquaresPosition;
	}

	int GetAttackRenge()
	{
		return m_AttackRenge;
	}

	int GetAttackPower()
	{
		return m_AttackPower;
	}

	int GetMoveRenge()
	{
		return m_MoveRenge;
	}

	int GetArmor()
	{
		return m_Armor;
	}

	int GetAITargetCharacterID()
	{
		return m_AITargetCharacterID;
	}

	int GetAIMoveSquareID()
	{
		return m_AIMoveSquareID;
	}

	Admin GetAdmin()
	{
		return m_Admin;
	}

	SoldiersType GetSoldiersType()
	{
		return m_SoldierType;
	}

	EnemyActionType GetEnemyActionType()
	{
		return m_EnemyAction;
	}

	Squares* GetTargetAISquare()
	{
		return m_TargetAISquare;
	}

	Squares* GetNearestEnemySquare()
	{
		return m_NearestEnemySquare;
	}
};

class Infantry : public Platoon
{
public:
	Infantry()
	{
		m_SoldierType = SoldiersType::infantry;
		m_AttackRenge = 2;
		m_MoveRenge = 5;
		m_AttackPower = 30;
		m_Armor = 0;
		m_Soldiers = 100;
		m_MaxSoldiers = 100;
		m_Morales = 50;
		m_MaxMorales = 100;
		Abilities[0] = AbilityType::BayonetCharge;
		Abilities[1] = AbilityType::ConcentratedFire;
	}
};

class MachineGunner : public Platoon
{
public:
	MachineGunner()
	{
		m_SoldierType = SoldiersType::machinegunner;
		m_AttackRenge = 2;
		m_MoveRenge = 4;
		m_AttackPower = 45;
		m_Armor = 0;
		m_Soldiers = 80;
		m_MaxSoldiers = 80;
		m_Morales = 60;
		m_MaxMorales = 100;
		Abilities[0] = AbilityType::ConcentratedFire;
		Abilities[1] = AbilityType::Scout;
	}
};

class Artillery : public Platoon
{
public:
	Artillery()
	{
		m_SoldierType = SoldiersType::artillery;
		m_AttackRenge = 5;
		m_MoveRenge = 3;
		m_AttackPower = 40;
		m_Armor = 0;
		m_Soldiers = 30;
		m_MaxSoldiers = 30;
		m_Morales = 20;
		m_MaxMorales = 100;
		Abilities[0] = AbilityType::ConcentratedFire;
	}
};

class Scout : public Platoon
{
public:
	Scout()
	{
		m_SoldierType = SoldiersType::scout;
		m_AttackRenge = 5;
		m_MoveRenge = 6;
		m_AttackPower = 20;
		m_Armor = 0;
		m_Soldiers = 50;
		m_MaxSoldiers = 50;
		m_Morales = 50;
		m_MaxMorales = 100;
		Abilities[0] = AbilityType::Scout;
	}
};

class Armored : public Platoon
{
public:
	Armored()
	{
		m_SoldierType = SoldiersType::armored;
		m_AttackRenge = 4;
		m_MoveRenge = 6;
		m_AttackPower = 40;
		m_Armor = 75;
		m_Soldiers = 25;
		m_MaxSoldiers = 25;
		m_Morales = 50;
		m_MaxMorales = 100;
		Abilities[0] = AbilityType::ConcentratedFire;
		Abilities[1] = AbilityType::Scout;
	}
};


