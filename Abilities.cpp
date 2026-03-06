#include <iostream>
#include <random>

#include "Abilities.h"
#include "BattleFieldManager.h"

void Abilities::ConcentratedFire(FieldCharacter* attackingchara, FieldCharacter* attackedchara)	//集中射撃アビリティ
{
	int damage = 0;

	if (BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Forest || BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Tower)	//地形が森林、もしくは監視塔
	{
		damage = attackingchara->CharaPower * 1.2;
	}
	else
	{
		damage = attackingchara->CharaPower;
	}

	if (BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::Hills || BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::River)		//地形が丘陵、もしくは河川
	{
		damage = damage * 0.75;
	}

	if (!attackedchara->Detected)	//攻撃対象の偵察が完了していない場合
	{
		damage = damage * 0.75;
	}

	BFMng->GetDamageHUD()->SetDamage(damage * 1.2f, attackedchara->CharaMaxSoldiers, attackedchara->CharaSoldiers);	//ダメージHUD表示

	attackingchara->CharaMorales = attackingchara->CharaMorales - 20;				//攻撃側の士気を減少
	attackedchara->CharaSoldiers = attackedchara->CharaSoldiers - damage * 1.2f;	//防御側の兵力を減少

	BFMng->CreateAbilityLog(attackingchara, ActionName::ConcentratedFire, damage * 1.2f);	//行動ログ作成

	BFMng->SetStrengthValues();	//優勢ゲージ更新

	BFMng->RefreshLogs(attackingchara, attackedchara, ActionName::ConcentratedFire, damage * 1.2f, false);	//行動ログ更新
}

void Abilities::BayonetCharge(FieldCharacter* attackingchara, FieldCharacter* attackedchara)	//銃剣突撃アビリティ
{
	int damage = 0;

	if (BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Forest || BFMng->GetFieldSquaresList()[attackingchara->CharaPos]->terrainname == Terrain::Tower) //地形が森林、もしくは監視塔
	{
		damage = attackingchara->CharaPower * 1.2;
	}
	else
	{
		damage = attackingchara->CharaPower;
	}

	if (BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::Hills || BFMng->GetFieldSquaresList()[attackedchara->CharaPos]->terrainname == Terrain::River)	//地形が丘陵、もしくは河川
	{
		damage = damage * 0.75;
	}

	BFMng->GetDamageHUD()->SetDamage(damage * 1.2f, attackedchara->CharaMaxSoldiers, attackedchara->CharaSoldiers);	//ダメージHUD表示

	attackingchara->CharaMorales = attackingchara->CharaMorales - 10;										//攻撃側の士気を減少
	attackingchara->CharaSoldiers = attackingchara->CharaSoldiers - attackingchara->CharaSoldiers * 0.1;	//攻撃側の兵力を減少
	attackedchara->CharaSoldiers = attackedchara->CharaSoldiers - damage * 1.2f;							//防御側の兵力を減少

	BFMng->CreateAbilityLog(attackingchara, ActionName::BayonetCharge, damage * 1.2f);						//行動ログ作成

	BFMng->SetStrengthValues();																				//優勢ゲージ更新

	BFMng->RefreshLogs(attackingchara, attackedchara, ActionName::BayonetCharge, damage * 1.2f, false);		//行動ログ更新
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

	BFMng->CreateAbilityLog(attackingchara, ActionName::Scout, 0.0f);	//行動ログ更新
}
