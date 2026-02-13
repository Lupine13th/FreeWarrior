#pragma once

#include "GameObject.h"
#include "FBXCharacterData.h" 
#include "Admin.h"
#include "CharacterEquipment.h"

class PlayerBase : public GameComponent
{
private:
    
protected:
	FBXCharacterData* m_chData = nullptr;
	Admin m_admin = Admin::None;
    CharacterEquipment* m_RightEquipment = nullptr;
    CharacterEquipment* m_BackEquipment = nullptr;
    XMVECTOR m_AttackEffectPosition = {};
    bool IsAlive = true;
public:
    void SetAdmin(Admin admin)
    {
		m_admin = admin;
    }
    void Dead()
    {
        IsAlive = false;
    }
    void SetHandEquipment(CharacterEquipment* equipment);
    void SetBackEquipment(CharacterEquipment* equipment);

    void SetMatrixForEquipment(CharacterEquipment* equipment, XMMATRIX matrix, XMFLOAT3 rotate, XMFLOAT3 position, XMFLOAT3 scale);
};

class InfantryPlayer : public PlayerBase
{
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

class ArtilleryPlayer : public PlayerBase
{
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

class MachinegunnerPlayer : public PlayerBase
{
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

class ScoutPlayer : public PlayerBase
{
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};

class ArmoredPlayer : public PlayerBase
{
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;
};


