#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "TimeManager.h"
#include "Tween.h"

#include <D3D12Helper.h>

enum class AnimationState
{
	None,
	OnInit,
	Init,
	Run,
	Finish
};

enum class EasingVector
{
	Verticle,
	Horizontal
};

namespace OrderInLayer
{
	constexpr float BackGround = 2.0f;
	constexpr float MoveObject = 1.0f;
	constexpr float Text = 0.5f;
};

class HUDObject : public GameComponent	//HUDの親クラス
{
protected:
	AnimationState m_AnimationState = {};
	vector<const wchar_t*> m_TextureIdList = {};
	vector<std::unique_ptr<Tween>> m_ActiveTweenList = {};
	vector<std::unique_ptr<SpriteCharacter>> m_SpriteList = {};
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

	const float kFlipWidth = 512.0f;
	const float kFlipHeight = 512.0f;
	const float kFullFlipWidth = 2048.0f;
	const float kFullFlipHeight = 2048.0f;

	int m_AnimationPages = 0;

	float m_FlipAnimationCount = 0.0f;
	float m_FlipDuration = 0.0f;

	float m_DelayCount = 0.0f;
	float m_MaxDelayCount = 0.0f;

	XMFLOAT4 m_PatternRect = { 0.0f, 0.0f, 1.0f, 1.0f };

	const vector<XMFLOAT2> kPageAreaList = 
	{ 
		XMFLOAT2{0.0f, 0.0f},
		XMFLOAT2{1.0f, 0.0f},
		XMFLOAT2{2.0f, 0.0f},
		XMFLOAT2{3.0f, 0.0f},
		XMFLOAT2{0.0f, 1.0f},
		XMFLOAT2{1.0f, 1.0f},
		XMFLOAT2{2.0f, 1.0f},
		XMFLOAT2{3.0f, 1.0f},
		XMFLOAT2{0.0f, 2.0f},
		XMFLOAT2{1.0f, 2.0f},
		XMFLOAT2{2.0f, 2.0f},
		XMFLOAT2{3.0f, 2.0f},
		XMFLOAT2{0.0f, 3.0f},
		XMFLOAT2{1.0f, 3.0f},
		XMFLOAT2{2.0f, 3.0f},
		XMFLOAT2{3.0f, 3.0f}
	};

	const std::unordered_map<string, const wchar_t*, const wchar_t*> kFontAsset;
public:
	void SetAnimationState(AnimationState state)
	{
		m_AnimationState = state;
	}
	void AddTextureId(const wchar_t* texId)
	{
		m_TextureIdList.push_back(texId);
	}
	AnimationState GetAnimationState()
	{
		return m_AnimationState;
	}
	const wchar_t* GetTextureId(int index)
	{
		return m_TextureIdList[index];
	}
	Tween* GetActiveTween(int index)
	{
		return m_ActiveTweenList[index].get();
	}
	void SetEasingAnimation(SpriteCharacter* sprite, EasingVector vector, float startPos, float endPos, float duration, const std::function<float(float, float, float, float)>& easing);
	void FlipAnimation(SpriteCharacter* sprite);

	void SetShaderResoruce();
	virtual void ResetHUD();
};

class HUDTextObject : public HUDObject	//HUD(テキストあり)の親クラス
{
protected:
	int m_SpriteCount = 0;

	const wchar_t* kWordListEnd;
	const wchar_t* m_FontTextureId;
	const wchar_t* m_FontWordList;

	unordered_map<string, const wchar_t*> m_TextList;

	std::unordered_map<wchar_t, XMFLOAT4> m_FontMap;

	std::vector<std::unique_ptr<SpriteCharacter>> m_WordSpriteList;

	WordList m_WordList;

	int MakeSpriteString(int startIndex, float posX, float posY, float width, float height, const wchar_t* string, XMFLOAT3 color);
	int MakeSpriteStringRightAligned(int startIndex, float posX, float posY, float width, float height, const wchar_t* string, XMFLOAT3 color);

	void SetFont(const wchar_t* fontTextureId, const wchar_t* fontWordList);
};

class HUDManager : public GameComponent		//HUD全体管理
{
private:
	std::unordered_map<string, HUDObject*> m_HUDList;
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void AddHUDObject(string name, HUDObject* hudObj)
	{
		m_HUDList[name] = hudObj;
	}
	HUDObject* GetHUDObject(string name)
	{
		return m_HUDList[name];
	}
	int GetHUDObjectCount()
	{
		return m_HUDList.size();
	}

	void ResetHUDWhenMoveCursor();
};

class MeterHUD : public HUDTextObject	//兵数・士気バーHUD
{
private:
	const float kSoldierArrowPosY = 55.0f;
	const float kMoraleArrowPosY = -45.0f;
	const float kArrowLeftPosX = 305.0f;
	const float kArrowsRenge = 140.0f;
	const float kBarMovingTime = 0.5f;
	const float kMeterPositionX = 380.0f;
	const float kSoldierMeterPositionY = 70.0f;
	const float kMoraleMeterPositionY = -30.0f;

	const XMFLOAT2 kSoldierTextPos = { 341.0f, 110.0f };
	const XMFLOAT2 kMoraleTextPos = { 341.0f, 10.0f };

	float m_SoldierArrowEndPosX = 0.0f;
	float m_MoraleArrowEndPosX = 0.0f;
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void AnimateBarsTo(FieldCharacter* targetChara);
	void SetSoldiersPersent(FieldCharacter* targetChara);
	void SetMoralePersent(FieldCharacter* targetChara);
};

class StatusHUD : public HUDObject		//右上部ステータスHUD
{
private:
	
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void SetLabelColor();
	void SetRender(int index);
};

class AbilityHUD : public HUDTextObject	//行動ノートHUD
{
private:
	enum class AbilityHUDState
	{
		None,
		FolderUp,
		FileUp,
		FileDown,
		FileOpen,
		Stay
	};

	AbilityHUDState m_AbilityHUDState = AbilityHUDState::None;

	const XMFLOAT3 kAbillityNotePositionX = { -250.0f, 0.0f, 250.0f };
	const XMFLOAT2 kDoubleAbillityNotePositionX = { -150.0f, 150.0f };
	const XMFLOAT2 kAbillityBackGroundPosition = { 0.0f, 0.0f };
	const float kAbillityNotePositionY = 0.0f;
	const float kOriginAbillityNoteScale = 400.0f;
	const float kMaxAbillityNotePositionX = 600.0f;
	const float kAbillityNameTextPositionY = 50.0f;
	const float kAbillityDetailTextPositionY = -50.0f;
	const float kBackGroundAnimationCount = 0.5f;

	float m_AnimationCount = 0.0f;
	float m_AbillityNoteScale = 400.0f;
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void ResetHUD() override;
};

class StatusText : public HUDTextObject		//ステータスHUD上のテキスト
{
private:
	const XMFLOAT2 kCharacterNameTextPos = { 320.0f, 155.0f };
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class GuideHUD : public HUDTextObject		//操作説明HUD
{
private:
	const XMFLOAT2 kGuideTextPos = { -430.0f, -235.0f };
	const XMFLOAT2 kGuideTextPos2 = { -430.0f, -260.0f };
	const XMFLOAT2 kGuideBackGroundPos = { -100.0f, -230.0f };
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class SideMenuHUD : public HUDTextObject	//サイドメニューHUD
{
private:
	float kTextPositionX = -100.0f;

	const vector<float> kTextPositionY = 
	{
		90.0f,
		40.0f,
		-10.0f
	};

	const XMFLOAT2 kSideMenuBackGroundPos = { 0.0f, 0.0f };
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class CurrentTerrainHUD : public HUDTextObject	//地形表示HUD
{
private:
	const float kTextPositionX = 320.0f;

	const vector<float> kTextPositionY =
	{
		-160.0f, -190.0f, -210.0f, -240.0f
	};

	const XMFLOAT2 kBackGroundPos = { 400.0f, -200.0f };
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class BattleCameraHUD : public HUDTextObject	//戦闘カメラHUD
{
private:
	const XMFLOAT2 kAttackerTextPos = { -400.0f, 220.0f };
	const XMFLOAT2 kDefenderTextPos = { 200.0f, -220.0f };

	const XMFLOAT2 kBackGroundPos = { 0.0f, 0.0f };
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class MainMenuHUD : public HUDTextObject
{
private:
	const vector<const wchar_t*> kMenuTextList =
	{
		L"攻撃",
		L"移動",
		L"行動",
		L"待機",
		L"キャンセル",
		L">",
	};

	const vector<float> kMenuTextPositionY =
	{
		185.0f,
		105.0f,
		25.0f,
		-55.0f,
		-135.0f
	};

	const vector<float> kNixieBasePositionY =
	{
		185.0f,
		105.0f,
		25.0f,
		-55.0f,
		-135.0f
	};

	const vector<float> kNixieTubePositionX =
	{ 
		-450.0f, 
		-410.0f, 
		-370.0f, 
		-330.0f, 
		-290.0f, 
		-250.0f 
	};

	const vector<float> kNixieTubePositionY = 
	{
		180.0f,
		100.0f,
		20.0f,
		-60.0f,
		-140.0f 
	};

	const float kTextPositionX = -410.0f;
	const float kArrowPositionX = -450.0f;
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class DamageEffectHUD : public HUDObject	//ダメージエフェクトUI
{
private:
	const XMFLOAT3 kLowDamageEffectPosition = { -200.0f, -75.0f, 0.0f };
	const XMFLOAT3 kMidDamageEffectPosition = { 170.0f, -100.0f, 0.0f };
	const XMFLOAT3 kHighDamageEffectPosition = { -170.0f, -125.0f, 0.0f };

	const float kSmallDamageEffectScale = 150.0f;
	const float kLargeDamageEffectScale = 300.0f;

	int m_EffectCount = 0;

public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void SetDamageEffect(float damageRatio);
};
