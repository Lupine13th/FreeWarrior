#include "MenuText.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>

#include "SceneManager.h"
#include "BattleFieldManager.h"

using namespace std::chrono;

int MenuText::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
	int count = startIndex;

	while (*str != '\0')
	{
		if (std::find(wordList.m_chListJVT, m_chEnd, *str) != m_chEnd)
		{
			m_sprites[count]->SetSpritePattern(0, width, height, m_fontMap[*str]);
			m_sprites[count]->setSpriteIndex(0);

			m_sprites[count]->setPosition(ltX, ltY, 1.0f);
			count++;
		}

		ltX += width + 8.0f;

		str++;
	}

	return count;
}		//正常


void MenuText::OpenMenuText()
{
	m_IsEnable = false;
	TextCount = 0.0f;
}

void MenuText::initAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	m_TimeManager = MyAccessHub::GetTimeManager();

	engine->InitCameraConstantBuffer(chData);

	chData->setPosition(0.0f, 0.0f, 0.0f);

	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);		
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);		
	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
	XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));


	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));

	m_spriteCount = 50;

	SpriteCharacter* spc;

	for (int i = 0; i < m_spriteCount; i++)
	{
		spc = new SpriteCharacter();

		spc->setTextureId(L"JPNHUDTextureVT");
		spc->SetCameraLabel(L"HUDCamera", 0);
		spc->setColor(1,1,1,1);

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
	float w = 64.0f;	
	float h = 114.0f;	
	while (wordList.m_chListJVT[index] != '\0')
	{
		XMFLOAT4 r = {x * invW, y * invH, w * invW, h * invH};
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

bool MenuText::frameAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");

	int count = 0;

	//攻撃表示
	if (BFMng->GetInLengeEnemyCount() > 0)
	{
		count = MakeSpriteString(count, kTextPositionX, kPositionY[0], 32, 54, kStringList[0]);
	}

	//移動表示
	count = MakeSpriteString(count, kTextPositionX, kPositionY[1], 32, 54, kStringList[1]);

	//アビリティ表示
	if (BFMng->GetInLengeEnemyCount() > 0)
	{
		count = MakeSpriteString(count, kTextPositionX, kPositionY[2], 32, 54, kStringList[2]);
	}

	//待機表示
	count = MakeSpriteString(count, kTextPositionX, kPositionY[3], 32, 54, kStringList[3]);

	//キャンセル表示
	count = MakeSpriteString(count, kTextPositionX, kPositionY[4], 32, 54, kStringList[4]);

	//矢印表示
	float arrowPositionY = 0.0f;

	switch (BFMng->GetMenuSelectIndex())
	{
	default:
		break;
	case 0:
		arrowPositionY = kPositionY[0];
		break;
	case 1:
		arrowPositionY = kPositionY[1];
		break;
	case 2:
		arrowPositionY = kPositionY[2];
		break;
	case 3:
		arrowPositionY = kPositionY[3];
		break;
	case 4:
		arrowPositionY = kPositionY[4];
		break;
	}
	
	count = MakeSpriteString(count, kArrowPositionX, arrowPositionY, 32, 54, kStringList[5]);

	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	if (BFMng->GetMenuUI()->MenuUIenable)
	{
		TextCount += m_TimeManager->GetDeltaTime();

		if (TextCount > 0.6f)
		{
			m_IsEnable = true;
			TextCount = 0.0f;
		}
	}

	if (BFMng->GetMenuUI()->MenuUIenable && m_IsEnable && !BFMng->GetMenuUI()->CloseAnim)
	{
		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
	}

	return true;
}

void MenuText::finishAction()
{
	m_sprites.clear();

	SceneManager* scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	scene->RemoveCamera(this);

}
