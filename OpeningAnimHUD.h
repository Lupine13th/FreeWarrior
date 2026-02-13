#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "TimeManager.h"

#include "vector"

enum class OpeningState
{
	None,
	FadeAnimation,
    BeforeAnim,
	WinTarget,
	DefTarget,
	AfterAnim
};

class OpeningAnimHUD : public GameComponent
{
private:
	vector<std::unique_ptr<SpriteCharacter>> o_sprite;  //0:ラジオ本体 1:紙の部分(後ろのほうに置く) 2:勝利条件 3:敗北条件
    BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

	XMFLOAT3 RadioPos = { 1000.0f, 0.0f, 0.0f};
	XMFLOAT3 TarWinPos = { 0.0f, -500.0f, 0.0f};
	XMFLOAT3 TarDefPos = { 0.0f, -500.0f, 0.0f};
	XMFLOAT3 FadePos = { 0.0f, 0.0f, 0.0f};

	XMFLOAT3 RadioScale = { 500.0f, 500.0f, 0.0f};
	XMFLOAT3 TarWinScale = { 550.0f, 550.0f, 0.0f};
	XMFLOAT3 TarDefScale = { 550.0f, 550.0f, 0.0f};
	XMFLOAT3 FadeScale = { 1200.0f, 1000.0f, 0.0f};

	bool m_InitDefeat = false;
public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

	float OPAnimCount = 0.0f;

	OpeningState oState = OpeningState::FadeAnimation;
};

