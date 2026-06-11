#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "BattleFieldManager.h"
#include "BattleSceneManager.h"
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
	Vertical,
	Horizontal
};

namespace OrderInLayer
{
	constexpr float BackGround = 2.0f;
	constexpr float MoveObject = 1.0f;
	constexpr float Text = 0.5f;
	constexpr float OnTextObject = 0.1f;
};

class HUDObject : public GameComponent	//HUDの親クラス
{
protected:
	AnimationState m_AnimationState = {};
	vector<const wchar_t*> m_TextureIdList = {};
	vector<std::unique_ptr<Tween>> m_ActiveTweenList = {};
	vector<std::unique_ptr<SpriteCharacter>> m_SpriteList = {};
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	BattleSceneManager* BSMng = MyAccessHub::GetBattleSceneManager();
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

	XMFLOAT4 m_PatternRect = { 0.0f, 0.0f, 1.0f, 1.0f };	//UV座標のRect　左上原点　単位はテクスチャ全体に対する割合

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
	void MakeSpriteObject(const wchar_t* textureId, wstring cameraLabel, wstring pipeLine, XMFLOAT4 pattern, XMFLOAT4 color);
	void SetEasingAnimation(SpriteCharacter* sprite, EasingVector vector, float startPos, float endPos, float duration, const std::function<float(float, float, float, float)>& easing);
	void FlipAnimation(SpriteCharacter* sprite, int pages, float duration);

	void SetShaderResoruce();
	virtual void ResetHUD();

	void RefreshEasingAnimation();
};

class HUDTextObject : public HUDObject	//HUD(テキストあり)の親クラス
{
protected:
	int m_SpriteCount = 0;

	float m_TextDuration = 0.0f;

	const wchar_t* kWordListEnd;
	const wchar_t* m_FontTextureId;
	const wchar_t* m_FontWordList;

	unordered_map<string, wstring> m_TextList;

	std::unordered_map<wchar_t, XMFLOAT4> m_FontMap;

	std::vector<std::unique_ptr<SpriteCharacter>> m_WordSpriteList;

	WordList m_WordList;

	int MakeSpriteStringLeftEdge(int startIndex, float posX, float posY, float width, float height, const wchar_t* string, XMFLOAT3 color);
	int MakeSpriteStringRightEdge(int startIndex, float posX, float posY, float width, float height, const wchar_t* string, XMFLOAT3 color);
	int MakeSpriteStringMid(int startIndex, float posX, float posY, float width, float height, const wchar_t* string, XMFLOAT3 color);

	void SetFont(const wchar_t* fontTextureId, const wchar_t* fontWordList);
};

class HUDManager : public GameComponent		//HUD全体管理
{
private:
	std::unordered_map<string, HUDObject*> m_HUDList;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

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

//兵数・士気表示用ゲージのHUD
class MeterHUD : public HUDTextObject	
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

	Platoon* character = nullptr;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void AnimateBarsTo(Platoon* targetChara);
	void SetSoldiersPersent(Platoon* targetChara);
	void SetMoralePersent(Platoon* targetChara);
};

//「偵察カメラ」HUD
class StatusHUD : public HUDObject
{
private:
	
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void SetLabelColor();
	void SetRender(int index);
};

//アビリティ選択ノートHUD
class AbilityHUD : public HUDTextObject	
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
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void ResetHUD() override;
};

class StatusText : public HUDTextObject		//ステータスHUD上のテキスト
{
private:
	const XMFLOAT2 kCharacterNameTextPos = { 320.0f, 155.0f };
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class GuideHUD : public HUDTextObject		//操作説明HUD
{
private:
	const XMFLOAT2 kGuideTextPos = { -430.0f, -235.0f };
	const XMFLOAT2 kGuideTextPos2 = { -430.0f, -260.0f };
	const XMFLOAT2 kGuideBackGroundPos = { -100.0f, -230.0f };
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
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
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
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
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class BattleCameraHUD : public HUDTextObject	//戦闘カメラHUD
{
private:
	const XMFLOAT2 kAttackerTextPos = { -400.0f, 220.0f };
	const XMFLOAT2 kAttackerMoveTextPos = { -75.0f, 175.0f };
	const XMFLOAT2 kDefenderTextPos = { 200.0f, -220.0f };

	const XMFLOAT2 kBackGroundPos = { 0.0f, 0.0f };
	const XMFLOAT2 kAttackerTextBackGroundPos = { 0.0f, 175.0f };
	const XMFLOAT2 kDefenderTextBackGroundPos = { 0.0f, -200.0f };

	const map<Animations, wstring> kAttackerMoveTexts =
	{
		{ Animations::Attack, L"攻撃" },
		{ Animations::ConcentratedFire, L"集中射撃" },
		{ Animations::BayonetCharge, L"銃剣突撃" },
		{ Animations::Scout, L"偵察" }
	};
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void SetActionText(wstring text)
	{
		m_TextList["攻撃側行動"] = text;
	}

	wstring SetAttackerMoveText(Animations anim)
	{
		if (kAttackerMoveTexts.count(anim) > 0)
		{
			return kAttackerMoveTexts.at(anim);
		}
		else
		{
			return L"";
		}
	}
};

class MainMenuHUD : public HUDTextObject		//メインメニューHUD
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

	const float kMenuArrowPositionX = -450.0f;

	const float kMenuTextPositionX = -410.0f;

	const vector<float> kMenuTextPositionY =
	{
		185.0f,
		105.0f,
		25.0f,
		-55.0f,
		-135.0f
	};

	const float kNixieBaseDefaultPositionX = -850.0f;

	const float kNixieBaseMovedPositionX = -350.0f;

	const vector<float> kNixieBasePositionY =
	{
		140.0f,
		60.0f,
		-20.0f,
		-100.0f,
		-180.0f
	};

	const vector<float> kNixieTubeMovedPositionX =
	{ 
		-450.0f, 
		-410.0f, 
		-370.0f, 
		-330.0f, 
		-290.0f, 
		-250.0f 
	};

	const vector<float> kNixieTubeDefaultPositionX =
	{ 
		-950.0f, 
		-910.0f, 
		-870.0f, 
		-830.0f, 
		-790.0f, 
		-750.0f 
	};

	const vector<float> kNixieTubePositionY = 
	{
		180.0f,
		100.0f,
		20.0f,
		-60.0f,
		-140.0f 
	};

	const float kMenuBaseDefaultPositionX = -980.0f;

	const float kMenuBaseMovedPositionX = -480.0f;

	const float kMenuBasePositionY = 60.0f;

	const float kMenuGageDefaultPositionX = -700.0f;

	const float kMenuGageMovedPositionX = -350.0f;

	const vector<float> kMenuGagePositionY =
	{
		143.0f,
		63.0f,
		-17.0f,
		-97.0f,
		-177.0f
	};

	const float m_PosZ = 2.0f;

	const float kMenuAnimationTime = 0.5f;

	float m_MenuAnimationCount = 0.0f;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class DamageEffectHUD : public HUDTextObject	//ダメージエフェクトUI
{
private:
	const XMFLOAT3 kLowDamageEffectPosition = { -200.0f, -75.0f, 0.0f };
	const XMFLOAT3 kMidDamageEffectPosition = { 170.0f, -100.0f, 0.0f };
	const XMFLOAT3 kHighDamageEffectPosition = { -170.0f, -125.0f, 0.0f };

	const float kSmallDamageEffectScale = 150.0f;
	const float kLargeDamageEffectScale = 300.0f;

	int m_EffectCount = 0;

public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void SetDamageEffect(float damageRatio);
};

class SuperiorityGaugeHUD : public HUDTextObject	//制圧ゲージのテキスト
{
private:
	const float kGagePositionY = 250.0f;		//優勢ゲージのY座標
	const float kGageLeftPosition = -150.0f;	//優勢ゲージの左端X座標
	const float kGageRightPosition = 150.0f;	//優勢ゲージの右端X座標
	float m_NextGagePercent = 0.0f;				//優勢ゲージの現在の割合　1.0fで味方完全優勢　0.0fで敵完全優勢
	float m_CurrentGagePercent = 0.0f;			//現在のゲームのパーセント ゲージにアニメーションを付ける用
	float m_AnimationCount = 0.0f;				//ゲージアニメーションのカウント
	float m_GageSizeX = 0.0f;					//ゲージのX軸スケール
	XMFLOAT2 m_StartGageSizeX = { 0.0f, 0.0f };				//ゲージアニメーション開始時のゲージのX軸スケール
	XMFLOAT2 m_EndGageSizeX = { 0.0f, 0.0f };				//ゲージアニメーション終了時のゲージのX軸スケール

	bool m_IsFirstTime = true;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void SetGagePercent(XMFLOAT2 strengthValues);
};

class LoadAnimationHUD : public HUDTextObject	//ロード画面のテキスト＆背景
{
private:
	//テキストの座標
	XMFLOAT2 pos = { -400.0f, 250.0f } ;

	//アニメーション時間
	float m_AnimationCount = 0.0f;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class TurnEndHUD : public HUDTextObject			//ターンエンド画面
{
private:
	//テキストの座標
	XMFLOAT2 pos = { -200.0f, 200.0f };

	//HUD表示までの遅延時間
	float m_DelayCount = 0.0f;		

	//HUD表示までの遅延時間
	const float kDelayCount = 3.0f;		

	//現在のアニメーション時間
	float m_AnimationCount = 0.0f;

	//冒頭のアニメーション時間
	float m_InitAnimationCount = 0.0f;

	//アニメーション時間
	const float kAnimationTime = 0.2f;

	//矢印の角度
	float m_ArrowRotation = 0.0f;

	//矢印の角度
	float m_StartArrowRotation = 0.0f;

	const vector<float> kBackGroundPosY =
	{
		-450.0f,
		-150.0f
	};

	const vector<float> kArrowPosY =
	{
		-525.0f,
		-225.0f
	};

	const vector<float> kTextBackGroundPosY =
	{
		-100.0f,
		200.0f
	};
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class TurnHUD : public HUDTextObject			//ターンエンド画面
{
private:
	float m_AnimationCount = 0.0f;
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class EndingHUD : public HUDTextObject			//ターンエンド画面
{
private:
	enum class EndingState
	{
		None,
		FadeIn,
		NewsPaperDrop,
		NewsPaperZoom,
		NewsPaper,
		NewsPaperClose,
		Result,
		Finish
	};

	enum class ResultState
	{
		None,
		Turns,
		TurnsPlus,
		KillCount,
		KillCountPlus,
		KilledCount,
		KilledCountPlus,
		Score,	
		ScorePlus,
		PressSpace
	};

	EndingState m_EndingState = EndingState::None;
	ResultState m_ResultState = ResultState::None;

	int m_NewsPaperAnimationPages = 5;

	float m_AnimationCount = 0.0f;

	float m_NewsPaperScale = 300.0f;

	const XMFLOAT2 kNewsPaperDropPositionY = { 300.0f, 0.0f };

	const float kFadeInDuration = 0.5f;
	const float kNewsPaperDropDuration = 0.5f;
	const float kNewsPaperRotateDuration = 0.7f;
	const float kResultTextPosX = -200.0f;			//リザルトのテキストのX座標
	const float kPressSpaceTextPosX = -50.0f;		//PressSpaceテキストのX座標
	const float kTurnTextPosY = 50.0f;
	const float kKillTextPosY = 0.0f;
	const float kKilledTextPosY = -50.0f;
	const float kLastScoreTextPosY = -125.0f;
	const float kPressSpaceTextPosY = -200.0f;

	const wchar_t* kResultTextList[5] =
	{
		L"経過ターン:",
		L"倒した部隊数:",
		L"倒された部隊数:",
		L"スコア:",
		L"スペースキーで終了"
	};

	const XMFLOAT3 kNewsPaperTextColor = { 0.0f, 0.0f, 0.0f };
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	int GetLastScoreValue();

	void SetEndingState(EndingState state)
	{
		m_EndingState = state;
	}
};

class BattlePredictionHUD : public HUDTextObject			//ターンエンド画面
{
private:
	float m_AnimationCount = 0.0f;
	float m_GreenBarSizeX = 0.0f;
	float m_RedBarSizeX = 0.0f;

	float m_GreenBarPositionX = 0.0f;
	float m_RedBarPositionX = 0.0f;

	float m_RedBarRightEdgePositionX = 0.0f;

	float m_RedBarAlpha = 0.0f;

	const float kDogtagAnimationDuration = 0.5f;
	const float kAlliesTextPositionX = -400.0f;
	const float kEnemyTextPositionX = 100.0f;
	const float kGeneralTextPositionY = -30.0f;
	const float kPlatoonTextPositionY = -70.0f;
	const float kOriginBarSizeX = 300.0f;

	const float kBarLeftEdgePositionX = 100.0f;

	const XMFLOAT2 kDogtagPositionY = { -300.0f, -50.0f };
	const XMFLOAT2 kDogtagPositionX = { -150.0f, 150.0f };

	const XMFLOAT2 kBarPositionY = { -250.0f, 0.0f };
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;
};

class DamageUI : public HUDTextObject			//ターンエンド画面
{
private:
	enum class DamageUIState
	{
		None,
		MovingGreen,
		MovingRed,
	};

	DamageUIState m_DamageUIState = DamageUIState::None;

	float m_AnimationCount = 0.0f;

	float m_StartGreenBarSizeX = 0.0f;
	float m_EndGreenBarSizeX = 0.0f;

	float m_StartGreenBarPositionX = 0.0f;
	float m_EndGreenBarPositionX = 0.0f;

	float m_GreenBarPositionX = 0.0f;
	float m_GreenBarSizeX = 0.0f;

	float m_StartRedBarSizeX = 0.0f;
	float m_EndRedBarSizeX = 0.0f;

	float m_StartRedBarPositionX = 0.0f;
	float m_EndRedBarPositionX = 0.0f;

	float m_RedBarPositionX = 0.0f;
	float m_RedBarSizeX = 0.0f;

	float m_CurrentDamage = 0.0f;

	XMFLOAT2 m_DamageTextPosition = { 0.0f, 0.0f };

	const XMFLOAT2 kOriginBarPosition = { 50.0f, -80.0f };
	const XMFLOAT2 kBarScale = { 100.0f, 10.0f };
	const float kBarLeftEdgePositionX = 0.0f;
	const XMFLOAT2 kOriginDamageTextPosition = { 50.0f, -50.0f };
public:
	void InitAction() override;
	bool FrameAction() override;
	void FinishAction() override;

	void SetDamage(float damage, float maxSoldiers, float soldiers);
};
