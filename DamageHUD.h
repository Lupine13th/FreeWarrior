#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "TimeManager.h"
#include "MyAccessHub.h"
#include "EnumClassList.h"

class DamageHUD : public GameComponent
{
private:
	TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

	std::vector<std::unique_ptr<SpriteCharacter>> m_sprite;	//0:緑ゲージ 1:赤ゲージ 2:黒ゲージ

	float animCount = 0.0f;

	float OriginBarSizeX = 100.0f;
	float OrigenBarPosX = 50.0f;

	//HPバーの大きさ
	float GreenBarSizeX = 4.0f;

	//HPバーの赤ゲージの大きさ
	float RedBarSizeX = 4.0f;

	//HPバーの減った分の大きさ
	float BlackBarSizeX = 4.0f;

	//HPバーの位置
	float GreenBarPosX = 50.0f;

	//HPバーの赤ゲージの位置
	float RedBarPosX = 50.0f;

	//HPバーの減った分の大きさ
	float BlackBarPosX = 50.0f;

	const float kBarPosLeftX = 0.0f;    // ゲージの左端の固定座標

	const float kBarPosY = -80.0f;     // Y座標固定
	const float kBarPosX = -50.0f;     // Y座標固定
	float startGreenSizeX = 0.0f;
	float endGreenSizeX = 0.0f;
	float startRedSizeX = 0.0f;
	float endRedSizeX = 0.0f;

	float m_DelayCount = 0.0f;
	const float kDelayTime = 1.0f;

	DamegeAnimationState m_DamegeAnimationState = DamegeAnimationState::None;
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

	void SetDamage(float damage, float maxsol, float sol);

	float Damage = 0.0f;
	float MaxSol = 0.0f;
	float Sol = 0.0f;
	float solPersent = 0.0f;

	DamegeAnimationState GetDamegeAnimationState()
	{
		return m_DamegeAnimationState;
	}
};

