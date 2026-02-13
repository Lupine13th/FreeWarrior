#pragma once

#include "GameObject.h"
#include "SpriteCharacter.h"
#include "TimeManager.h"
#include "MyAccessHub.h"
#include "KeyBindComponent.h"
#include "FieldCharacter.h"

class EffectObject;

class EffectGenerator : public GameComponent
{
private:
	KeyBindComponent* m_keyBind;
	std::unordered_map<wstring, EffectObject*> m_EffectObjectList;
public:
	void GenerateMuzzleFlashEffect(wstring name);
	void GenerateCanonMuzzleFlashEffect(wstring name);
	void GenerateExplosiveEffect(wstring name);
	void GenerateLargeExplosiveEffect(wstring name);
	void GenerateScoutEffect(wstring name);
	void GenerateScoutedEffect(wstring name);
	void GenerateCircleEffect(wstring name);

	EffectObject* GetEffectObject(wstring name)
	{
		return m_EffectObjectList[name];
	};

	// GameComponent を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class EffectObject : public GameComponent
{
protected:
	float m_AnimationTime = 0.0f;
	float m_AnimationInterval = 0.0f;
	float m_CurrentSpriteRotate = 0.0f;
	float m_SpriteRotateInterval = 0.0f;
	float m_SpriteScale = 0.0f;

	const float kFlipWidth = 512.0f;
	const float kFlipHeight = 512.0f;
	const float kFullFlipWidth = 2048.0f;
	const float kFullFlipHeight = 2048.0f;
	
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

	int m_PageCount = 0;
	int m_MaxPageCount = 0;
	int m_LoopCount = 0;
	int m_MaxLoopCount = 0;

	int m_EffectSpriteCount = 0;

	bool m_IsPlaying = false;
	bool m_IsFirstTime = true;

	XMFLOAT3 m_EffectPosition = {};

	XMFLOAT3 m_EffectRotation = {};

	XMFLOAT4 m_PatternRect = {};

	vector<SpriteCharacter*> m_SpriteCharacterList = {};

	TimeManager* m_TimeManager = nullptr;

public:
	virtual ~EffectObject()
	{
		for (SpriteCharacter* character : m_SpriteCharacterList)
		{
			delete character;
		}
		m_SpriteCharacterList.clear();
	}

	void SetEffectPosition(float x, float y, float z)
	{
		m_EffectPosition = { x, y, z };
	};
	void SetEffectRotation(float x, float y, float z)
	{
		m_EffectRotation = { x, y, z };
	};
	void SetEffectScale(float scale)
	{
		m_SpriteScale = scale;
	};
	void DeleteEffect(wstring name);
	void PlayEffect(XMFLOAT3 pos, XMFLOAT3 rot, float scale);
	void StopEffect();
};

class MuzzleFlashEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class CanonMuzzleFlashEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class ExplosiveEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class LargeExplosiveEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class ScoutEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class ScoutedEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};

class CircleEffect : public EffectObject
{
private:

public:
	// EffectObject を介して継承されました
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;
};
