#include "DamageHUDW.h"

#include "MenuText.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <chrono>

#include "SceneManager.h"
#include "BattleFieldManager.h"
#include "AbilityType.h"

int DamageHUDW::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
	int count = startIndex;

	while (*str != '\0')
	{
		if (std::find(wordList.m_chListJVT, m_chEnd, *str) != m_chEnd)
		{
			m_sprites[count]->SetSpritePattern(0, width, height, m_fontMap[*str]);
			m_sprites[count]->setSpriteIndex(0);

			m_sprites[count]->setPosition(ltX, ltY, 0.0f);
			count++;
		}

		ltX += width;

		str++;
	}

	return count;
}		//正常

void DamageHUDW::initAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	engine->InitCameraConstantBuffer(chData);

	chData->setPosition(0.0f, 0.0f, 0.0f);

	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
	XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));


	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));


	m_spriteCount = 3;

	SpriteCharacter* spc;

	for (int i = 0; i < m_spriteCount; i++)
	{
		spc = new SpriteCharacter();

		spc->setTextureId(L"JPNHUDTextureVT");
		spc->SetCameraLabel(L"HUDCamera", 0);

		spc->setColor(1, 1, 1, 1);

		//UI用のパイプラインは別にすべきなんだけども、記述量を減らしたいので使いまわし
		spc->SetGraphicsPipeLine(L"Sprite");

		m_sprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
	}

	//FontMap
	Texture2DContainer* tex = engine->GetTextureManager()->GetTexture(L"JPNHUDTextureVT");

	m_fontMap.reserve(wcslen(wordList.m_chListJVT));

	m_chEnd = wordList.m_chListJVT + wcslen(wordList.m_chListJVT);

	int index = 0;
	float invW = 1.0f / tex->fWidth;
	float invH = 1.0f / tex->fHeight;

	float x = 0.0f;
	float y = 0.0f;
	float w = 64.0f;	//フォントサイズ
	float h = 114.0f;	//等幅フォントだと計算簡単
	while (wordList.m_chListJVT[index] != '\0')
	{
		XMFLOAT4 r = { x * invW, y * invH, w * invW, h * invH };
		m_fontMap[wordList.m_chListJVT[index]] = r;
		x += 64.0f;

		if (x >= tex->fWidth)
		{
			x = 0.0f;
			y += h;
		}

		index++;
	}
}

bool DamageHUDW::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");
	auto damageHUD = BFMng->GetDamageHUD();

	std::wstring cstr;

	int count = 0;

	cstr = to_wstring((int)damageHUD->Damage);

	if (damageHUD->GetDamegeAnimationState() == DamegeAnimationState::Play)
	{
		animCount += m_TimeManager->GetDeltaTime();
		
		if (animCount < 0.5f)
		{
			x = x + 1.0f;
			y = y + 0.5f;
		}
		else if (animCount < 1.0f && animCount > 0.5f)
		{
			x = x + 1.0f;
			y = y - 0.5f;
		}
		else if (animCount > 1.0f)
		{
			animCount = 0.0f;
			x = originx;
			y = originy;

			return true;
		}

		const wchar_t* str = cstr.c_str();

		count = MakeSpriteString(count, x, y, 30, 72, str);

		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
	}
	return true;
}

void DamageHUDW::finishAction()
{
	m_sprites.clear();

	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);
}

