#include "TurnEndW.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <chrono>

int TurnEndW::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str)
{
	int count = startIndex;

	while (*str != '\0')
	{
		if (std::find(wordList.m_chListJ, m_chEnd, *str) != m_chEnd)
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

void TurnEndW::initAction()
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	CharacterData* chData = getGameObject()->getCharacterData();

	engine->InitCameraConstantBuffer(chData);

	chData->setPosition(0.0f, 0.0f, 0.0f);

	//半分直接D3D触ってるようなもんだから良くはないんだけど、マトリクスを固定で作ってしまう。
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);		//視点（カメラ）座標
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);		//フォーカスする（カメラが向く）座標
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);		//カメラの上方向単位ベクトル（カメラのロール軸）
	XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
	XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));


	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
	engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));


	m_spriteCount = 30;

	cstr.resize(3);

	str.resize(3);

	SpriteCharacter* spc;

	for (int i = 0; i < m_spriteCount; i++)
	{
		spc = new SpriteCharacter();

		spc->setTextureId(L"JPNHUDTexture");
		spc->SetCameraLabel(L"HUDCamera", 0);

		spc->setColor(0, 0, 0, 1);

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

bool TurnEndW::frameAction()
{
	m_TurnEndUI = BFMng->GetTurnEndUI();
	if (BFMng->GetMode() == Mode::TurnEndMode && m_TurnEndUI->GetCurrentCount() > 3.0f)
	{
		MyGameEngine* engine = MyAccessHub::getMyGameEngine();
		GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");

		int count = 0;

		cstr[0] = L"ターンを終了してもいいですか？";
		cstr[1] = L"はい";
		cstr[2] = L"いいえ";

		for (int i = 0; i < cstr.size(); i++)
		{
			str[i] = cstr[i].c_str();
			count = MakeSpriteString(count, posx[i], posy[i], 24, 48, str[i]);
		}

		for (int i = 0; i < count; i++)
		{
			pipeLine->AddRenderObject(m_sprites[i].get());
		}
	}
	return true;
}

void TurnEndW::finishAction()
{
}
