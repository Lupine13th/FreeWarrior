#pragma once

#include "GameObject.h"
#include "FBXCharacterData.h" 
#include "Admin.h"
#include "CharacterEquipment.h"
#include "TimeManager.h"
#include "FieldCharacter.h"
#include "BattleFieldManager.h"

class FBXCharacterData;
class CharacterEquipment;
class Platoon;

class PlayerBase : public GameComponent
{
private:
    
protected:
	TimeManager* m_TimeManager = nullptr;
	FBXCharacterData* m_chData = nullptr;
	BattleFieldManager* BFMng = nullptr;
	Admin m_Admin = Admin::None;
	Platoon* m_Platoon = nullptr;
    CharacterEquipment* m_RightEquipment = nullptr;
    CharacterEquipment* m_BackEquipment = nullptr;
    XMVECTOR m_AttackEffectPosition = {};
    bool m_IsAlive = true;
	bool m_IsMoving = false;

	float m_CharacterAnimationCount = 0.0f;

	const float kMoveAnimationDuration = 2.0f; //移動アニメーションの総時間

	XMFLOAT3 m_CurrentPosition = {0.0f, 0.0f, 0.0f};
	XMFLOAT3 m_NextPosition = {0.0f, 0.0f, 0.0f};

    XMFLOAT2 m_MoveDistance = { 0.0f, 0.0f };
public:
    void SetAdmin(Admin admin)
    {
		m_Admin = admin;
    }
    void SetHandEquipment(CharacterEquipment* equipment);
    void SetBackEquipment(CharacterEquipment* equipment);

    void SetMatrixForEquipment(CharacterEquipment* equipment, XMMATRIX matrix, XMFLOAT3 rotate, XMFLOAT3 position, XMFLOAT3 scale);
    void SetCharacterCameraLabel(FBXCharacterData* characterData, bool hasAnime);
    void SetMoveAnimation(XMFLOAT3 currentPosition, XMFLOAT3 nextPosition);
    void SetPlatoon(Platoon* platoon)
    {
		m_Platoon = platoon;
    }

    Platoon* GetPlatoon()
    {
		return m_Platoon;
    }

    void MovingUpdate();
};

class InfantryPlayer : public PlayerBase
{
public:
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};

class ArtilleryPlayer : public PlayerBase
{
public:
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};

class MachinegunnerPlayer : public PlayerBase
{
public:
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};

class ScoutPlayer : public PlayerBase
{
public:
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};

class ArmoredPlayer : public PlayerBase
{
public:
    virtual void InitAction() override;
    virtual bool FrameAction() override;
    virtual void FinishAction() override;
};


