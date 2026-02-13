#pragma once
#include "GameObject.h"
#include "SpriteCharacter.h"
#include "EnumClassList.h"
#include "BattleFieldManager.h"
#include "WordList.h"
#include "TimeManager.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

enum class TitleMenuState
{
	GameStart = 0,
	ExitGame = 1
};

enum class TitleState
{
	Menu,
	Flash,
	NextScene
};

class TitleScene : public GameComponent
{
private:
	vector<std::unique_ptr<SpriteCharacter>> m_Sprite;
	GAME_SCENES m_NextScene;
	int sta = 0;
	int m_MenuIndex = (int)TitleMenuState::GameStart;

	std::vector<std::unique_ptr<SpriteCharacter>> m_WordSprites;
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	//==========テキストの設定==========
	WordList m_WordList;

	const wchar_t* m_ChEnd;

	std::unordered_map<wchar_t, XMFLOAT4> m_FontMap;

	std::wstring m_TextureType = L"JPNHUDTextureNP";

	const wchar_t* m_WordListName = m_WordList.m_chListNewsPaperJ;

	int m_WordCount;

	int MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str);

	XMFLOAT4 m_PatternRect = { 0.0f, 0.0f, 0.0f, 0.0f };

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

	const float kFlipWidth = 512.0f;
	const float kFlipHeight = 512.0f;
	const float kFullFlipWidth = 2048.0f;
	const float kFullFlipHeight = 2048.0f;

	int m_AnimationPages = 0;

	const int kMaxPageCount = 8;

	float m_MenuAnimationCount = 0.0f;

	int m_FlashAnimationPages = 0;

	const int kMaxFlashPageCount = 13;

	float m_FlashAnimationCount = 0.0f;

	TimeManager* m_TimeManager = MyAccessHub::GetTimeManager();

	TitleState m_TitleState = TitleState::Menu;
public:
	virtual void initAction() override;
	virtual bool frameAction() override;
	virtual void finishAction() override;

	void SetNextScene(GAME_SCENES nextSc);
};

