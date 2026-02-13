#pragma once

#include "GameObject.h"
#include "FBXCharacterData.h" 

enum EquipType
{
	None,
	LongGun,
	HandGun,
	Body,
};

class CharacterEquipment : public GameComponent
{
private:
	
protected:
	FBXCharacterData* m_chData = nullptr;

	virtual void InInit(FBXCharacterData*) {};
	virtual bool InFrame() { return true; };
	virtual void InFinish() {};
	void SetActive(bool active);

	bool m_IsAlive = true;
public:
	virtual void initAction() override;
	virtual bool frameAction() override;
	virtual void finishAction() override;

	FBXCharacterData* GetCharacterData()
	{
		return m_chData;
	};
};

class RebelAssaltRifle : public CharacterEquipment
{
private:
	EquipType m_EquipType = EquipType::LongGun;
public:
	virtual void InInit(FBXCharacterData*) override;
	virtual bool InFrame() override;
	virtual void InFinish() override;
};

class ImperialAssaltRifle : public CharacterEquipment
{
private:
	EquipType m_EquipType = EquipType::LongGun;
public:
	virtual void InInit(FBXCharacterData*) override;
	virtual bool InFrame() override;
	virtual void InFinish() override;
};

class RebelMachinegun : public CharacterEquipment
{
private:
	EquipType m_EquipType = EquipType::LongGun;
public:
	virtual void InInit(FBXCharacterData*) override;
	virtual bool InFrame() override;
	virtual void InFinish() override;
};

class RebelScoutRifle : public CharacterEquipment
{
private:
	EquipType m_EquipType = EquipType::LongGun;
public:
	virtual void InInit(FBXCharacterData*) override;
	virtual bool InFrame() override;
	virtual void InFinish() override;
};

class ImperialScoutRifle : public CharacterEquipment
{
private:
	EquipType m_EquipType = EquipType::LongGun;
public:
	virtual void InInit(FBXCharacterData*) override;
	virtual bool InFrame() override;
	virtual void InFinish() override;
};

class Backpack : public CharacterEquipment
{
private:
	EquipType m_EquipType = EquipType::Body;
public:
	virtual void InInit(FBXCharacterData*) override;
	virtual bool InFrame() override;
	virtual void InFinish() override;
};

