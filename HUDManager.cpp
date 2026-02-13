#include "HUDManager.h"

#include <iostream>
#include <random>

void HUDManager::initAction()
{
    MeterHUD* meterHUD = new MeterHUD();
    getGameObject()->addComponent(meterHUD);
	AddHUDObject("MeterHUD", meterHUD);

	StatusHUD* statusHUDs = new StatusHUD();
    getGameObject()->addComponent(statusHUDs);
    AddHUDObject("StatusHUD", statusHUDs);

    StatusText* statusText = new StatusText();
    getGameObject()->addComponent(statusText);
    AddHUDObject("StatusText", statusText);

    AbilityHUD* abillityHUD = new AbilityHUD();
    getGameObject()->addComponent(abillityHUD);
    AddHUDObject("AbilityHUD", abillityHUD);

    GuideHUD* guideHUD = new GuideHUD();
    getGameObject()->addComponent(guideHUD);
    AddHUDObject("GuideHUD", guideHUD);

    SideMenuHUD* sideMenuHUD = new SideMenuHUD();
    getGameObject()->addComponent(sideMenuHUD);
    AddHUDObject("SideMenuHUD", sideMenuHUD);

    CurrentTerrainHUD* currentTerrainHUD = new CurrentTerrainHUD();
    getGameObject()->addComponent(currentTerrainHUD);
    AddHUDObject("CurrentTerrainHUD", currentTerrainHUD);

    BattleCameraHUD* battleCameraHUD = new BattleCameraHUD();
    getGameObject()->addComponent(battleCameraHUD);
    AddHUDObject("BattleCameraHUD", battleCameraHUD);

    DamageEffectHUD* damageEffectHUD = new DamageEffectHUD();
    getGameObject()->addComponent(damageEffectHUD);
    AddHUDObject("DamageEffectHUD", damageEffectHUD);
}

bool HUDManager::frameAction()
{
	return true;
}

void HUDManager::finishAction()
{
}

void HUDManager::ResetHUDWhenMoveCursor()
{
    m_HUDList["MeterHUD"]->SetAnimationState(AnimationState::Init);
    m_HUDList["StatusHUD"]->SetAnimationState(AnimationState::Init);
}

void MeterHUD::initAction()
{
    SetAnimationState(AnimationState::OnInit);

    m_FontTextureId = L"DiginalHUDTexture";
    m_FontWordList = m_WordList.m_chListDigi;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    m_MaxDelayCount = kBarMovingTime;

    m_SpriteList.resize(4);

    for (int i = 0; i < m_SpriteList.size(); i++)
    {
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kArrowLeftPosX, kSoldierArrowPosY, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(25.0f, 25.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"SoldierBarArrowTexture");
            break;
        case 1:
            m_SpriteList[i]->setPosition(kArrowLeftPosX, kMoraleArrowPosY, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(25.0f, 25.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"SoldierBarArrowTexture");
            break;
        case 2:
            m_SpriteList[i]->setPosition(kMeterPositionX, kSoldierMeterPositionY, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"SoldierTexture");
            break;
        case 3:
            m_SpriteList[i]->setPosition(kMeterPositionX, kMoraleMeterPositionY, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"MoraleTexture");
            break;
        }
    }

    m_TextList["兵数"] = L"";
    m_TextList["士気"] = L"";

    SetAnimationState(AnimationState::Init);
}

bool MeterHUD::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    wstring soldiers;
    wstring morale;

    for (int i = m_ActiveTweenList.size() - 1; i >= 0; --i)
    {
        m_ActiveTweenList[i]->Update(m_TimeManager->GetDeltaTime());
        if (!m_ActiveTweenList[i]->IsActive())
        {
            // Tweenが終了したらリストから削除
            m_ActiveTweenList.erase(m_ActiveTweenList.begin() + i);
        }
    }

    switch (m_AnimationState)
    {
    case AnimationState::Init:
    {
        FieldCharacter* character = nullptr;
        int currentId = -1;

        m_DelayCount = 0.0f;

        if (BFMng->GetCursorState() == CursorState::Select)
        {
            currentId = BFMng->GetSelectID();
        }
        else if (BFMng->GetCursorState() == CursorState::Target)
        {
            currentId = BFMng->GetTargetID();
        }

        if (currentId != -1 && BFMng->GetFieldSquaresList()[currentId]->chara != nullptr)
        {
            character = BFMng->GetFieldSquaresList()[currentId]->chara;
        }

        if (character != nullptr)
        {
            AnimateBarsTo(character);
        }
        break;
    }
    case AnimationState::Run:

        int count = 0;

        int SelectId = 0;

        if (m_MaxDelayCount > m_DelayCount)
        {
			m_DelayCount += m_TimeManager->GetDeltaTime();

            std::random_device rd;                          // ハードウェア乱数の種
            std::mt19937 gen(rd());                         // メルセンヌツイスター(高品質乱数生成器)
            std::uniform_int_distribution<> dist(0, 999);   // 0～999の一様分布

            int value = dist(gen);
            int value2 = dist(gen);

            soldiers = std::to_wstring(value);
            morale = std::to_wstring(value2);
        }
        else if (m_MaxDelayCount < m_DelayCount)
        {
            if (BFMng->GetHUDEnableCondition())
            {
                //兵数を表示するデジタル数字
                switch (BFMng->GetMode())
                {
                default:
                    break;
                case Mode::FieldMode:
                case Mode::MenuMode:
                    SelectId = BFMng->GetSelectID();
                    if (BFMng->GetFieldSquaresList()[SelectId]->chara != nullptr)
                    {
                        if (BFMng->GetFieldSquaresList()[SelectId]->chara->CharaAdmin == Admin::Imperial && !BFMng->GetFieldSquaresList()[SelectId]->chara->Detected)
                        {
                            soldiers = L"ERR";
                            morale = L"ERR";
                            break;
                        }
                        soldiers = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaSoldiers));
                        morale = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaMorales));
                    }
                    break;
                case Mode::AttackMode:
                case Mode::MoveMode:
                case Mode::AbilityMode:
                    SelectId = BFMng->GetTargetID();
                    if (BFMng->GetFieldSquaresList()[SelectId]->chara != nullptr)
                    {
                        if (BFMng->GetFieldSquaresList()[SelectId]->chara->CharaAdmin == Admin::Imperial && !BFMng->GetFieldSquaresList()[SelectId]->chara->Detected)
                        {
                            soldiers = L"ERR";
                            morale = L"ERR";
                            break;
                        }
                        soldiers = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaSoldiers));
                        morale = std::to_wstring(static_cast<int>(BFMng->GetFieldSquaresList()[SelectId]->chara->CharaMorales));
                    }
                    break;
                }
            }
        }

        m_TextList["兵数"] = soldiers.c_str();
        m_TextList["士気"] = morale.c_str();

        count = MakeSpriteStringRightAligned(count, kSoldierTextPos.x, kSoldierTextPos.y, 15, 23, m_TextList["兵数"], XMFLOAT3(1.0f, 1.0f, 1.0f));
        count = MakeSpriteStringRightAligned(count, kMoraleTextPos.x, kMoraleTextPos.y, 15, 23, m_TextList["士気"], XMFLOAT3(1.0f, 1.0f, 1.0f));

        if (BFMng->GetHUDEnableCondition())
        {
            for (int i = 0; i < count; i++)
            {
                pipe->AddRenderObject(m_WordSpriteList[i].get());
            }

            // バーのスプライトを描画リストに追加
            for (int i = 0; i < m_SpriteList.size(); i++)
            {
                pipe->AddRenderObject(m_SpriteList[i].get());
            }
        }
        break;
    }
	return true;
}

void MeterHUD::finishAction()
{
}

void MeterHUD::AnimateBarsTo(FieldCharacter* targetChara)
{
    // 既存のアニメーションを全て停止・削除
    m_ActiveTweenList.clear();

    // --- 兵数バーのアニメーション定義 ---
    SetSoldiersPersent(targetChara);
    SetEasingAnimation(m_SpriteList[0].get(), EasingVector::Horizontal, kArrowLeftPosX, m_SoldierArrowEndPosX, kBarMovingTime, Tween::EaseOutQuad);

    // --- 士気バーのアニメーション定義 ---
    SetMoralePersent(targetChara);
    SetEasingAnimation(m_SpriteList[1].get(), EasingVector::Horizontal, kArrowLeftPosX, m_MoraleArrowEndPosX, kBarMovingTime, Tween::EaseOutQuad);

    SetAnimationState(AnimationState::Run);
}

void MeterHUD::SetSoldiersPersent(FieldCharacter* targetChara)
{
    float soldierPercent = targetChara->CharaSoldiers / targetChara->CharaMaxSoldiers;
    if (!targetChara->Detected && targetChara->CharaAdmin == Admin::Imperial)
    {
        soldierPercent = 1.15f;
    }
	m_SoldierArrowEndPosX = kArrowLeftPosX + kArrowsRenge * soldierPercent;
}

void MeterHUD::SetMoralePersent(FieldCharacter* targetChara)
{
    float moralePercent = targetChara->CharaMorales / targetChara->CharaMaxMorales;
    if (!targetChara->Detected && targetChara->CharaAdmin == Admin::Imperial)
    {
        moralePercent = 1.15f;
    }
    m_MoraleArrowEndPosX = kArrowLeftPosX + kArrowsRenge * moralePercent;
}

void HUDObject::SetEasingAnimation(SpriteCharacter* sprite, EasingVector vector, float startPos, float endPos, float duration, const std::function<float(float, float, float, float)>& easing)
{
	std::function<void(float)> setter = nullptr;
    switch (vector)
    {
    case EasingVector::Horizontal:
        setter = [sprite](float x) {
            XMFLOAT3 currentPos = sprite->getPosition();
            sprite->setPosition(x, currentPos.y, currentPos.z);
            };
        break;
    case EasingVector::Verticle:
        setter = [sprite](float y) {
            XMFLOAT3 currentPos = sprite->getPosition();
            sprite->setPosition(currentPos.x, y, currentPos.z);
            };
        break;
    }

    m_ActiveTweenList.push_back(std::make_unique<Tween>(
        startPos,               // 開始座標
        endPos,                 // 終了座標
        duration,               // 時間
        setter,                 // 更新処理
        easing                  // 緩急の種類
    ));
}

void HUDObject::FlipAnimation(SpriteCharacter* sprite)
{
    int activePage = 0;

    if (m_FlipDuration * m_AnimationPages < m_FlipAnimationCount)
    {
        SetAnimationState(AnimationState::Finish);
        return;
    }

    for (int i = 0; i < m_AnimationPages; i++)
    {
        if (m_FlipDuration * (i + 1) > m_FlipAnimationCount)
        {
            activePage = i;
			break;
        }
    }

    m_PatternRect ={
        kPageAreaList[activePage].x* kFlipWidth / kFullFlipWidth, 
        kPageAreaList[activePage].y* kFlipHeight / kFullFlipHeight,
        kFlipWidth / kFullFlipWidth, 
        kFlipHeight / kFullFlipHeight
    };

    sprite->SetSpritePattern(0, 1, 1, m_PatternRect);

	m_FlipAnimationCount += m_TimeManager->GetDeltaTime();
}

void HUDObject::SetShaderResoruce()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    CharacterData* chData = getGameObject()->getCharacterData();

    engine->InitCameraConstantBuffer(chData);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixTranspose(MakeViewMatix(Eye, At, Up));
    XMMATRIX proj = XMMatrixTranspose(MakeOrthographicPrjectionMatrix(engine->GetWidth(), engine->GetHeight(), 0.01f, 3.0f));

    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(0), &view, sizeof(XMMATRIX));
    engine->UpdateShaderResourceOnGPU(chData->GetConstantBuffer(1), &proj, sizeof(XMMATRIX));
}

void HUDObject::ResetHUD()
{
	SetAnimationState(AnimationState::Init);
}

void StatusHUD::initAction()
{
    SetAnimationState(AnimationState::OnInit);

    m_AnimationPages = 10;
    m_FlipDuration = 0.05f;

    m_SpriteList.resize(3);

    for (int i = 0; i < m_SpriteList.size(); i++)
    {
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:     //ブラウン管のノイズ
            m_SpriteList[i]->setPosition(380.0f, 200.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(160.0f, 120.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"BrownTelevisionNoizeTexture");
            break;
        case 2:     //テレビラベル
            m_SpriteList[i]->setPosition(380.0f, 155.0f, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 40.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"Sprite00");
            break;
        case 1:     //ブラウン管テレビフレーム
            m_SpriteList[i]->setPosition(380.0f, 200.0f, OrderInLayer::Text + 0.2f);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"BrownTelevisiomnFrameImage");
            break;
        }
    }
    SetAnimationState(AnimationState::Init);
}

bool StatusHUD::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");
    SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());

    Squares* selectSquare = BFMng->GetFieldSquaresList()[BFMng->GetSelectID()];
    Squares* targetSquare = BFMng->GetFieldSquaresList()[BFMng->GetTargetID()];

    int index = 0;

    switch (m_AnimationState)
    {
    case AnimationState::Init:
		m_FlipAnimationCount = 0.0f;
        p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
        index = 2;
		SetAnimationState(AnimationState::Run);
        break;
    case AnimationState::Run:
        FlipAnimation(m_SpriteList[0].get());
        index = 2;
        break;
    case AnimationState::Finish:

        index = 3;

        switch (BFMng->GetCursorState())
        {
            case CursorState::Select:
                if (selectSquare->chara == nullptr)
					return true;

                if (!selectSquare->chara->Detected && selectSquare->chara->CharaAdmin == Admin::Imperial)
                {
                    SetAnimationState(AnimationState::Init);
                    index = 2;
                }
				break;
            case CursorState::Target:
                if (targetSquare->chara == nullptr)
                    return true;

                if (!targetSquare->chara->Detected && selectSquare->chara->CharaAdmin == Admin::Imperial)
                {
                    SetAnimationState(AnimationState::Init);
                    index = 2;
                }
				break;
        }

        if (BFMng->GetHUDEnableCondition())
        {
            p_scene->SetActiveCameraCompornent(L"ScoutingCamera", true);
        }
        else
        {
            p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
        }
        break;
    }

    if (BFMng->GetHUDEnableCondition() && BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara != nullptr && BFMng->GetCursorState() == CursorState::Select)
    {
        SetLabelColor();
        SetRender(index);
    }
    else if (BFMng->GetHUDEnableCondition() && BFMng->GetFieldSquaresList()[BFMng->GetTargetID()]->chara != nullptr && BFMng->GetCursorState() == CursorState::Target)
    {
        SetLabelColor();
        SetRender(index);
    }

    return true;
}

void StatusHUD::finishAction()
{
}

void StatusHUD::SetLabelColor()
{
    switch (BFMng->GetCursorState())
    {
    case CursorState::Select:
        if (BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara == nullptr)
        {
            return;
		}
        switch (BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara->CharaAdmin)
        {
        case Admin::Rebel:
            m_SpriteList[2]->setColor(0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case Admin::Imperial:
            m_SpriteList[2]->setColor(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        }
        break;
    case CursorState::Target:
        if (BFMng->GetFieldSquaresList()[BFMng->GetTargetID()]->chara == nullptr)
        {
            return;
		}
        switch (BFMng->GetFieldSquaresList()[BFMng->GetTargetID()]->chara->CharaAdmin)
        {
        case Admin::Rebel:
            m_SpriteList[2]->setColor(0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case Admin::Imperial:
            m_SpriteList[2]->setColor(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        }
        break;
    }
    
}

void StatusHUD::SetRender(int index)
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    switch (index)
    {
    case 2:
        pipe->AddRenderObject(m_SpriteList[0].get());
        pipe->AddRenderObject(m_SpriteList[1].get());
		break;
    case 3:
        pipe->AddRenderObject(m_SpriteList[1].get());
        pipe->AddRenderObject(m_SpriteList[2].get());
		break;
    }
}

void StatusText::initAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);
}

bool StatusText::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    Squares* selectSquare = BFMng->GetFieldSquaresList()[BFMng->GetSelectID()];
    Squares* targetSquare = BFMng->GetFieldSquaresList()[BFMng->GetTargetID()];

    if (BFMng->GetHUDEnableCondition() && MyAccessHub::GetHUDManager()->GetHUDObject("StatusHUD")->GetAnimationState() == AnimationState::Finish && selectSquare->chara != nullptr || targetSquare->chara != nullptr)
    {
        //ステータス表示:名前
        std::wstring cstr;

        int count = 0;

        if (BFMng->GetCursorState() == CursorState::Select)
        {
            if (BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara == nullptr)
            {
				return true;
            }

            if (selectSquare->SqAdmin == Admin::Rebel)
            {
                cstr = BFMng->GetAlliesCharacterList()[selectSquare->ThereCharaID]->CharaName;
            }
            else if (selectSquare->SqAdmin == Admin::Imperial)
            {
                if (BFMng->GetEnemyCharacterList()[selectSquare->ThereCharaID]->Detected)
                {
                    cstr = BFMng->GetEnemyCharacterList()[selectSquare->ThereCharaID]->CharaName;
                }
                else
                {
                    cstr = L"信号無し";
                }
            }
        }
        else if (BFMng->GetCursorState() == CursorState::Target)
        {
            if (targetSquare->chara == nullptr)
            {
                return true;
            }

            if (targetSquare->SqAdmin == Admin::Rebel)
            {
                cstr = BFMng->GetAlliesCharacterList()[selectSquare->ThereCharaID]->CharaName;
            }
            else if (targetSquare->SqAdmin == Admin::Imperial)
            {
                if (targetSquare->chara->Detected)
                {
                    cstr = BFMng->GetEnemyCharacterList()[selectSquare->ThereCharaID]->CharaName;
                }
                else
                {
                    cstr = L"信号無し";
                }
            }
        }
        
        //ステータス表示:名前End

        if (selectSquare->SqAdmin == Admin::Rebel)
        {
            count = MakeSpriteString(count, kCharacterNameTextPos.x, kCharacterNameTextPos.y, 15, 30, cstr.c_str(), XMFLOAT3(1.0f, 1.0f, 1.0f));
        }
        else if (selectSquare->SqAdmin == Admin::Imperial)
        {
            count = MakeSpriteString(count, kCharacterNameTextPos.x, kCharacterNameTextPos.y, 15, 30, cstr.c_str(), XMFLOAT3(1.0f, 1.0f, 1.0f));
        }

        for (int i = 0; i < count; i++)
        {
            if (BFMng->GetHUDEnableCondition() && MyAccessHub::GetHUDManager()->GetHUDObject("StatusHUD")->GetAnimationState() == AnimationState::Finish)    
            {
                pipeLine->AddRenderObject(m_WordSpriteList[i].get());
            }
        }
    }
    return true;
}

void StatusText::finishAction()
{
}

int HUDTextObject::MakeSpriteString(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str, XMFLOAT3 color)
{
    int count = startIndex;

    while (*str != '\0')
    {
        if (std::find(m_FontWordList, kWordListEnd, *str) != kWordListEnd)
        {
            m_WordSpriteList[count]->SetSpritePattern(0, width, height, m_FontMap[*str]);
            m_WordSpriteList[count]->setSpriteIndex(0);
            m_WordSpriteList[count]->setColor(color.x, color.y, color.z, 1);

            m_WordSpriteList[count]->setPosition(ltX, ltY, 0.0f);
            count++;
        }

        ltX += width;

        str++;
    }

    return count;
}

int HUDTextObject::MakeSpriteStringRightAligned(int startIndex, float ltX, float ltY, float width, float height, const wchar_t* str, XMFLOAT3 color)
{
    int count = startIndex;

    int len = 0;
    const wchar_t* tmp = str;
    while (*tmp != L'\0')
    {
        if (std::find(m_FontWordList, kWordListEnd, *tmp) != kWordListEnd)
            len++;
        tmp++;
    }

    float totalWidth = len * (width + 2.0f) - 2.0f;

    float startX = ltX - totalWidth;

    while (*str != L'\0')
    {
        if (std::find(m_FontWordList, kWordListEnd, *str) != kWordListEnd)
        {
            m_WordSpriteList[count]->SetSpritePattern(0, width, height, m_FontMap[*str]);
            m_WordSpriteList[count]->setSpriteIndex(0);
            m_WordSpriteList[count]->setPosition(startX, ltY, 0.0f);
            m_WordSpriteList[count]->setColor(color.x, color.y, color.z, 1);
            count++;
            startX += width + 2.0f;
        }

        str++;
    }

    return count;
}

void HUDTextObject::SetFont(const wchar_t* fontTextureId, const wchar_t* fontWordList)
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();

    SpriteCharacter* spc;

    for (int i = 0; i < m_SpriteCount; i++)
    {
        spc = new SpriteCharacter();

        spc->setTextureId(fontTextureId);
        spc->SetCameraLabel(L"HUDCamera", 0);

        spc->SetGraphicsPipeLine(L"AlphaSprite");

        m_WordSpriteList.push_back(std::unique_ptr<SpriteCharacter>(spc));
    }

    //FontMap
    Texture2DContainer* tex = engine->GetTextureManager()->GetTexture(fontTextureId);

    m_FontMap.reserve(wcslen(fontWordList));

    kWordListEnd = fontWordList + wcslen(fontWordList);

    int index = 0;
    float invW = 1.0f / tex->fWidth;
    float invH = 1.0f / tex->fHeight;

    float x = 0.0f;
    float y = 0.0f;
    float w = 64.0f;	//フォントサイズ
    float h = 114.0f;	//等幅フォントだと計算簡単
    while (fontWordList[index] != '\0')
    {
        XMFLOAT4 r = { x * invW, y * invH, w * invW, h * invH };
        m_FontMap[fontWordList[index]] = r;
        x += 64.0f;

        if (x >= tex->fWidth)
        {
            x = 0.0f;
            y += h;
        }

        index++;
    }
}

void AbilityHUD::initAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 4; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(500.0f, 500.0f, 0.1f);
            m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->setTextureId(L"AbillityBackGroundTexture");
            break;
        case 1:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 400.0f, 0.1f);
            m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->setTextureId(L"AbillityNoteTexture");
            break;
        case 2:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 400.0f, 0.1f);
            m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->setTextureId(L"AbillityNoteTexture");
            break;
        case 3:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 400.0f, 0.1f);
            m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->setTextureId(L"AbillityNoteTexture");
            break;
        }
    }

    m_TextList["スキル1"] = L"";
    m_TextList["スキル1説明"] = L"";
    m_TextList["スキル2"] = L"";
    m_TextList["スキル2説明"] = L"";
    m_TextList["スキル3"] = L"";
    m_TextList["スキル3説明"] = L"";

    SetAnimationState(AnimationState::Init);
}

bool AbilityHUD::frameAction()
{
    if (BFMng->GetHUDEnableCondition() && BFMng->GetMode() == Mode::AbilityMode && BFMng->GetTargetMode() == TargetMode::None)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        for (int i = m_ActiveTweenList.size() - 1; i >= 0; --i)
        {
            m_ActiveTweenList[i]->Update(m_TimeManager->GetDeltaTime());
            if (!m_ActiveTweenList[i]->IsActive())
            {
                // Tweenが終了したらリストから削除
                m_ActiveTweenList.erase(m_ActiveTweenList.begin() + i);
            }
        }

        int count = 0;
        int abillityCount = 0;

        for (int i = 0; i < 3; i++)
        {
            if (BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->ThereCharaID] != nullptr)

                if (BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->ThereCharaID]->Abilities[i] != AbilityType::None)
                {
                    switch (BFMng->GetAlliesCharacterList()[BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->ThereCharaID]->Abilities[i])
                    {
                    case AbilityType::ConcentratedFire:
                        m_SpriteList[i + 1]->setTextureId(L"AbillityConcentratedFireTexture");
                        break;
                    case AbilityType::BayonetCharge:
                        m_SpriteList[i + 1]->setTextureId(L"AbillityBayonetChargeTexture");
                        break;
                    case AbilityType::Scout:
                        m_SpriteList[i + 1]->setTextureId(L"AbillityScoutTexture");
                        break;
                    }
                    abillityCount++;
                }
        }

        switch (m_AnimationState)
        {
        case AnimationState::Init:
            if (abillityCount > 0)
            {
                m_AnimationCount += m_TimeManager->GetDeltaTime();

                switch (m_AbilityHUDState)
                {
                case AbilityHUDState::FolderUp:
                    m_ActiveTweenList.clear();
                    m_AnimationCount = 0.0f;
                    SetEasingAnimation(m_SpriteList[0].get(), EasingVector::Verticle, -600.0f, kAbillityBackGroundPosition.y, kBackGroundAnimationCount, Tween::EaseOutQuad);
                    m_AbilityHUDState = AbilityHUDState::FileUp;
                    break;
                case AbilityHUDState::FileUp:
                    if (m_AnimationCount > kBackGroundAnimationCount)
                    {
                        for (int i = 1; i < 4; i++)
                        {
                            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::BackGround + 0.1f);
                            SetEasingAnimation(m_SpriteList[i].get(), EasingVector::Horizontal, 0.0f, kMaxAbillityNotePositionX, kBackGroundAnimationCount, Tween::EaseOutQuad);
                        }

                        m_AbilityHUDState = AbilityHUDState::FileDown;
                    }
                    break;
                case AbilityHUDState::FileDown:
                    if (m_AnimationCount > kBackGroundAnimationCount * 2)
                    {
                        for (int i = 1; i < 4; i++)
                        {
                            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
                            SetEasingAnimation(m_SpriteList[i].get(), EasingVector::Horizontal, kMaxAbillityNotePositionX, kAbillityBackGroundPosition.y, kBackGroundAnimationCount, Tween::EaseOutQuad);
                        }

                        m_AbilityHUDState = AbilityHUDState::FileOpen;
                    }
                    break;
                case AbilityHUDState::FileOpen:
                    if (m_AnimationCount < kBackGroundAnimationCount * 3)
                    {
                        m_AbillityNoteScale += 0.5f;
                        for (int i = 1; i < 4; i++)
                        {
                            m_SpriteList[i]->setScale(m_AbillityNoteScale, m_AbillityNoteScale, 0.1f);
                        }
                    }
                    if (m_AnimationCount > kBackGroundAnimationCount * 3)
                    {
                        switch (abillityCount)
                        {
                        case 1:
                            m_SpriteList[1]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
                            break;
                        case 2:
                            SetEasingAnimation(m_SpriteList[1].get(), EasingVector::Horizontal, kAbillityBackGroundPosition.x, kDoubleAbillityNotePositionX.x, kBackGroundAnimationCount, Tween::EaseInQuad);
                            SetEasingAnimation(m_SpriteList[2].get(), EasingVector::Horizontal, kAbillityBackGroundPosition.x, kDoubleAbillityNotePositionX.y, kBackGroundAnimationCount, Tween::EaseInQuad);
                            break;
                        case 3:
                            SetEasingAnimation(m_SpriteList[1].get(), EasingVector::Horizontal, kAbillityBackGroundPosition.x, kAbillityNotePositionX.x, kBackGroundAnimationCount, Tween::EaseInQuad);
                            SetEasingAnimation(m_SpriteList[2].get(), EasingVector::Horizontal, kAbillityBackGroundPosition.x, kAbillityNotePositionX.y, kBackGroundAnimationCount, Tween::EaseInQuad);
                            SetEasingAnimation(m_SpriteList[3].get(), EasingVector::Horizontal, kAbillityBackGroundPosition.x, kAbillityNotePositionX.z, kBackGroundAnimationCount, Tween::EaseInQuad);
                            break;
                        }
                        m_AbilityHUDState = AbilityHUDState::Stay;
                    }
                    break;
                case AbilityHUDState::Stay:
                    m_AnimationState = AnimationState::Run;
                    break;
                }

            }
            break;

        case AnimationState::Run:
            for (int i = 1; i < 4; i++)
            {
                m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
            }

            switch (BFMng->GetAbillityIndex())
            {
            case 0:
                m_SpriteList[1]->setColor(1.0f, 0.5f, 0.5f, 1);
                break;
            case 1:
                m_SpriteList[2]->setColor(1.0f, 0.5f, 0.5f, 1);
                break;
            case 2:
                m_SpriteList[3]->setColor(1.0f, 0.5f, 0.5f, 1);
                break;
            }
            break;
        }

        pipeline->AddRenderObject(m_SpriteList[0].get());

        if (m_AnimationCount > kBackGroundAnimationCount)
        {
            switch (abillityCount)
            {
            case 1:
                for (int i = 1; i < 2; i++)
                {
                    pipeline->AddRenderObject(m_SpriteList[i].get());
                }
                break;
            case 2:
                for (int i = 1; i < 3; i++)
                {
                    pipeline->AddRenderObject(m_SpriteList[i].get());
                }
                break;
            case 3:
                for (int i = 1; i < 4; i++)
                {
                    pipeline->AddRenderObject(m_SpriteList[i].get());
                }
                break;
            }
        }

    }
    return true;
}

void AbilityHUD::finishAction()
{
}

void AbilityHUD::ResetHUD()
{
    SetAnimationState(AnimationState::Init);
    m_AbilityHUDState = AbilityHUDState::FolderUp;
	m_AbillityNoteScale = kOriginAbillityNoteScale;
}

void GuideHUD::initAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 1; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kGuideBackGroundPos.x, kGuideBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(700.0f, 700.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"WindowTexture");
            break;
        }
    }
	
	m_TextList["フィールド"] = (L"方向キー:選択_スペースキー::決定_WASDキー:カメラ移動");
	m_TextList["フィールド2"] = (L"E/Qキー:カメラズームイン/アウト");
	m_TextList["メニュー"] = (L"上下キー:カーソル移動_スペースキー:決定_エスケープキー:閉じる");
	m_TextList["アビリティ"] = (L"左右キー:選択_スペースキー::決定_エスケープキー:閉じる");
	m_TextList["敵へアビリティ"] = (L"方向キー:選択_スペースキー::実行_エスケープキー:閉じる");
	m_TextList["攻撃"] = (L"方向キー:選択_スペースキー::攻撃_エスケープキー:閉じる");
	m_TextList["移動"] = (L"方向キー:選択_スペースキー::移動_エスケープキー:閉じる");
}

bool GuideHUD::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipeLine = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    int count = 0;

    switch (BFMng->GetMode())
    {
        default:
			break;
        case Mode::FieldMode:
            count = MakeSpriteString(count, kGuideTextPos.x, kGuideTextPos.y, 20, 30, m_TextList["フィールド"], XMFLOAT3(0.0f, 0.0f, 0.0f));
            count = MakeSpriteString(count, kGuideTextPos2.x, kGuideTextPos2.y, 20, 30, m_TextList["フィールド2"], XMFLOAT3(0.0f, 0.0f, 0.0f));
            break;
        case Mode::MenuMode:
            count = MakeSpriteString(count, kGuideTextPos.x, kGuideTextPos.y, 20, 30, m_TextList["メニュー"], XMFLOAT3(0.0f, 0.0f, 0.0f));
			break;
        case Mode::AbilityMode:
            switch (BFMng->GetAbillityMenuState())
            {
                case AbillityMenuState::Menu:
                    count = MakeSpriteString(count, kGuideTextPos.x, kGuideTextPos.y, 20, 30, m_TextList["アビリティ"], XMFLOAT3(0.0f, 0.0f, 0.0f));
					break;
                case AbillityMenuState::Target:
                    count = MakeSpriteString(count, kGuideTextPos.x, kGuideTextPos.y, 20, 30, m_TextList["攻撃"], XMFLOAT3(0.0f, 0.0f, 0.0f));
					break;
            }
			break;
        case Mode::AttackMode:
            count = MakeSpriteString(count, kGuideTextPos.x, kGuideTextPos.y, 20, 30, m_TextList["攻撃"], XMFLOAT3(0.0f, 0.0f, 0.0f));
			break;
        case Mode::MoveMode:
            count = MakeSpriteString(count, kGuideTextPos.x, kGuideTextPos.y, 20, 30, m_TextList["移動"], XMFLOAT3(0.0f, 0.0f, 0.0f));
			break;
    }

    if (BFMng->GetHUDEnableCondition())
    {
        for (int i = 0; i < count; i++)
        {
            pipeLine->AddRenderObject(m_WordSpriteList[i].get());
        }

        pipeLine->AddRenderObject(m_SpriteList[0].get());
    }

    return true;
}

void GuideHUD::finishAction()
{
}

void SideMenuHUD::initAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 4; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
        case 1:
        case 2:
            m_SpriteList[i]->setPosition(kSideMenuBackGroundPos.x, kTextPositionY[i], OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 50.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"Sprite00");
            break;
        case 3:
            m_SpriteList[i]->setPosition(kSideMenuBackGroundPos.x, kSideMenuBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(600.0f, 600.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"SideMenuTexture");
            break;
        }
    }

    m_TextList["ターン終了"] = (L"ターン終了");
    m_TextList["ゲーム終了"] = (L"ゲーム終了");
    m_TextList["キャンセル"] = (L"キャンセル");
}

bool SideMenuHUD::frameAction()
{
    if (BFMng->GetMode() == Mode::SideMenuMode)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

        int count = 0;

        switch (BFMng->GetSideMenuSelectIndex())
        {
        default:
            break;
        case 0:
            pipeline->AddRenderObject(m_SpriteList[0].get());
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[0], 35, 50, m_TextList["ターン終了"], XMFLOAT3(0.0f, 0.0f, 0.0f));
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[1], 35, 50, m_TextList["ゲーム終了"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[2], 35, 50, m_TextList["キャンセル"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            break;
        case 1:
            pipeline->AddRenderObject(m_SpriteList[1].get());
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[0], 35, 50, m_TextList["ターン終了"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[1], 35, 50, m_TextList["ゲーム終了"], XMFLOAT3(0.0f, 0.0f, 0.0f));
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[2], 35, 50, m_TextList["キャンセル"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            break;
        case 2:
            pipeline->AddRenderObject(m_SpriteList[2].get());
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[0], 35, 50, m_TextList["ターン終了"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[1], 35, 50, m_TextList["ゲーム終了"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            count = MakeSpriteString(count, kTextPositionX, kTextPositionY[2], 35, 50, m_TextList["キャンセル"], XMFLOAT3(0.0f, 0.0f, 0.0f));
            break;
        }

        pipeline->AddRenderObject(m_SpriteList[3].get());

        for (int i = 0; i < count; i++)
        {
            pipeline->AddRenderObject(m_WordSpriteList[i].get());
        }

    }
    
    return true;
}

void SideMenuHUD::finishAction()
{
}

void CurrentTerrainHUD::initAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    m_MaxDelayCount = 1.0f;

    for (int i = 0; i < 1; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kBackGroundPos.x, kBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"TerrainHUDTexture");
            break;
        }
    }

    m_TextList["地形名称"] = (L"");
    m_TextList["地形効果"] = (L"地形効果:");
    m_TextList["地形効果詳細"] = (L"");
    m_TextList["座標"] = (L"");

    SetAnimationState(AnimationState::Init);
}

bool CurrentTerrainHUD::frameAction()
{
    if (!BFMng->GetHUDEnableCondition() || m_AnimationState == AnimationState::Init)
    {
		m_DelayCount += m_TimeManager->GetDeltaTime();
        if (m_DelayCount > m_MaxDelayCount)
        {
            SetAnimationState(AnimationState::Run);
            m_DelayCount = 0.0f;
        }
        return true;
	}

    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    Squares* selectSquare = nullptr;

    int count = 0;

    switch (BFMng->GetCursorState())
    {
    case CursorState::Target:
        selectSquare = BFMng->GetFieldSquaresList()[BFMng->GetTargetID()];
        break;
    case CursorState::Select:
        selectSquare = BFMng->GetFieldSquaresList()[BFMng->GetSelectID()];
        break;
    }

    switch (selectSquare->terrainname)
    {
    case Terrain::Plane:
        m_TextList["地形名称"] = L"平地";
        m_TextList["地形効果詳細"] = L"なし";
        break;
    case Terrain::Forest:
        m_TextList["地形名称"] = L"森林";
        m_TextList["地形効果詳細"] = L"与ダメージ+20%";
        break;
    case Terrain::River:
        m_TextList["地形名称"] = L"河川";
        m_TextList["地形効果詳細"] = L"被ダメージ-25%";
        break;
    case Terrain::Hills:
        m_TextList["地形名称"] = L"丘陵";
        m_TextList["地形効果詳細"] = L"被ダメージ-25%";
        break;
    case Terrain::Tower:
        m_TextList["地形名称"] = L"監視塔";
        m_TextList["地形効果詳細"] = L"与ダメージ+20%";
        break;
    case Terrain::Supply:
        m_TextList["地形名称"] = L"補給所";
        m_TextList["地形効果詳細"] = L"なし";
        break;
    default:
        m_TextList["地形名称"] = L"";
        m_TextList["地形効果詳細"] = L"";
        break;
    }

    wstring squarePosition = L"X:" + std::to_wstring(selectSquare->charaPosX) + L" Y:" + std::to_wstring(selectSquare->charaPosY);

    m_TextList["座標"] = squarePosition.c_str();

    count = MakeSpriteString(count, kTextPositionX, kTextPositionY[0], 30, 45, m_TextList["地形名称"], XMFLOAT3(1.0f, 1.0f, 1.0f));
    count = MakeSpriteString(count, kTextPositionX, kTextPositionY[1], 20, 30, m_TextList["地形効果"], XMFLOAT3(1.0f, 1.0f, 1.0f));
    count = MakeSpriteString(count, kTextPositionX, kTextPositionY[2], 18, 27, m_TextList["地形効果詳細"], XMFLOAT3(1.0f, 1.0f, 1.0f));
    count = MakeSpriteString(count, kTextPositionX, kTextPositionY[3], 20, 30, m_TextList["座標"], XMFLOAT3(1.0f, 1.0f, 1.0f));

    pipeline->AddRenderObject(m_SpriteList[0].get());

    for (int i = 0; i < count; i++)
    {
        pipeline->AddRenderObject(m_WordSpriteList[i].get());
    }

    return true;
}

void CurrentTerrainHUD::finishAction()
{
}

void BattleCameraHUD::initAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 1; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kBackGroundPos.x, kBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(960.0f, 950.0f, 0.1f);
            m_SpriteList[i]->setTextureId(L"BattleCameraHUDTexture");
            break;
        }
    }

    m_TextList["攻撃側"] = (L"");
    m_TextList["防御側"] = (L"");

    SetAnimationState(AnimationState::Init);
}

bool BattleCameraHUD::frameAction()
{
    if (BFMng->GetBattleCameraEnable() == true)
    {
        MyGameEngine* engine = MyAccessHub::getMyGameEngine();
        GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

		int count = 0;

        Squares* attackerCharacterSquare = BFMng->GetAttackingCharacterSquares()->GetNowChara();
        Squares* defenderCharacterSquare = BFMng->GetAttackingCharacterSquares()->GetNextChara();

        if (attackerCharacterSquare != nullptr && defenderCharacterSquare != nullptr)
        {
            m_TextList["攻撃側"] = (attackerCharacterSquare->chara->CharaName.c_str());
            m_TextList["防御側"] = (defenderCharacterSquare->chara->CharaName.c_str());

            count = MakeSpriteString(count, kAttackerTextPos.x, kAttackerTextPos.y, 30, 45, m_TextList["攻撃側"], XMFLOAT3(1.0f, 1.0f, 1.0f));
            count = MakeSpriteString(count, kDefenderTextPos.x, kDefenderTextPos.y, 30, 45, m_TextList["防御側"], XMFLOAT3(1.0f, 1.0f, 1.0f));
        }

		pipeline->AddRenderObject(m_SpriteList[0].get());    //外枠を描画
        for (int i = 0; i < count; i++)
        {
            pipeline->AddRenderObject(m_WordSpriteList[i].get());
        }
    }
    return true;
}

void BattleCameraHUD::finishAction()
{
}

void MainMenuHUD::initAction()
{
    SetShaderResoruce();

    m_FontTextureId = L"JPNHUDTextureVT";
    m_FontWordList = m_WordList.m_chListJVT;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 25; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        
        if (i < 18)
        {
            m_SpriteList[i]->setTextureId(L"NixieTubesTexture");
            m_SpriteList[i]->setScale(110.0f, 90.0f, 0.1f);
        }
        else if (i < 23)
        {
            m_SpriteList[i]->setTextureId(L"NixieBaseTexture");
            m_SpriteList[i]->setScale(250.0f, 250.0f, 1.0f);
        }
        else if (i < 24)
        {
            m_SpriteList[i]->setTextureId(L"MenuBaseTexture");
            m_SpriteList[i]->setScale(200.0f, 330.0f, 1.0f);
        }
        else if (i < 25)
        {
            m_SpriteList[i]->setTextureId(L"NixieGageTexture");
            m_SpriteList[i]->setScale(300.0f, 300.0f, 1.0f);
        }

        switch (i)
        {
        case 0:
            /*m_SpriteList[i]->setPosition(kBackGroundPos.x, kBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(960.0f, 950.0f, 0.1f);*/
            
            break;
        }
    }

    m_TextList["攻撃側"] = (L"");
    m_TextList["防御側"] = (L"");

    SetAnimationState(AnimationState::Init);
}

bool MainMenuHUD::frameAction()
{
    return true;
}

void MainMenuHUD::finishAction()
{
}

void DamageEffectHUD::initAction()
{
    for (int i = 0; i < 3; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->setColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kLowDamageEffectPosition.x, kLowDamageEffectPosition.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(kSmallDamageEffectScale, kSmallDamageEffectScale, 0.1f);
            m_SpriteList[i]->setTextureId(L"DamagedEffectTexture");
            break;
        case 1:
            m_SpriteList[i]->setPosition(kMidDamageEffectPosition.x, kMidDamageEffectPosition.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(kLargeDamageEffectScale, kLargeDamageEffectScale, 0.1f);
            m_SpriteList[i]->setTextureId(L"DamagedEffectTexture");
            break;
        case 2:
            m_SpriteList[i]->setPosition(kHighDamageEffectPosition.x, kHighDamageEffectPosition.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(kSmallDamageEffectScale, kSmallDamageEffectScale, 0.1f);
            m_SpriteList[i]->setTextureId(L"DamagedEffectTexture");
            break;
        }
    }
}

bool DamageEffectHUD::frameAction()
{
    MyGameEngine* engine = MyAccessHub::getMyGameEngine();
    GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    switch (m_AnimationState)
    {
    case AnimationState::Init:
        SetDamageEffect(BFMng->GetAttackedCharacter()->CharaSoldiers / BFMng->GetAttackedCharacter()->CharaMaxSoldiers);
        break;
    case AnimationState::Run:
		m_DelayCount += m_TimeManager->GetDeltaTime();
        if (m_DelayCount > 1.0f && m_DelayCount < 3.0f)
        {
            switch (m_EffectCount)
            {
                case 1:
					pipeline->AddRenderObject(m_SpriteList[0].get());
					break;
                case 2:
					pipeline->AddRenderObject(m_SpriteList[0].get());
					pipeline->AddRenderObject(m_SpriteList[1].get());
					break;
                case 3:
					pipeline->AddRenderObject(m_SpriteList[0].get());
					pipeline->AddRenderObject(m_SpriteList[1].get());
					pipeline->AddRenderObject(m_SpriteList[2].get());
					break;
            }
        }
        else if (m_DelayCount > 3.0f)
        {
            SetAnimationState(AnimationState::Finish);
            m_DelayCount = 0.0f;
        }
        break;
    default:
        break;
    }
    return true;
}

void DamageEffectHUD::finishAction()
{
}

void DamageEffectHUD::SetDamageEffect(float damageRatio)
{
    if (damageRatio > 0.66f)
    {
        m_EffectCount = 1;
    }
    else if (damageRatio > 0.33f)
    {
        m_EffectCount = 2;
    }
    else
    {
		m_EffectCount = 3;
    }
    SetAnimationState(AnimationState::Run);
}
