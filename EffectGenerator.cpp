#include "EffectGenerator.h"
#include "SceneManager.h"
#include <DirectXMath.h>

void MuzzleFlashEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 1;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.1f;
	m_MaxPageCount = 5;
	m_MaxLoopCount = 5;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(1, 1, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"MuzzleFlashTexture");
		spriteCharacter->AddCameraLabel(L"AttackerCamera");
		spriteCharacter->AddCameraLabel(L"DefenderCamera");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool MuzzleFlashEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleX = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationX(localAngleX);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount == m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void MuzzleFlashEffect::finishAction()
{
}

void EffectObject::DeleteEffect(wstring name)
{
	SceneManager* scene = dynamic_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	//for (SpriteCharacter* character : m_SpriteCharacterList) {
	//	delete character;
	//}
	//m_SpriteCharacterList.clear();
	//engine->RemoveGameObject(MyAccessHub::GetEffectGenerator()->GetEffectObject(name));
	//this->getGameObject()->cleanupGameObject();
}

void EffectObject::PlayEffect(XMFLOAT3 pos, XMFLOAT3 rot, float scale)
{
	m_EffectPosition = { pos.x, pos.y, pos.z };
	m_EffectRotation = { rot.x, rot.y, rot.z };
	m_SpriteScale = scale;
	m_IsPlaying = true;
}

void EffectObject::StopEffect()
{
	m_IsPlaying = false;
	m_IsFirstTime = true;
	m_PageCount = 0;
	m_LoopCount = 0;
	m_AnimationTime = 0.0f;
}

void EffectGenerator::GenerateMuzzleFlashEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* muzzleFlashData = new CharacterData;
	GameObject* muzzleFlashEffectObject = new GameObject(muzzleFlashData);
	MuzzleFlashEffect* muzzleFlashEffect = new MuzzleFlashEffect;
	muzzleFlashEffect->SetEffectPosition(0, 0, 0);
	muzzleFlashEffectObject->addComponent(muzzleFlashEffect);
	m_EffectObjectList[name] = muzzleFlashEffect;
	engine->AddGameObject(muzzleFlashEffectObject);
}

void EffectGenerator::GenerateCanonMuzzleFlashEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* canonMuzzleFlashData = new CharacterData;
	GameObject* canonMuzzleFlashEffectObject = new GameObject(canonMuzzleFlashData);
	CanonMuzzleFlashEffect* canonMuzzleFlashEffect = new CanonMuzzleFlashEffect;
	canonMuzzleFlashEffect->SetEffectPosition(0, 0, 0);
	canonMuzzleFlashEffectObject->addComponent(canonMuzzleFlashEffect);
	m_EffectObjectList[name] = canonMuzzleFlashEffect;
	engine->AddGameObject(canonMuzzleFlashEffectObject);
}

void EffectGenerator::GenerateExplosiveEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* explosiveData = new CharacterData;
	GameObject* explosiveEffectObject = new GameObject(explosiveData);
	ExplosiveEffect* explosiveEffect = new ExplosiveEffect;
	explosiveEffect->SetEffectPosition(0, 0, 0);
	explosiveEffectObject->addComponent(explosiveEffect);
	m_EffectObjectList[name] = explosiveEffect;
	engine->AddGameObject(explosiveEffectObject);
}

void EffectGenerator::GenerateLargeExplosiveEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* largeExplosiveData = new CharacterData;
	GameObject* largeExplosiveEffectObject = new GameObject(largeExplosiveData);
	LargeExplosiveEffect* largeExplosiveEffect = new LargeExplosiveEffect;
	largeExplosiveEffect->SetEffectPosition(0, 0, 0);
	largeExplosiveEffectObject->addComponent(largeExplosiveEffect);
	m_EffectObjectList[name] = largeExplosiveEffect;
	engine->AddGameObject(largeExplosiveEffectObject);
}

void EffectGenerator::GenerateScoutEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* scoutData = new CharacterData;
	GameObject* scoutEffectObject = new GameObject(scoutData);
	ScoutEffect* scoutEffect = new ScoutEffect;
	scoutEffect->SetEffectPosition(0, 0, 0);
	scoutEffectObject->addComponent(scoutEffect);
	m_EffectObjectList[name] = scoutEffect;
	engine->AddGameObject(scoutEffectObject);
}

void EffectGenerator::GenerateScoutedEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* scoutedData = new CharacterData;
	GameObject* scoutedEffectObject = new GameObject(scoutedData);
	ScoutedEffect* scoutedEffect = new ScoutedEffect;
	scoutedEffect->SetEffectPosition(0, 0, 0);
	scoutedEffectObject->addComponent(scoutedEffect);
	m_EffectObjectList[name] = scoutedEffect;
	engine->AddGameObject(scoutedEffectObject);
}

void EffectGenerator::GenerateCircleEffect(wstring name)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* circleData = new CharacterData;
	GameObject* circleEffectObject = new GameObject(circleData);
	CircleEffect* circleEffect = new CircleEffect;
	circleEffect->SetEffectPosition(0, 0, 0);
	circleEffectObject->addComponent(circleEffect);
	m_EffectObjectList[name] = circleEffect;
	engine->AddGameObject(circleEffectObject);
}

void EffectGenerator::initAction()
{
	SceneManager* scene = dynamic_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	m_keyBind = dynamic_cast<KeyBindComponent*>(scene->getKeyComponent());
}

bool EffectGenerator::frameAction()
{
	//if (m_keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_DEBUG))
	//{
	//	GetEffectObject(L"Explosive")->PlayEffect(XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 90.0f, 45.0f), 0.5f);
	//}
	return true;
}

void EffectGenerator::finishAction()
{
}

void ExplosiveEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 12;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.05f;
	m_MaxPageCount = 15;
	m_MaxLoopCount = 0;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(1, 1, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"ExplosiveTexture");
		spriteCharacter->AddCameraLabel(L"DefenderCamera");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool ExplosiveEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleY = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationY(localAngleY);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount == m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void ExplosiveEffect::finishAction()
{
}

void LargeExplosiveEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 12;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.05f;
	m_MaxPageCount = 10;
	m_MaxLoopCount = 0;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(1, 1, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"LargeExplosiveTexture");
		spriteCharacter->AddCameraLabel(L"DefenderCamera");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool LargeExplosiveEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleY = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationY(localAngleY);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount == m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void LargeExplosiveEffect::finishAction()
{
}

void CanonMuzzleFlashEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 12;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.05f;
	m_MaxPageCount = 5;
	m_MaxLoopCount = 2;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(1, 1, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"MuzzleFlashTexture");
		spriteCharacter->AddCameraLabel(L"AttackerCamera");
		spriteCharacter->AddCameraLabel(L"DefenderCamera");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool CanonMuzzleFlashEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleY = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationY(localAngleY);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount == m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void CanonMuzzleFlashEffect::finishAction()
{
}

void ScoutEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 1;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.05f;
	m_MaxPageCount = 11;
	m_MaxLoopCount = 3;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(1, 1, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"ScoutEffectTexture");
		spriteCharacter->AddCameraLabel(L"DefenderCamera");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool ScoutEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleY = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationY(localAngleY);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount == m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_SpriteCharacterList.size(); i++)
		{
			m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
			m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void ScoutEffect::finishAction()
{
	
}

void ScoutedEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 1;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.05f;
	m_MaxPageCount = 11;
	m_MaxLoopCount = 0;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(1, 1, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"ScoutedEffectTexture");
		spriteCharacter->AddCameraLabel(L"DefenderCamera");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool ScoutedEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleY = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationY(localAngleY);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount <= m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void ScoutedEffect::finishAction()
{
}

void CircleEffect::initAction()
{
	m_TimeManager = MyAccessHub::GetTimeManager();
	m_EffectSpriteCount = 1;
	m_SpriteRotateInterval = 360.0f / m_EffectSpriteCount;
	m_AnimationInterval = 0.1f;
	m_MaxPageCount = 7;
	m_MaxLoopCount = 0;
	for (int i = 0; i < m_EffectSpriteCount; i++)
	{
		m_CurrentSpriteRotate = m_SpriteRotateInterval * i;

		m_EffectRotation.x = m_CurrentSpriteRotate;

		SpriteCharacter* spriteCharacter = new SpriteCharacter;
		spriteCharacter->setRotation(m_EffectRotation.x, m_EffectRotation.y, m_EffectRotation.z);
		spriteCharacter->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
		spriteCharacter->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
		spriteCharacter->setColor(0, 0, 1, 1);
		spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");
		spriteCharacter->setTextureId(L"CircleTexture");
		XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };
		spriteCharacter->SetSpritePattern(0, 1.0f, 1.0f, uvPattern);
		spriteCharacter->setSpriteIndex(0);

		m_SpriteCharacterList.push_back(spriteCharacter);
	}
}

bool CircleEffect::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

	if (m_IsPlaying)
	{
		if (m_IsFirstTime)
		{
			// 全体の傾き行列
			XMMATRIX mGlobal = XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_EffectRotation.x),
				XMConvertToRadians(m_EffectRotation.y),
				XMConvertToRadians(m_EffectRotation.z)
			);

			for (int i = 0; i < m_SpriteCharacterList.size(); i++)
			{
				// 個別の並び行列
				float localAngleY = m_SpriteRotateInterval * i;
				XMMATRIX mLocal = XMMatrixRotationY(localAngleY);

				// 行列の合成
				XMMATRIX mTotal = mLocal * mGlobal;

				// 行列をMatrixに書き込む
				m_SpriteCharacterList[i]->SetRotationMatrix(mTotal);
				m_SpriteCharacterList[i]->setPosition(m_EffectPosition.x, m_EffectPosition.y, m_EffectPosition.z);
				m_SpriteCharacterList[i]->setScale(m_SpriteScale, m_SpriteScale, m_SpriteScale);
			}
			m_IsFirstTime = false;
		}

		m_AnimationTime += m_TimeManager->GetDeltaTime();
		if (m_AnimationTime > m_AnimationInterval)
		{
			if (m_MaxPageCount == m_PageCount)
			{
				if (m_MaxLoopCount <= m_LoopCount)
				{
					StopEffect();
					return true;
				}
				m_LoopCount++;
				m_PageCount = 0;
			}
			m_PageCount++;
			m_AnimationTime = 0.0f;
		}
		for (int i = 0; i < m_EffectSpriteCount; i++)
		{
			m_PatternRect = {
				kPageAreaList[m_PageCount].x * kFlipWidth / kFullFlipWidth,
				kPageAreaList[m_PageCount].y * kFlipHeight / kFullFlipHeight,
				kFlipWidth / kFullFlipWidth,
				kFlipHeight / kFullFlipHeight
			};
			m_SpriteCharacterList[i]->SetSpritePattern(0, 10.0f, 10.0f, m_PatternRect);

			pipeLine->AddRenderObject(m_SpriteCharacterList[i]);
		}
	}

	return true;
}

void CircleEffect::finishAction()
{
}
