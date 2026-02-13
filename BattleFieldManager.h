#pragma once

#include <memory>
#include <vector>
#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include <chrono>

class Squares;
//class StatusHUD;
class StatusWords;
class LogHUD;
class LogHUDW;
class DamageHUD;
class DamageHUDW;
class OpeningAnimHUD;
class TurnEndUI;
class MenuText;
class TerrainHUD;
class ResultUI;
class MovedCountHUD;
class TurnUI;
class HUDManager;
class Abilities;

#include "Squares.h"
#include "GameObject.h"
#include "FieldCharacter.h"
#include "MenuUI.h"
#include "MonitorSelectUI.h"
#include "Abilities.h"
#include "LogHUD.h"
#include "LogHUDW.h"
#include "BattleReadyScene.h"
#include "DamageHUD.h"
#include "DamageHUDW.h"
#include "TurnEndUI.h"
#include "OpeningAnimHUD.h"
#include "MenuText.h"
#include "ResultUI.h"
#include "MovedCountHUD.h"
#include "TurnUI.h"
#include "HUDManager.h"
#include "CameraChangerComponent.h"
#include "TimeManager.h"

using namespace std;

enum class Mode
{
	FieldMode,
	SideMenuMode,
	MenuMode,
	AttackMode,
	MoveMode,
	AbilityMode,
	TurnEndMode
};

enum class TargetMode
{
	EnemyTarget,
	SquareTarget,
	AliesTarget,
	None
};

enum class Turn
{
	Allies,
	Enemy,
	EnemyMove,
	TurnChanging,
	First,
	Result
};

enum class ActionName
{
	Attack,
	Move,
	ConcentratedFire,
	BayonetCharge,
	Scout,
};

enum class CursorState
{
	Select,
	Target
};

enum class AbillityMenuState
{
	None,
	Menu,
	Target
};

class BattleFieldManager:public GameComponent
{
private:
	bool m_PlayerWin = false;							//プレイヤーが勝利したかどうか
	int m_TurnCount = 0;								//現在のターン数
	int m_PlayerKillCount = 0;							//プレイヤーが倒した部隊の数
	int m_EnemyKillCount = 0;							//敵が倒した部隊の数
	int m_SelectID = 0;									//選択カーソルのマスID
	vector<int> m_SelectPos = { 0, 0 };					//選択カーソルのマス座標
	int m_TargetID = 0;									//攻撃カーソルのマスID
	vector<int> m_TargetPos = { 0, 0 };					//攻撃カーソルのマス座標
	int m_MenuSelectIndex = 0;							//メニューの選択インデックス
	int m_SideMenuSelectIndex = 0;						//サイドメニューの選択インデックス
	int m_MovedCount = 0;								//行動済みの部隊数
	int m_Lifecount = 0;								//生存している部隊数
	vector<int> m_InRangeIDListForMenu = {};			//攻撃範囲内のマスIDリスト(メニューを開く際に利用)
	vector<int> m_InRangeEnemyList = {};				//攻撃範囲内の敵のマスIDリスト
	vector<int> m_InRangeIDList = {};					//移動可能範囲内のマスIDリスト
	vector<int> m_EnemyRangeIDList = {};				//移動可能範囲内のマスIDリスト
	int m_AbillityIndex = 0;							//使用するアビリティのインデックス
	int m_AbillityCount = 0;

	float m_AttackingCount = 0.0f;						//攻撃アニメーションのカウント
	const float kMaxAttackingCount = 3.1f;				//攻撃アニメーションのカウント最大値

	bool m_OpenLog = false;								//行動ログを開いているかどうか
	bool m_Firsttime = false;							//初回のみの処理かどうか
	bool m_OnlyOneTime = false;							//一回だけの処理
	bool m_TurnEnd = false;								//ターン終了したかどうか
	bool m_IsAttacking = false;							//攻撃アニメーション中かどうか
	bool m_BattleCameraEnable = false;					//バトルカメラが有効かどうか
	bool m_IsSucceedScout = false;						//直前の偵察が成功したかどうか

	Mode m_Mode = Mode::FieldMode;						//モードの名前
	TargetMode m_TargetMode = TargetMode::None;			//ターゲットのモード
	CursorState m_CursorState = CursorState::Select;	//カーソルの状態
	Abilities* m_Abilities;								//Abilitiesのクラス 
	Turn m_CurrentTurn = Turn::First;					//現在のターン
	Turn m_PreviousTurn = Turn::First;					//一つ前のターン
	vector<Squares*> m_FieldSquaresList;				//それぞれのマスの情報
	Squares* m_SelectingSquare;							//選択中のマス
	vector<Squares*> m_PassedSquaresList;				//通過したマスのリスト
	vector<Squares*> m_ChangedSquaresList;				//移動可能範囲に変化したマスのリスト
	vector<FieldCharacter*> m_AlliesCharacterList;		//味方(プレイヤー)の部隊リスト
	vector<FieldCharacter*> m_EnemyCharacterList;		//敵(AI)の部隊リスト
	Squares* m_AttackigCharacterSquare;					//攻撃している部隊のマス
	AbillityMenuState m_AbillityMenuState = AbillityMenuState::None;	//アビリティメニューの状態

	//==========Pointer==========
	MenuUI* m_MenuUI;
	MonitorSelectUI* m_MenuSelectUI;
	//StatusHUD* m_StatusHUD;
	StatusWords* m_StatusText;
	TerrainHUD* m_TerrainHUD;
	MenuText* m_MenuText;
	LogHUD* m_LogHUD;
	LogHUDW* m_LogHUDText;
	DamageHUD* m_DamageHUD;
	DamageHUDW* m_DamageHUDText;
	TurnEndUI* m_TurnEndUI;
	OpeningAnimHUD* m_OpeningAnimHUD;
	ResultUI* m_ResultUI;
	MovedCountHUD* m_MovedCountHUD;
	TurnUI* m_TurnUI;

	HUDManager* m_HUDManager;

	CameraChangerComponent* m_CameraChangerCompornent;

	TimeManager* m_TimeManager = nullptr;

	FieldCharacter* m_AttackedCharacter = nullptr;

	wstring logs;

	XMFLOAT4 logColors[8];
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	//==========Setter==========
	void SetMenuUI(MenuUI* MenuUI);
	void SetMenuSelectUI(MonitorSelectUI* MonitorSelectUI);
	//void SetStatusHUD(StatusHUD* StatusHUD);
	void SetStatusText(StatusWords* StatusWords);
	void SetTerrainHUD(TerrainHUD* TerrainHUD);
	void SetMenuText(MenuText* MenuText);
	void SetLogHUD(LogHUD* LogHUD);
	void SetLogHUDText(LogHUDW* LogHUDW);
	void SetDamageHUD(DamageHUD* DamageHUD);
	void SetDamageHUDText(DamageHUDW* DamageHUDW);
	void SetTurnEndUI(TurnEndUI* TurnEndUI);
	void SetOpeningAnimHUD(OpeningAnimHUD* OpeningAnimHUD);
	void SetResultUI(ResultUI* ResultUI);
	void SetMovedCountHUD(MovedCountHUD* MovedCountHUD);
	void SetTurnUI(TurnUI* TurnUI);
	void SetCurrentTurn(Turn turn);
	void AddFieldSquare(Squares* square);
	void SetAlliesCharacterList(vector<FieldCharacter*> list);
	void SetEnemyCharacterList(vector<FieldCharacter*> list);
	void SetAbiliteis(Abilities* abilities);
	XMFLOAT3 GetCharaPos(Squares* squ);
	void SetTerrainData();  
	void SetChangedSq(int index);
	void SetInRengeSquareColor(int index, float renge);
	void SetEnemyRengeSquareTexture();
	void SetPlayerWin(bool win);
	void SetCameraChangerCompornent(CameraChangerComponent* cameraChanger);
	void AddTurnCount();
	void SetBattleCameraEnable(bool enable)
	{
		m_BattleCameraEnable = enable;
	};
	void SetAttackingCharacterSquares(Squares* squares)
	{
		m_AttackigCharacterSquare = squares;
	};

	//==========Getter==========
	int GetTurnCount();
	int GetPlayerKillCount();
	int GetEnemyKillCount();
	int GetSelectID()
	{
		return m_SelectID;
	}
	int GetTargetID()
	{
		return m_TargetID;
	}
	vector<int> GetSelectPos()
	{
		return m_SelectPos;
	}
	vector<int> GetTargetPos()
	{
		return m_TargetPos;
	}
	Turn GetPreviousTurn() 
	{ 
		return m_PreviousTurn;
	}
	bool GetPlayerWin()
	{
		return m_PlayerWin;
	}
	bool GetOpenLog()
	{
		return m_OpenLog;
	}
	int GetMenuSelectIndex()
	{
		return m_MenuSelectIndex;
	}
	int GetAbillityIndex()
	{
		return m_AbillityIndex;
	}
	bool GetTurnEnd()
	{
		return m_TurnEnd;
	}
	vector<int>& GetInRangeIDListForMenu()
	{
		return m_InRangeIDListForMenu;
	}
	vector<Squares*> GetFieldSquaresList()
	{
		return m_FieldSquaresList;
	}
	MenuUI* GetMenuUI()
	{
		return m_MenuUI;
	}
	MonitorSelectUI* GetMonitorSelectUI()
	{
		return m_MenuSelectUI;
	}
	StatusWords* GetStatusText()
	{
		return m_StatusText;
	}
	TerrainHUD* GetTerrainHUD()
	{
		return m_TerrainHUD;
	}
	MenuText* GetMenuText()
	{
		return m_MenuText;
	}
	LogHUD* GetLogHUD()
	{
		return m_LogHUD;
	}
	LogHUDW* GetLogHUDText()
	{
		return m_LogHUDText;
	}
	DamageHUD* GetDamageHUD()
	{
		return m_DamageHUD;
	}
	DamageHUDW* GetDamageHUDText()
	{
		return m_DamageHUDText;
	}
	TurnEndUI* GetTurnEndUI()
	{
		return m_TurnEndUI;
	}
	OpeningAnimHUD* GetOpeningAnimHUD()
	{
		return m_OpeningAnimHUD;
	}
	ResultUI* GetResultUI()
	{
		return m_ResultUI;
	}
	MovedCountHUD* GetMovedCountHUD()
	{
		return m_MovedCountHUD;
	}
	TurnUI* GetTurnUI()
	{
		return m_TurnUI;
	}
	Mode GetMode()
	{
		return m_Mode;
	}
	vector<FieldCharacter*> GetAlliesCharacterList()
	{
		return m_AlliesCharacterList;
	}
	vector<FieldCharacter*> GetEnemyCharacterList()
	{
		return m_EnemyCharacterList;
	}
	Turn GetCurrentTurn()
	{
		return m_CurrentTurn;
	}
	TargetMode GetTargetMode()
	{
		return m_TargetMode;
	}
	CursorState GetCursorState()
	{
		return m_CursorState;
	}
	CameraChangerComponent* GetCameraChangerCompornent()
	{
		return m_CameraChangerCompornent;
	}
	int GetSideMenuSelectIndex()
	{
		return m_SideMenuSelectIndex;
	}
	bool GetHUDEnableCondition()
	{
		if (m_Mode != Mode::SideMenuMode && m_CurrentTurn == Turn::Allies && m_BattleCameraEnable == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool GetBattleCameraEnable()
	{
		return m_BattleCameraEnable;
	}
	Squares* GetAttackingCharacterSquares()
	{
		return m_AttackigCharacterSquare;
	}
	int GetInLengeEnemyCount()
	{
		return m_InRangeEnemyList.size();
	}
	AbillityMenuState GetAbillityMenuState()
	{
		return m_AbillityMenuState;
	}
	bool GetIsSucceedScout()
	{
		return m_IsSucceedScout;
	}
	FieldCharacter* GetAttackedCharacter()
	{
		return m_AttackedCharacter;
	}
	//==========Method==========
	void ChangeTurnAllies();
	void ChangeTurnEnemy();
	void UpdateBattleField();
	void Attack(FieldCharacter* attackingchara, FieldCharacter* attackedchara);
	void Move(int nowPos, int nextPos, float charaID);
	void Wait(int nowPos);
	void ResetFieldFromMove();
	void ChangeTurn();
	void ResetHUDs(int SEindex);
	void Abiliting(FieldCharacter* attackingchara, FieldCharacter* attackedchara);
	void RefreshLogs(FieldCharacter* attackingchara, FieldCharacter* attackedchara, ActionName action, int damage, bool sccess);
	void DeleteChara(int deadCharaPos);
	void CheckDead(FieldCharacter* chara);
	void CheckMoved();
	void SetResult(bool win);
	void SearchInRengeSquare(int charaPosition, float renge, vector<int>& idList);
	void ResetAbillityMenu();
};




