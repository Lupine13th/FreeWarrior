#include <iostream>
#include <random>

#include "Abilities.h"
#include "BattleFieldManager.h"

void Abilities::ConcentratedFire(FieldCharacter* attackingchara, FieldCharacter* attackedchara)
{
	int damage = 0;

	if (BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Forest || BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Tower)
	{
		damage = attackingchara->CharaPower * 1.2;
	}
	else
	{
		damage = attackingchara->CharaPower;
	}
	if (BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::Hills || BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::River)
	{
		damage = damage * 0.75;
	}
	if (!attackedchara->Detected)
	{
		damage = damage * 0.75;
	}

	BFMng->GetDamageHUD()->SetDamage(damage * 1.2f, attackedchara->CharaMaxSoldiers, attackedchara->CharaSoldiers);

	attackingchara->CharaMorales = attackingchara->CharaMorales - 20;
	attackedchara->CharaSoldiers = attackedchara->CharaSoldiers - damage * 1.2f;

	BFMng->CreateAbilityLog(attackingchara, ActionName::ConcentratedFire, damage * 1.2f);

	BFMng->RefreshLogs(attackingchara, attackedchara, ActionName::ConcentratedFire, damage * 1.2f, false);
}

void Abilities::BayonetCharge(FieldCharacter* attackingchara, FieldCharacter* attackedchara)
{
	int damage = 0;

	if (BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Forest || BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Tower)
	{
		damage = attackingchara->CharaPower * 1.2;
	}
	else
	{
		damage = attackingchara->CharaPower;
	}
	if (BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::Hills || BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::River)
	{
		damage = damage * 0.75;
	}

	BFMng->GetDamageHUD()->SetDamage(damage * 1.2f, attackedchara->CharaMaxSoldiers, attackedchara->CharaSoldiers);

	attackingchara->CharaMorales = attackingchara->CharaMorales - 10;
	attackingchara->CharaSoldiers = attackingchara->CharaSoldiers - attackingchara->CharaSoldiers * 0.1;
	attackedchara->CharaSoldiers = attackedchara->CharaSoldiers - damage * 1.2f;

	BFMng->CreateAbilityLog(attackingchara, ActionName::BayonetCharge, damage * 1.2f);

	BFMng->RefreshLogs(attackingchara, attackedchara, ActionName::BayonetCharge, damage * 1.2f, false);
}

bool Abilities::Scout(FieldCharacter* attackingchara, FieldCharacter* attackedchara)
{
	std::random_device rd;  // ハードウェア乱数の種
	std::mt19937 gen(rd()); // メルセンヌツイスター(高品質乱数生成器)
	std::uniform_int_distribution<> dist(0, 99); // 0～99の一様分布

	int value = dist(gen);

	if (value > -1)
	{
		attackedchara->Detected = true; // 偵察成功
		BFMng->RefreshLogs(attackingchara, attackedchara, ActionName::Scout, 0, attackedchara->Detected);
		return true;
	}
	else
	{
		attackedchara->Detected = false; // 偵察失敗
		BFMng->RefreshLogs(attackingchara, attackedchara, ActionName::Scout, 0, attackedchara->Detected);
		return false;
	}

	BFMng->CreateAbilityLog(attackingchara, ActionName::Scout, 0.0f);
}
