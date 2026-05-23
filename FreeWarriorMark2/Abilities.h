#pragma once
#include "FieldCharacter.h"
#include "AbilityType.h"

class BattleFieldManager;
class Platoon;

class Abilities
{
private:
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
public:
	void ConcentratedFire(Platoon* attackingchara, Platoon* attackedchara);
	void BayonetCharge(Platoon* attackingchara, Platoon* attackedchara);
	bool Scout(Platoon* attackingchara, Platoon* attackedchara);

	
};

