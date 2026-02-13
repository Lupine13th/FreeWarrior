#pragma once

#include <memory>
#include <vector>
#include <string>
#include <Windows.h>
#include <DirectXMath.h>
#include <chrono>
#include <cmath> 

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "FBXCharacterData.h"
#include "SceneManager.h"
#include "SquareColor.h"
#include "Admin.h"
#include "FlyingCameraController.h"
#include "TimeManager.h"

#include "BattleFieldManager.h"

using namespace std;

class FieldCharacter;

enum class Terrain
{
	Plane,
	Forest,
	Hills,
	River,
	Tower,
	Supply
};

class Squares : public GameComponent
{
private:
	CharacterData* m_SqData = nullptr;

	FlyingCameraController* Fcam;

	TimeManager* m_TimeManager;

	BattleFieldManager* BFMng;

	int m_SquareID = -1;	//マスのID	

	bool m_IsAnimating = false;
	bool m_IsFirstAnimation = false;
	bool m_IsDamaged = false;

	float m_CharacterAnimationCount = 0.0f;
	float m_DistanceX = 0.0f;
	float m_DistanceY = 0.0f;
	float m_Rotate = 0.0f;
	float m_ScoutEffectPosZ = 0.0f;

	const float kOriginScoutEffectPosZ = 80.0f;
	const float kScoutEffectPosZInterval = -0.55f;

	Animations NowAnimation = Animations::None;

	Squares* NowChara = nullptr;
	Squares* NextChara = nullptr;

	Admin NowAdmin = Admin::None;

	XMFLOAT3 NowCharaPos = {};
	XMFLOAT3 NextCharaPos = {};
	XMFLOAT3 NowCharaRot = {};
	XMFLOAT3 NextCharaRot = {};
public:
	
	int ThereCharaID = -1;	//そのマスにいるキャラクター

	int charaPosX = -1;
	int charaPosY = -1;

	XMFLOAT3 SqPos;

	bool changeColor = false;
	bool firstcount = false;

	bool passed = false; //移動可能範囲表示の際に通過したかどうか

	bool target = false;

	Admin SqAdmin = Admin::None;	//所有勢力

	FieldCharacter* chara = nullptr;	//そのマスにいるキャラ

	FBXCharacterData* fbxD = nullptr;

	Terrain terrainname = Terrain::Plane;

	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void HitReaction();
	void AttackReaction();

	void SetSquaresColor(SquareColor color);
	void SetAnimation(Animations anim, Admin admin, Squares* chara, Squares* next);
	void SetSquareID(int index)
	{
		m_SquareID = index;
	}

	int GetSquareID()
	{
		return m_SquareID;
	}
	int GetCharacterID()
	{
		return m_SquareID;
	}

	void SetBattlePosition();
	void SetPreviousPosition();

	Squares* GetNextChara()
	{
		return NextChara;
	}
	Squares* GetNowChara()
	{
		return NowChara;
	}
};

