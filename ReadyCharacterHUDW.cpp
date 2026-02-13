#include "ReadyCharacterHUDW.h"

#include "MenuText.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <chrono>

using namespace std::chrono;

#include "SceneManager.h"
#include "BattleFieldManager.h"



int ReadyCharacterHUDW::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str, int makingSpriteCount)
{
	int count = startIndex;

	while (*str != '\0')
	{
		if (std::find(wordList.m_chListJ, m_chEnd, *str) != m_chEnd)
		{
			m_sprites[count]->SetSpritePattern(0, width, height, m_fontMap[*str]);
			m_sprites[count]->setSpriteIndex(0);

			if (BRScene->menuIndex + 3 == makingSpriteCount && BRScene->lighting)
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

void ReadyCharacterHUDW::initAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	m_spriteCount = 200;

	SpriteCharacter* spc;

	for (int i = 0; i < m_spriteCount; i++)
	{
		spc = new SpriteCharacter();

		spc->setTextureId(L"JPNHUDTexture");
		spc->SetCameraLabel(L"HUDCamera", 0);

		spc->setColor(BRScene->textcolor.x, BRScene->textcolor.y, BRScene->textcolor.z, 1);

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

	ally.resize(5);
	enemy.resize(5);
	strs.resize(15);
	CountList.resize(15);
}

bool ReadyCharacterHUDW::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");

	int count = 0;

	
	switch (BRScene->m_ReadySceneState)
	{
	case ReadySceneState::Init:
		if (BRScene->m_InitCount < 1.0f)
		{
			strs[13] = L">パスワード:";
		}
		else if (BRScene->m_InitCount < 1.5f)
		{
			strs[13] = L">パスワード:*";
		}
		else if (BRScene->m_InitCount < 2.0f)
		{
			strs[13] = L">パスワード:**";
		}
		else if (BRScene->m_InitCount < 2.5f)
		{
			strs[13] = L">パスワード:***";
		}
		else if (BRScene->m_InitCount < 3.0f)
		{
			strs[13] = L">パスワード:****";
		}
		else if (BRScene->m_InitCount < 3.5f)
		{
			strs[13] = L">";
		}
		else if (BRScene->m_InitCount < BRScene->kMaxInitCount)
		{
			strs[13] = L">ログイン中";
		}

		count = MakeSpriteString(count, xpos[0], ypos[0], 40, 60, strs[13], 0);

		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
		
		break;
	case ReadySceneState::Menu:
		for (int i = 0; i < 5; i++)
		{
			strs[0] = allies.c_str();
			strs[1] = enemies.c_str();

			if (BRScene->RDAlliesCharacterList[i] != nullptr)
			{
				ally[i] = BRScene->RDAlliesCharacterList[i]->CharaName;
				strs[i + 2] = ally[i].c_str();
			}
			else if (BRScene->RDAlliesCharacterList[i] == nullptr)
			{
				ally[i] = L"ーーーーーーーーーー";
				strs[i + 2] = ally[i].c_str();
			}
			if (BRScene->RDEnemyCharacterList[i] != nullptr)
			{
				enemy[i] = BRScene->RDEnemyCharacterList[i]->CharaName;
				strs[i + 7] = enemy[i].c_str();
			}
			else if (BRScene->RDEnemyCharacterList[i] == nullptr)
			{
				enemy[i] = L"ーーーーーーーーーー";
				strs[i + 7] = enemy[i].c_str();
			}
		}

		strs[12] = L"準備完了";
		strs[13] = L"戦闘に参加する部隊を選択してください";
		strs[14] = L"↑↓:カーソル移動 スペース:決定";

		count = MakeSpriteString(count, xpos[0], ypos[0], 40, 60, strs[13], 0);

		count = MakeSpriteString(count, xpos[0], ypos[1], 20, 40, strs[0], 1);
		count = MakeSpriteString(count, xpos[1], ypos[1], 20, 40, strs[1], 2);

		count = MakeSpriteString(count, xpos[0], ypos[2], 20, 40, strs[2], 3);
		count = MakeSpriteString(count, xpos[0], ypos[3], 20, 40, strs[3], 4);
		count = MakeSpriteString(count, xpos[0], ypos[4], 20, 40, strs[4], 5);
		count = MakeSpriteString(count, xpos[0], ypos[5], 20, 40, strs[5], 6);
		count = MakeSpriteString(count, xpos[0], ypos[6], 20, 40, strs[6], 7);

		count = MakeSpriteString(count, xpos[0], ypos[7], 21, 60, strs[12], 8);

		count = MakeSpriteString(count, xpos[1], ypos[2], 20, 40, strs[7], 9);
		count = MakeSpriteString(count, xpos[1], ypos[3], 20, 40, strs[8], 10);
		count = MakeSpriteString(count, xpos[1], ypos[4], 20, 40, strs[9], 11);
		count = MakeSpriteString(count, xpos[1], ypos[5], 20, 40, strs[10], 12);
		count = MakeSpriteString(count, xpos[1], ypos[6], 20, 40, strs[11], 13);

		count = MakeSpriteString(count, xpos[1], ypos[7], 21, 60, strs[14], 14);

		if (AnimationFlag)
		{
			AnimCount += MyAccessHub::GetTimeManager()->GetDeltaTime();
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
			else if (AnimCount > 1.3f)
			{
				for (int i = 0; i < count; i++)
				{
					pipeLine->AddRenderObject(m_sprites[i].get());
				}
				AnimCount = 0.0f;
				AnimationFlag = false;
			}
		}
		else if (!AnimationFlag)
		{
			for (int i = 0; i < count; i++)
			{
				pipeLine->AddRenderObject(m_sprites[i].get());
			}
		}
		break;
	case ReadySceneState::Loading:
		if (BRScene->m_LoadCount < 1.0f)
		{
			strs[13] = L">司令部のプリンターへ接続中";
		}
		else if (BRScene->m_LoadCount < 2.0f)
		{
			strs[13] = L">接続完了";
		}
		else if (BRScene->m_LoadCount < BRScene->kMaxLoadCount)
		{
			strs[13] = L">データを送信中";
		}

		count = MakeSpriteString(count, xpos[0], ypos[0], 40, 60, strs[13], 0);

		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
		break;
	case ReadySceneState::Finish:
		if (BRScene->m_LoadCount < 0.5f)
		{
			strs[13] = L">データを送信中*";
		}
		else if (BRScene->m_LoadCount < 1.0f)
		{
			strs[13] = L">データを送信中**";
		}
		else if (BRScene->m_LoadCount < BRScene->kMaxLoadCount)
		{
			strs[13] = L">データを送信中***";
		}

		count = MakeSpriteString(count, xpos[0], ypos[0], 40, 60, strs[13], 0);

		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
		break;
	}
	

	return true;
}

void ReadyCharacterHUDW::finishAction()
{
}

void ReadyCharacterHUDW::ResetAnimation()
{
	AnimCount = 0.0f;
	AnimationFlag = true;
	for (int i = 0; i < CountList.size(); i++)
	{
		CountList[i] = 0;
	}
}
