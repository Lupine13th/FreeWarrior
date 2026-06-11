#include <iostream>
#include <random>

#include "Abilities.h"
#include "BattleFieldManager.h"

void Abilities::ConcentratedFire(Platoon* attackingchara, Platoon* attackedchara)	//集中射撃アビリティ
{
	int damage = BFMng->CalculateDamage(ActionName::ConcentratedFire, attackingchara, attackedchara);

	HUDObject* damageUIobj = MyAccessHub::GetHUDManager()->GetHUDObject("DamageUI");
	if (DamageUI* damageUI = dynamic_cast<DamageUI*>(damageUIobj))
	{
		damageUI->SetDamage(damage, attackedchara->GetMaxSoldiers(), attackedchara->GetSoldiers());
	}

	BFMng->GetDamageHUD()->SetDamage(damage, attackedchara->GetMaxSoldiers(), attackedchara->GetSoldiers());	//ダメージHUD表示

	attackingchara->SetMorale(attackingchara->GetMorale() - 20);				//攻撃側の士気を減少
	attackedchara->SetSoldier(attackedchara->GetSoldiers() - damage);	//防御側の兵力を減少

	attackingchara->SetIsActioned(true);	//攻撃側の行動済みフラグを立てる

	BFMng->CreateAbilityLog(attackingchara, ActionName::ConcentratedFire, damage);	//行動ログ作成

	BFMng->SetStrengthValues();	//優勢ゲージ更新
}

void Abilities::BayonetCharge(Platoon* attackingchara, Platoon* attackedchara)	//銃剣突撃アビリティ
{
	int damage = BFMng->CalculateDamage(ActionName::ConcentratedFire, attackingchara, attackedchara);

	HUDObject* damageUIobj = MyAccessHub::GetHUDManager()->GetHUDObject("DamageUI");
	if (DamageUI* damageUI = dynamic_cast<DamageUI*>(damageUIobj))
	{
		damageUI->SetDamage(damage, attackedchara->GetMaxSoldiers(), attackedchara->GetSoldiers());
	}

	BFMng->GetDamageHUD()->SetDamage(damage * 1.2f, attackedchara->GetMaxSoldiers(), attackedchara->GetSoldiers());	//ダメージHUD表示

	attackingchara->SetMorale(attackingchara->GetMorale() - 10);										//攻撃側の士気を減少
	attackingchara->SetSoldier(attackingchara->GetSoldiers() - attackingchara->GetSoldiers() * 0.1);	//攻撃側の兵力を減少
	attackedchara->SetSoldier(attackedchara->GetSoldiers() - damage * 1.2f);							//防御側の兵力を減少
	attackingchara->SetIsActioned(true);	//攻撃側の行動済みフラグを立てる

	BFMng->CreateAbilityLog(attackingchara, ActionName::BayonetCharge, damage * 1.2f);						//行動ログ作成

	BFMng->SetStrengthValues();																				//優勢ゲージ更新
}

bool Abilities::Scout(Platoon* attackingchara, Platoon* attackedchara)
{
	std::random_device rd;  // ハードウェア乱数の種
	std::mt19937 gen(rd()); // メルセンヌツイスター(高品質乱数生成器)
	std::uniform_int_distribution<> dist(0, 99); // 0～99の一様分布

	int value = dist(gen);

	if (value > -1)
	{
		attackedchara->SetIsDetected(true); // 偵察成功
		return true;
	}
	else
	{
		attackedchara->SetIsDetected(false); // 偵察失敗
		return false;
	}

	attackingchara->SetIsActioned(true);	//攻撃側の行動済みフラグを立てる

	BFMng->CreateAbilityLog(attackingchara, ActionName::Scout, 0.0f);	//行動ログ更新
}


