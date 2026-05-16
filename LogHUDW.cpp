#include "LogHUDW.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>

#include "SceneManager.h"

int LogHUDW::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
	int count = startIndex;

	while (*str != '\0')
	{
		if (std::find(wordList.m_chListJ, m_chEnd, *str) != m_chEnd)
		{
			m_Sprites[count]->SetSpritePattern(0, width, height, m_fontMap[*str]);
			m_Sprites[count]->setSpriteIndex(0);

			m_Sprites[count]->setPosition(ltX, ltY, 0.0f);
			count++;
		}

		ltX += width;

		str++;
	}

	return count;
}

void LogHUDW::InitAction()
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	CharacterData* chData = GetGameObject()->GetCharacterData();

	m_spriteCount = 300;

	SpriteCharacter* spc;

	for (int i = 0; i < m_spriteCount; i++)
	{
		spc = new SpriteCharacter();
		spc->SetTextureId(L"JPNHUDTexture");
		spc->SetCameraLabel(L"HUDCamera", 0);
		spc->SetGraphicsPipeLine(L"Sprite");

		m_Sprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
	}

	//FontMap
	Texture2DContainer* tex = engine->GetTextureManager()->GetTexture(L"JPNHUDTexture");

	m_fontMap.reserve(wcslen(wordList.m_chListJ));

	m_chEnd = wordList.m_chListJ + wcslen(wordList.m_chListJ);

	int index = 0;
	float invW = 1.0f / tex->fWidth;
	float invH = 1.0f / tex->fHeight;

	float x = 0.0f;
	float y = 0.0f;
	float w = 64.0f;
	float h = 114.0f;
	while (wordList.m_chListJ[index] != '\0')
	{
		XMFLOAT4 r = { x * invW, y * invH, w * invW, h * invH };
		m_fontMap[wordList.m_chListJ[index]] = r;
		x += 64.0f;

		if (x >= tex->fWidth)
		{
			x = 0.0f;
			y += h;
		}

		index++;
	}
}

bool LogHUDW::FrameAction()
{
	MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");

	int count = 0;

	if (BFMng->GetCurrentTurn() != Turn::First && BFMng->GetCurrentTurn() != Turn::Result && BFMng->GetOpenLog())
	{
		for (int i = 0; i < 8; i++)
		{
			strs[i] = ctrs[i].c_str();
		}

		for (int i = 0; i < 8; i++)
		{
			count = MakeSpriteString(count, posx, posy[i], 16, 24, strs[i]);
		}

		for (int j = 0; j < count; j++)
		{
			pipeLine->AddRenderObject(m_Sprites[j].get());
		}
	}

	return true;
}

void LogHUDW::FinishAction()
{
	m_Sprites.clear();

	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);
}
