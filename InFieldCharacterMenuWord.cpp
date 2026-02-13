#include "InFieldCharacterMenuWord.h"

#include "MenuText.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <chrono>

#include "SceneManager.h"

using namespace std::chrono;

int InFieldCharacterMenuWord::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str, int makingSpriteCount)
{
	int count = startIndex;

	while (*str != '\0')
	{
		if (std::find(wordList.m_chListJ, m_chEnd, *str) != m_chEnd)
		{
			m_sprites[count]->SetSpritePattern(0, width, height, m_fontMap[*str]);
			m_sprites[count]->setSpriteIndex(0);
			
			if (BRScene->m_InFieldCharacterMenuIndex == makingSpriteCount && BRScene->lighting)
			{
				m_sprites[count]->setColor(0.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				m_sprites[count]->setColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);
			}

			m_sprites[count]->setPosition(ltX, ltY, 0.0f);
			count++;
		}

		ltX += width;

		str++;
	}

	CountList[makingSpriteCount] = count;

	return count;
}

void InFieldCharacterMenuWord::initAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	strs.resize(14);
	m_InFieldMenuTextList.resize(14);
	CountList.resize(14);

	engine->InitCameraConstantBuffer(chData);

	chData->setPosition(0.0f, 0.0f, 1.9f);

	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
	XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));


	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));


	m_spriteCount = 200;

	SpriteCharacter* spc;

	for (int i = 0; i < m_spriteCount; i++)
	{
		spc = new SpriteCharacter();

		spc->setTextureId(L"JPNHUDTexture");
		spc->SetCameraLabel(L"HUDCamera", 0);

		spc->setColor(1, 1, 1, 1);

		spc->SetGraphicsPipeLine(L"Sprite");

		m_sprites.push_back(std::unique_ptr<SpriteCharacter>(spc));
	}

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

bool InFieldCharacterMenuWord::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");

	static steady_clock::time_point lastTime = steady_clock::now();

	steady_clock::time_point currentTime = steady_clock::now();

	duration<float> elapsedTime = currentTime - lastTime;
	float deltaTime = elapsedTime.count();

	lastTime = currentTime;

	int count = 0;

	for (int i = 0; i < BRScene->InFieldAlliesCharacterList.size(); i++)
	{
		if (BRScene->InFieldAlliesCharacterList[i] != nullptr)
		{
			m_InFieldMenuTextList[i] = BRScene->InFieldAlliesCharacterList[i]->CharaName;
			if (BRScene->InFieldAlliesCharacterList[i]->Selected)
			{
				m_InFieldMenuTextList[i] = BRScene->InFieldAlliesCharacterList[i]->CharaName + L"(選択済み)";
			}
			strs[i] = m_InFieldMenuTextList[i].c_str();
		}
		else if (BRScene->InFieldAlliesCharacterList[i] == nullptr)
		{
			m_InFieldMenuTextList[i] = L"ーーーーーーーーーー";
			strs[i] = m_InFieldMenuTextList[i].c_str();
		}
		count = MakeSpriteString(count, xpos[0], ypos[i], 20, 40, strs[i], i);
	}

	if (BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex] != nullptr)
	{
		//兵数
		m_InFieldMenuTextList[10] = L"兵数:" + to_wstring((int)BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex]->CharaSoldiers);
		strs[10] = m_InFieldMenuTextList[10].c_str();
		count = MakeSpriteString(count, xpos[1], ypos[8], 20, 40, strs[10], 10);

		//士気
		m_InFieldMenuTextList[11] = L"士気:" + to_wstring((int)BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex]->CharaMorales);
		strs[11] = m_InFieldMenuTextList[11].c_str();
		count = MakeSpriteString(count, xpos[2], ypos[8], 20, 40, strs[11], 11);

		//攻撃力
		m_InFieldMenuTextList[12] = L"攻撃力:" + to_wstring((int)BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex]->CharaPower);
		strs[12] = m_InFieldMenuTextList[12].c_str();
		count = MakeSpriteString(count, xpos[1], ypos[9], 20, 40, strs[12], 12);

		//防御力
		m_InFieldMenuTextList[13] = L"防御力:" + to_wstring((int)BRScene->InFieldAlliesCharacterList[BRScene->m_InFieldCharacterMenuIndex]->CharaDiffence);
		strs[13] = m_InFieldMenuTextList[13].c_str();
		count = MakeSpriteString(count, xpos[2], ypos[9], 20, 40, strs[13], 13);
	}
	else
	{
		//兵数
		m_InFieldMenuTextList[10] = L"兵数:ーーー";
		strs[10] = m_InFieldMenuTextList[10].c_str();
		count = MakeSpriteString(count, xpos[1], ypos[8], 20, 40, strs[10], 10);

		//士気
		m_InFieldMenuTextList[11] = L"士気:ーーー";
		strs[11] = m_InFieldMenuTextList[11].c_str();
		count = MakeSpriteString(count, xpos[2], ypos[8], 20, 40, strs[11], 11);

		//攻撃力
		m_InFieldMenuTextList[12] = L"攻撃力:ーーー";
		strs[12] = m_InFieldMenuTextList[12].c_str();
		count = MakeSpriteString(count, xpos[1], ypos[9], 20, 40, strs[12], 12);

		//防御力
		m_InFieldMenuTextList[13] = L"防御力:ーーー";
		strs[13] = m_InFieldMenuTextList[13].c_str();
		count = MakeSpriteString(count, xpos[2], ypos[9], 20, 40, strs[13], 13);
	}

	if (BRScene->m_ReadySceneState == ReadySceneState::InField && AnimationFlag)
	{
		AnimCount += deltaTime;
		if (AnimCount < 0.1f)
		{
			for (int i = 0; i < CountList[0]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.1f && AnimCount < 0.2f)
		{
			for (int i = 0; i < CountList[1]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.2f && AnimCount < 0.3f)
		{
			for (int i = 0; i < CountList[2]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.3f && AnimCount < 0.4f)
		{
			for (int i = 0; i < CountList[3]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.4f && AnimCount < 0.5f)
		{
			for (int i = 0; i < CountList[4]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.5f && AnimCount < 0.6f)
		{
			for (int i = 0; i < CountList[5]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.6f && AnimCount < 0.7f)
		{
			for (int i = 0; i < CountList[6]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.7f && AnimCount < 0.8f)
		{
			for (int i = 0; i < CountList[7]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.8f && AnimCount < 0.9f)
		{
			for (int i = 0; i < CountList[8]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 0.9f && AnimCount < 1.0f)
		{
			for (int i = 0; i < CountList[9]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 1.0f && AnimCount < 1.1f)
		{
			for (int i = 0; i < CountList[10]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 1.1f && AnimCount < 1.2f)
		{
			for (int i = 0; i < CountList[11]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 1.2f && AnimCount < 1.3f)
		{
			for (int i = 0; i < CountList[12]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 1.3f && AnimCount < 1.4f)
		{
			for (int i = 0; i < CountList[13]; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		else if (AnimCount > 1.4f)
		{
			for (int i = 0; i < count; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
			AnimCount = 0.0f;
			AnimationFlag = false;
		}
	}
	

	if (BRScene->m_ReadySceneState == ReadySceneState::InField && !AnimationFlag)
	{
		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
	}

    return true;
}

void InFieldCharacterMenuWord::finishAction()
{
}

void InFieldCharacterMenuWord::ResetAnimation()
{
	AnimCount = 0.0f;
	AnimationFlag = true;
	for (int i = 0; i < CountList.size(); i++)
	{
		CountList[i] = 0;
	}
}
