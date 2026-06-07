#pragma once

#include "GameObject.h"
#include "FlyingCameraController.h"
#include "BattleFieldManager.h"
#include "FBXCharacterData.h"
#include "Admin.h"
#include "TimeManager.h"
#include "PlayerFBXs.h"

class FBXCharacterData;

class BattleSceneManager : public GameComponent 
{
public:
	// GameComponent を介して継承されました
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void SetBattlePosition(Animations anim, Platoon* attacker, Platoon* defender);
	void SetPreviousPosition();
	void SetMovePosition(Squares* currentSquare, Squares* nextSquare, FBXCharacterData* moveFbxData);
	void SetAttackAnimation();
	void SetReactionAnimation();

	Platoon* GetAttackerCharacter()
	{
		return m_AttackerCharacter;
	}
	Platoon* GetDefenderCharacter()
	{
		return m_DefenderCharacter;
	}
	Animations GetCurrentAnimation()
	{
		return m_CurrentAnimation;
	}
private:
	enum class SceneState 
	{
		CharacterSetting,
		Actioning,
		Reaction,
		Finish,
		None
	};

	BattleFieldManager* BFMng = nullptr;
	FlyingCameraController* m_FlyingCameraController = nullptr;
	TimeManager* m_TimeManager = nullptr;
	SceneState m_SceneState = SceneState::None;
	Squares* m_CurrentSquare = nullptr;
	Squares* m_NextSquare = nullptr;
	Platoon* m_MoveCharacter = nullptr;
	Platoon* m_AttackerCharacter = nullptr;
	Platoon* m_DefenderCharacter = nullptr;
	FBXCharacterData* m_AttackerFbxData = nullptr;
	FBXCharacterData* m_DefenderFbxData = nullptr;
	FBXCharacterData* m_MoveFbxData = nullptr;

	const XMFLOAT3 kAttackerFbxPosition = { 25.0f, 5.0f, -40.0f };
	const XMFLOAT3 kDefenderFbxPosition = { 25.0f, 5.0f, 40.0f };
	const XMFLOAT3 kAttackerFbxRotation = { 0.0f, 180.0f, 0.0f };
	const XMFLOAT3 kDefenderFbxRotation = { 0.0f, 0.0f, 0.0f };

	const float kOriginScoutEffectPosZ = 80.0f;
	const float kScoutEffectPosZInterval = -0.55f;

	pair<XMFLOAT3, XMFLOAT3> m_AttackerCharacterPreviousPosRotMap;	//攻撃側のキャラクターの元の位置と回転を保存するマップ
	pair<XMFLOAT3, XMFLOAT3> m_DefenderCharacterPreviousPosRotMap;	//防御側のキャラクターの元の位置と回転を保存するマップ

	Animations m_CurrentAnimation = Animations::None;

	float m_CharacterAnimationCount = 0.0f;
	float m_ScoutEffectPosZ = 0.0f;

	XMFLOAT2 m_MoveDistance = { 0.0f, 0.0f };
	XMFLOAT3 m_CharacterCurrentPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_CharacterNextPosition = { 0.0f, 0.0f, 0.0f };
};

