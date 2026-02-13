#pragma once
#include "FieldCharacter.h"
#include "AbilityType.h"

class BattleFieldManager;
class FieldCharacter;

class Abilities
{
private:
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
public:
	void ConcentratedFire(FieldCharacter* attackingchara, FieldCharacter* attackedchara);
	void BayonetCharge(FieldCharacter* attackingchara, FieldCharacter* attackedchara);
	bool Scout(FieldCharacter* attackingchara, FieldCharacter* attackedchara);
};

