#include "HUDManager.h"

#include <iostream>
#include <random>

void HUDManager::InitAction()
{
	MeterHUD* meterHUD = new MeterHUD();            //兵数・士気バーHUD
    GetGameObject()->addComponent(meterHUD);
	AddHUDObject("MeterHUD", meterHUD);

	StatusHUD* statusHUDs = new StatusHUD();        //ブラウン管テレビのキャラクター顔面カメラHUD
    GetGameObject()->addComponent(statusHUDs);
    AddHUDObject("StatusHUD", statusHUDs);

	StatusText* statusText = new StatusText();      //選択中キャラクターの名前を表示するテキストHUD
    GetGameObject()->addComponent(statusText);
    AddHUDObject("StatusText", statusText);

    AbilityHUD* abillityHUD = new AbilityHUD();     //アビリティ用のノートHUD
    GetGameObject()->addComponent(abillityHUD);
    AddHUDObject("AbilityHUD", abillityHUD);

    GuideHUD* guideHUD = new GuideHUD();            //画面下のガイドHUD
    GetGameObject()->addComponent(guideHUD);
    AddHUDObject("GuideHUD", guideHUD);

    SideMenuHUD* sideMenuHUD = new SideMenuHUD();   //サイドメニューHUD
    GetGameObject()->addComponent(sideMenuHUD);
    AddHUDObject("SideMenuHUD", sideMenuHUD);

    CurrentTerrainHUD* currentTerrainHUD = new CurrentTerrainHUD(); //カーソルを合わせたマスの地形を表示するHUD
    GetGameObject()->addComponent(currentTerrainHUD);
    AddHUDObject("CurrentTerrainHUD", currentTerrainHUD);

    BattleCameraHUD* battleCameraHUD = new BattleCameraHUD();       //戦闘画面用HUD
    GetGameObject()->addComponent(battleCameraHUD);
    AddHUDObject("BattleCameraHUD", battleCameraHUD);

    DamageEffectHUD* damageEffectHUD = new DamageEffectHUD();       //ダメージを受けた出血UI
    GetGameObject()->addComponent(damageEffectHUD);
    AddHUDObject("DamageEffectHUD", damageEffectHUD);

    SuperiorityGaugeHUD* superiorityGaugeHUD = new SuperiorityGaugeHUD();   //勢力ゲージHUD
    GetGameObject()->addComponent(superiorityGaugeHUD);
    AddHUDObject("SuperiorityGaugeHUD", superiorityGaugeHUD);

    LoadAnimationHUD* loadAnimationHUD = new LoadAnimationHUD();   //ロードアニメーションHUD
    GetGameObject()->addComponent(loadAnimationHUD);
    AddHUDObject("LoadAnimationHUD", loadAnimationHUD);

    MyAccessHub::SetLoadAnimationHUD(loadAnimationHUD);

    MainMenuHUD* mainMenuHUD = new MainMenuHUD();   //メインメニューHUD
    GetGameObject()->addComponent(mainMenuHUD);
    AddHUDObject("MainMenuHUD", mainMenuHUD);

    TurnEndHUD* turnEndHUD = new TurnEndHUD();   //ターン終了HUD
    GetGameObject()->addComponent(turnEndHUD);
    AddHUDObject("TurnEndHUD", turnEndHUD);
}

bool HUDManager::FrameAction()
{
	return true;
}

void HUDManager::FinishAction()
{
}

void HUDManager::ResetHUDWhenMoveCursor()
{
    m_HUDList["MeterHUD"]->SetAnimationState(AnimationState::Init);
    m_HUDList["StatusHUD"]->SetAnimationState(AnimationState::Init);
}

void MeterHUD::InitAction()
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
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kArrowLeftPosX, kSoldierArrowPosY, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(25.0f, 25.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"SoldierBarArrowTexture");
            break;
        case 1:
            m_SpriteList[i]->setPosition(kArrowLeftPosX, kMoraleArrowPosY, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(25.0f, 25.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"SoldierBarArrowTexture");
            break;
        case 2:
            m_SpriteList[i]->setPosition(kMeterPositionX, kSoldierMeterPositionY, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"SoldierTexture");
            break;
        case 3:
            m_SpriteList[i]->setPosition(kMeterPositionX, kMoraleMeterPositionY, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"MoraleTexture");
            break;
        }
    }

    m_TextList["兵数"] = L"";
    m_TextList["士気"] = L"";

    SetAnimationState(AnimationState::Init);
}

bool MeterHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

void MeterHUD::FinishAction()
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

void HUDObject::MakeSpriteObject(const wchar_t* textureId, wstring cameraLabel, wstring pipeLine, XMFLOAT4 pattern, XMFLOAT4 color)
{
    SpriteCharacter* sprite = new SpriteCharacter();
    m_SpriteList.push_back(std::unique_ptr<SpriteCharacter>(sprite));
    sprite->SetTextureId(textureId);
    sprite->SetCameraLabel(cameraLabel, 0);
    sprite->SetGraphicsPipeLine(pipeLine);
    sprite->SetSpritePattern(0, 1, 1, pattern);
    sprite->setSpriteIndex(0);
    sprite->SetColor(color.x, color.y, color.z, color.w);
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
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    CharacterData* chData = GetGameObject()->GetCharacterData();

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

void StatusHUD::InitAction()
{
    SetAnimationState(AnimationState::OnInit);

    m_AnimationPages = 10;
    m_FlipDuration = 0.05f;

    m_SpriteList.resize(4);

    for (int i = 0; i < m_SpriteList.size(); i++)
    {
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"Sprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:     //ブラウン管のノイズ
            m_SpriteList[i]->setPosition(380.0f, 200.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(160.0f, 120.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"BrownTelevisionNoizeTexture");
            break;
        case 2:     //テレビラベル
            m_SpriteList[i]->setPosition(380.0f, 155.0f, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 40.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"Sprite00");
            break;
        case 1:     //ブラウン管テレビフレーム
            m_SpriteList[i]->setPosition(380.0f, 200.0f, OrderInLayer::Text + 0.2f);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"BrownTelevisiomnFrameImage");
            break;
        case 3:     //テレビの背景
            m_SpriteList[i]->setPosition(380.0f, 225.0f, OrderInLayer::BackGround + 0.2f);
            m_SpriteList[i]->SetCameraLabel(L"BackGroundHUDCamera", 0); //背景用ラベルに切り替え
            m_SpriteList[i]->setScale(200.0f, 150.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"Sprite00");
            break;
        }
    }
    SetAnimationState(AnimationState::Init);
}

bool StatusHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"Sprite");
    SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());

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
		FlipAnimation(m_SpriteList[0].get());   //テレビのノイズアニメーション
        index = 2;
        break;
    case AnimationState::Finish:

        index = 3;

        switch (BFMng->GetCursorState())
        {
            case CursorState::Select:
                if (selectSquare->chara == nullptr)
					return true;

				if (!selectSquare->chara->Detected && selectSquare->chara->CharaAdmin == Admin::Imperial)   //未発見の帝国軍キャラクターを選択した場合はエラー表示
                {
                    SetAnimationState(AnimationState::Init);
                    index = 2;
                }
				break;
            case CursorState::Target:
                if (targetSquare->chara == nullptr)
                    return true;

                if (!targetSquare->chara->Detected && targetSquare->chara->CharaAdmin == Admin::Imperial)   //未発見の帝国軍キャラクターを選択した場合はエラー表示
                {
                    SetAnimationState(AnimationState::Init);
                    index = 2;
                }
				break;
        }

        if (BFMng->GetHUDEnableCondition() && !BFMng->GetBattleCameraEnable())
        {
            p_scene->SetActiveCameraCompornent(L"ScoutingCamera", true);
        }
        else
        {
            p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
        }

        if (BFMng->GetMode() == Mode::TurnEndMode)
        {
            p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
        }
        break;
    }

    if (BFMng->GetHUDEnableCondition() && BFMng->GetFieldSquaresList()[BFMng->GetSelectID()]->chara != nullptr && BFMng->GetCursorState() == CursorState::Select && BFMng->GetMode() != Mode::TurnEndMode)
    {
        SetLabelColor();
        SetRender(index);
    }
    else if (BFMng->GetHUDEnableCondition() && BFMng->GetFieldSquaresList()[BFMng->GetTargetID()]->chara != nullptr && BFMng->GetCursorState() == CursorState::Target && BFMng->GetMode() != Mode::TurnEndMode)
    {
        SetLabelColor();
        SetRender(index);
    }

    return true;
}

void StatusHUD::FinishAction()
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
            m_SpriteList[2]->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case Admin::Imperial:
            m_SpriteList[2]->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
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
            m_SpriteList[2]->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case Admin::Imperial:
            m_SpriteList[2]->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        }
        break;
    }
    
}

void StatusHUD::SetRender(int index)
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    switch (index)
    {
    case 2: 
        pipe->AddRenderObject(m_SpriteList[0].get());   //ノイズ描画
		pipe->AddRenderObject(m_SpriteList[1].get());   //テレビフレーム描画
		break;
    case 3:
		pipe->AddRenderObject(m_SpriteList[1].get());   //テレビフレーム描画
		pipe->AddRenderObject(m_SpriteList[2].get());   //キャラクター顔面カメラ描画
		pipe->AddRenderObject(m_SpriteList[3].get());   //キャラクター背景カメラ描画
		break;
    }
}

void StatusText::InitAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);
}

bool StatusText::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

void StatusText::FinishAction()
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
            m_WordSpriteList[count]->SetColor(color.x, color.y, color.z, 1);

            m_WordSpriteList[count]->setPosition(ltX, ltY, 0.0f);
            count++;
        }

        ltX += width + m_TextDuration;

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
            m_WordSpriteList[count]->SetColor(color.x, color.y, color.z, 1);
            count++;
            startX += width + 2.0f;
        }

        str++;
    }

    return count;
}

void HUDTextObject::SetFont(const wchar_t* fontTextureId, const wchar_t* fontWordList)
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();

    SpriteCharacter* spc;

    for (int i = 0; i < m_SpriteCount; i++)
    {
        spc = new SpriteCharacter();

        spc->SetTextureId(fontTextureId);
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

void AbilityHUD::InitAction()
{
    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 4; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(500.0f, 500.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->SetTextureId(L"AbillityBackGroundTexture");
            break;
        case 1:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 400.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->SetTextureId(L"AbillityNoteTexture");
            break;
        case 2:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 400.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->SetTextureId(L"AbillityNoteTexture");
            break;
        case 3:
            m_SpriteList[i]->setPosition(0.0f, 0.0f, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(400.0f, 400.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            m_SpriteList[i]->SetTextureId(L"AbillityNoteTexture");
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

bool AbilityHUD::FrameAction()
{
    if (BFMng->GetHUDEnableCondition() && BFMng->GetMode() == Mode::AbilityMode && BFMng->GetTargetMode() == TargetMode::None)
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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
                        m_SpriteList[i + 1]->SetTextureId(L"AbillityConcentratedFireTexture");
                        break;
                    case AbilityType::BayonetCharge:
                        m_SpriteList[i + 1]->SetTextureId(L"AbillityBayonetChargeTexture");
                        break;
                    case AbilityType::Scout:
                        m_SpriteList[i + 1]->SetTextureId(L"AbillityScoutTexture");
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
                m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
            }

            switch (BFMng->GetAbillityIndex())
            {
            case 0:
                m_SpriteList[1]->SetColor(1.0f, 0.5f, 0.5f, 1);
                break;
            case 1:
                m_SpriteList[2]->SetColor(1.0f, 0.5f, 0.5f, 1);
                break;
            case 2:
                m_SpriteList[3]->SetColor(1.0f, 0.5f, 0.5f, 1);
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

void AbilityHUD::FinishAction()
{
}

void AbilityHUD::ResetHUD()
{
    SetAnimationState(AnimationState::Init);
    m_AbilityHUDState = AbilityHUDState::FolderUp;
	m_AbillityNoteScale = kOriginAbillityNoteScale;
}

void GuideHUD::InitAction()
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
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kGuideBackGroundPos.x, kGuideBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(700.0f, 700.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"WindowTexture");
            break;
        }
    }
	
	m_TextList["フィールド"] = (L"方向キー:選択_スペースキー:決定_WASDキー:カメラ移動");
	m_TextList["フィールド2"] = (L"E/Qキー:カメラズームイン/アウト");
	m_TextList["メニュー"] = (L"上下キー:カーソル移動_スペースキー:決定_エスケープキー:閉じる");
	m_TextList["アビリティ"] = (L"左右キー:選択_スペースキー:決定_エスケープキー:閉じる");
	m_TextList["敵へアビリティ"] = (L"方向キー:選択_スペースキー::実行_エスケープキー:閉じる");
	m_TextList["攻撃"] = (L"方向キー:選択_スペースキー:攻撃_エスケープキー:閉じる");
	m_TextList["移動"] = (L"方向キー:選択_スペースキー:移動_エスケープキー:閉じる");
}

bool GuideHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

void GuideHUD::FinishAction()
{
}

void SideMenuHUD::InitAction()
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
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
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
            m_SpriteList[i]->SetTextureId(L"Sprite00");
            break;
        case 3:
            m_SpriteList[i]->setPosition(kSideMenuBackGroundPos.x, kSideMenuBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(600.0f, 600.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"SideMenuTexture");
            break;
        }
    }

    m_TextList["ターン終了"] = (L"ターン終了");
    m_TextList["ゲーム終了"] = (L"ゲーム終了");
    m_TextList["キャンセル"] = (L"キャンセル");
}

bool SideMenuHUD::FrameAction()
{
    if (BFMng->GetMode() == Mode::SideMenuMode)
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

void SideMenuHUD::FinishAction()
{
}

void CurrentTerrainHUD::InitAction()
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
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kBackGroundPos.x, kBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(200.0f, 200.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"TerrainHUDTexture");
            break;
        }
    }

    m_TextList["地形名称"] = (L"");
    m_TextList["地形効果"] = (L"地形効果:");
    m_TextList["地形効果詳細"] = (L"");
    m_TextList["座標"] = (L"");

    SetAnimationState(AnimationState::Init);
}

bool CurrentTerrainHUD::FrameAction()
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

    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

void CurrentTerrainHUD::FinishAction()
{
}

void BattleCameraHUD::InitAction()
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
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kBackGroundPos.x, kBackGroundPos.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(960.0f, 950.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"BattleCameraHUDTexture");
            break;
        }
    }

    m_TextList["攻撃側"] = (L"");
    m_TextList["防御側"] = (L"");

    SetAnimationState(AnimationState::Init);
}

bool BattleCameraHUD::FrameAction()
{
    if (BFMng->GetBattleCameraEnable() == true)
    {
        MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
        GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

		int count = 0;

        FieldCharacter* attackerCharacterSquare = BFMng->GetAttackingCharacterSquares()->GetNowChara()->chara;
        FieldCharacter* defenderCharacterSquare = BFMng->GetAttackingCharacterSquares()->GetNextChara()->chara;

        if (attackerCharacterSquare != nullptr && defenderCharacterSquare != nullptr)
        {
            m_TextList["攻撃側"] = (attackerCharacterSquare->CharaName.c_str());
            m_TextList["防御側"] = (defenderCharacterSquare->CharaName.c_str());

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

void BattleCameraHUD::FinishAction()
{
}

void MainMenuHUD::InitAction()
{
    SetShaderResoruce();

    m_FontTextureId = L"JPNHUDTextureVT";
    m_FontWordList = m_WordList.m_chListJVT;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    m_TextDuration = 8.0f;

    for (int i = 0; i < 25; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);
        
        if (i < 18) //0～17はテキストを表示する場所
        {
            m_SpriteList[i]->SetTextureId(L"NixieTubesTexture");
            m_SpriteList[i]->setScale(110.0f, 90.0f, 0.1f);
        }
        else if (i < 23)    //18～22は表示部分の基盤の柱
        {
            m_SpriteList[i]->SetTextureId(L"NixieBaseTexture");
            m_SpriteList[i]->setScale(250.0f, 250.0f, 0.1f);
        }
        else if (i < 24)    //23はメニュー部分の基盤
        {
            m_SpriteList[i]->SetTextureId(L"MenuBaseTexture");
            m_SpriteList[i]->setScale(200.0f, 330.0f, 0.1f);
        }
		else if (i < 25)    //24はゲージ部分の基盤
        {
            m_SpriteList[i]->SetTextureId(L"NixieGageTexture");
            m_SpriteList[i]->setScale(300.0f, 300.0f, 0.1f);
        }
    }

    //=====================文字用ニキシー管のポジション=========================
    m_SpriteList[0]->setPosition(kNixieTubeDefaultPositionX[1], kNixieTubePositionY[0], m_PosZ + 0.5f);    //攻
    m_SpriteList[1]->setPosition(kNixieTubeDefaultPositionX[2], kNixieTubePositionY[0], m_PosZ + 0.5f);    //撃

    m_SpriteList[2]->setPosition(kNixieTubeDefaultPositionX[1], kNixieTubePositionY[1], m_PosZ + 0.5f);    //移
    m_SpriteList[3]->setPosition(kNixieTubeDefaultPositionX[2], kNixieTubePositionY[1], m_PosZ + 0.5f);    //動

    m_SpriteList[4]->setPosition(kNixieTubeDefaultPositionX[1], kNixieTubePositionY[2], m_PosZ + 0.5f);    //行
    m_SpriteList[5]->setPosition(kNixieTubeDefaultPositionX[2], kNixieTubePositionY[2], m_PosZ + 0.5f);    //動

    m_SpriteList[6]->setPosition(kNixieTubeDefaultPositionX[1], kNixieTubePositionY[3], m_PosZ + 0.5f);    //待
    m_SpriteList[7]->setPosition(kNixieTubeDefaultPositionX[2], kNixieTubePositionY[3], m_PosZ + 0.5f);    //機

    m_SpriteList[8]->setPosition(kNixieTubeDefaultPositionX[1], kNixieTubePositionY[4], m_PosZ + 0.5f);    //キ
    m_SpriteList[9]->setPosition(kNixieTubeDefaultPositionX[2], kNixieTubePositionY[4], m_PosZ + 0.5f);    //ャ
    m_SpriteList[10]->setPosition(kNixieTubeDefaultPositionX[3], kNixieTubePositionY[4], m_PosZ + 0.5f);   //ン
    m_SpriteList[11]->setPosition(kNixieTubeDefaultPositionX[4], kNixieTubePositionY[4], m_PosZ + 0.5f);   //セ
    m_SpriteList[12]->setPosition(kNixieTubeDefaultPositionX[5], kNixieTubePositionY[4], m_PosZ + 0.5f);   //ル

    //=====================矢印用ニキシー管のポジション=========================
    m_SpriteList[13]->setPosition(kNixieTubeDefaultPositionX[0], kNixieTubePositionY[0], m_PosZ + 0.5f);
    m_SpriteList[14]->setPosition(kNixieTubeDefaultPositionX[0], kNixieTubePositionY[1], m_PosZ + 0.5f);
    m_SpriteList[15]->setPosition(kNixieTubeDefaultPositionX[0], kNixieTubePositionY[2], m_PosZ + 0.5f);
    m_SpriteList[16]->setPosition(kNixieTubeDefaultPositionX[0], kNixieTubePositionY[3], m_PosZ + 0.5f);
    m_SpriteList[17]->setPosition(kNixieTubeDefaultPositionX[0], kNixieTubePositionY[4], m_PosZ + 0.5f);

    //=====================ニキシー管の基盤部分ポジション=========================
    m_SpriteList[18]->setPosition(kNixieBaseDefaultPositionX, kNixieBasePositionY[0], m_PosZ);
    m_SpriteList[19]->setPosition(kNixieBaseDefaultPositionX, kNixieBasePositionY[1], m_PosZ);
    m_SpriteList[20]->setPosition(kNixieBaseDefaultPositionX, kNixieBasePositionY[2], m_PosZ);
    m_SpriteList[21]->setPosition(kNixieBaseDefaultPositionX, kNixieBasePositionY[3], m_PosZ);
    m_SpriteList[22]->setPosition(kNixieBaseDefaultPositionX, kNixieBasePositionY[4], m_PosZ);

    //=====================メニューの基盤部分のポジション=========================
    m_SpriteList[23]->setPosition(kMenuBaseDefaultPositionX, kMenuBasePositionY, m_PosZ);

    //=====================メニューのバーのポジション=========================
    m_SpriteList[24]->setPosition(kMenuGageDefaultPositionX, kMenuGagePositionY[0], m_PosZ + 0.2f);

    m_TextList["攻撃"] = (L"攻撃");
    m_TextList["移動"] = (L"移動");
    m_TextList["行動"] = (L"行動");
    m_TextList["待機"] = (L"待機");
    m_TextList["キャンセル"] = (L"キャンセル");
    m_TextList[">"] = (L">");
}

bool MainMenuHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    int renderCount = 0;

    int count = 0;

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
    default:
		break;
    case AnimationState::OnInit:

        if (m_MenuAnimationCount == 0.0f)   //最初のフレームでメニュー全体を動かす
        {
            SetEasingAnimation(m_SpriteList[0].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[1], kNixieTubeMovedPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[1].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[2], kNixieTubeMovedPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[2].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[1], kNixieTubeMovedPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[3].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[2], kNixieTubeMovedPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[4].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[1], kNixieTubeMovedPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[5].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[2], kNixieTubeMovedPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[6].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[1], kNixieTubeMovedPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[7].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[2], kNixieTubeMovedPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[8].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[1], kNixieTubeMovedPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[9].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[2], kNixieTubeMovedPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[10].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[3], kNixieTubeMovedPositionX[3], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[11].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[4], kNixieTubeMovedPositionX[4], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[12].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[5], kNixieTubeMovedPositionX[5], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[13].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[0], kNixieTubeMovedPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[14].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[0], kNixieTubeMovedPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[15].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[0], kNixieTubeMovedPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[16].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[0], kNixieTubeMovedPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[17].get(), EasingVector::Horizontal, kNixieTubeDefaultPositionX[0], kNixieTubeMovedPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[18].get(), EasingVector::Horizontal, kNixieBaseDefaultPositionX, kNixieBaseMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[19].get(), EasingVector::Horizontal, kNixieBaseDefaultPositionX, kNixieBaseMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[20].get(), EasingVector::Horizontal, kNixieBaseDefaultPositionX, kNixieBaseMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[21].get(), EasingVector::Horizontal, kNixieBaseDefaultPositionX, kNixieBaseMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[22].get(), EasingVector::Horizontal, kNixieBaseDefaultPositionX, kNixieBaseMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[23].get(), EasingVector::Horizontal, kMenuBaseDefaultPositionX, kMenuBaseMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);
        }
		else if (m_MenuAnimationCount > kMenuAnimationTime) //メニュー全体のアニメーション終了後
        {
			SetAnimationState(AnimationState::Init);
        }

        renderCount = 24;

		m_MenuAnimationCount += m_TimeManager->GetDeltaTime();

        break;

    case AnimationState::Init:  //ゲージのアニメーション
        m_SpriteList[24]->setPosition(kMenuGageDefaultPositionX, kMenuGagePositionY[BFMng->GetMenuSelectIndex()], m_PosZ + 0.5f);                                              //Y軸を調整
        SetEasingAnimation(m_SpriteList[24].get(), EasingVector::Horizontal, kMenuGageDefaultPositionX, kMenuGageMovedPositionX, kMenuAnimationTime, Tween::EaseInQuad);//X軸のアニメーション

        renderCount = 25;

        m_MenuAnimationCount = 0.0f;

        SetAnimationState(AnimationState::Run);
        break;
    case AnimationState::Run:   //動きなし
        renderCount = 25;
        break;
    case AnimationState::Finish:   //戻るアニメーション

        if (m_MenuAnimationCount == 0.0f)   //最初のフレームでメニュー全体を動かす
        {
            SetEasingAnimation(m_SpriteList[0].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[1], kNixieTubeDefaultPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[1].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[2], kNixieTubeDefaultPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[2].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[1], kNixieTubeDefaultPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[3].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[2], kNixieTubeDefaultPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[4].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[1], kNixieTubeDefaultPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[5].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[2], kNixieTubeDefaultPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[6].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[1], kNixieTubeDefaultPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[7].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[2], kNixieTubeDefaultPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[8].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[1], kNixieTubeDefaultPositionX[1], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[9].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[2], kNixieTubeDefaultPositionX[2], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[10].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[3], kNixieTubeDefaultPositionX[3], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[11].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[4], kNixieTubeDefaultPositionX[4], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[12].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[5], kNixieTubeDefaultPositionX[5], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[13].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[0], kNixieTubeDefaultPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[14].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[0], kNixieTubeDefaultPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[15].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[0], kNixieTubeDefaultPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[16].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[0], kNixieTubeDefaultPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[17].get(), EasingVector::Horizontal, kNixieTubeMovedPositionX[0], kNixieTubeDefaultPositionX[0], kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[18].get(), EasingVector::Horizontal, kNixieBaseMovedPositionX, kNixieBaseDefaultPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[19].get(), EasingVector::Horizontal, kNixieBaseMovedPositionX, kNixieBaseDefaultPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[20].get(), EasingVector::Horizontal, kNixieBaseMovedPositionX, kNixieBaseDefaultPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[21].get(), EasingVector::Horizontal, kNixieBaseMovedPositionX, kNixieBaseDefaultPositionX, kMenuAnimationTime, Tween::EaseInQuad);
            SetEasingAnimation(m_SpriteList[22].get(), EasingVector::Horizontal, kNixieBaseMovedPositionX, kNixieBaseDefaultPositionX, kMenuAnimationTime, Tween::EaseInQuad);

            SetEasingAnimation(m_SpriteList[23].get(), EasingVector::Horizontal, kMenuBaseMovedPositionX, kMenuBaseDefaultPositionX, kMenuAnimationTime, Tween::EaseInQuad);

            m_MenuAnimationCount += m_TimeManager->GetDeltaTime();
        }
        else if (m_MenuAnimationCount < kMenuAnimationTime) //メニュー全体のアニメーション終了後
        {
            m_MenuAnimationCount += m_TimeManager->GetDeltaTime();
        }
        else if (m_MenuAnimationCount > kMenuAnimationTime) //メニュー全体のアニメーション終了後
        {
            SetAnimationState(AnimationState::None);
            m_MenuAnimationCount = 0.0f;
        }

        renderCount = 24;

        break;
    }

    //攻撃表示
    if (BFMng->GetInLengeEnemyCount() > 0)
    {
        count = MakeSpriteString(count, kMenuTextPositionX, kMenuTextPositionY[0], 32, 54, m_TextList["攻撃"], XMFLOAT3(1.0f, 1.0f, 1.0f));
    }

    //移動表示
    count = MakeSpriteString(count, kMenuTextPositionX, kMenuTextPositionY[1], 32, 54, m_TextList["移動"], XMFLOAT3(1.0f, 1.0f, 1.0f));

    //アビリティ表示
    if (BFMng->GetInLengeEnemyCount() > 0)
    {
        count = MakeSpriteString(count, kMenuTextPositionX, kMenuTextPositionY[2], 32, 54, m_TextList["行動"], XMFLOAT3(1.0f, 1.0f, 1.0f));
    }

    //待機表示
    count = MakeSpriteString(count, kMenuTextPositionX, kMenuTextPositionY[3], 32, 54, m_TextList["待機"], XMFLOAT3(1.0f, 1.0f, 1.0f));

    //キャンセル表示
    count = MakeSpriteString(count, kMenuTextPositionX, kMenuTextPositionY[4], 32, 54, m_TextList["キャンセル"], XMFLOAT3(1.0f, 1.0f, 1.0f));

    //矢印表示
    float arrowPositionY = 0.0f;

    //メニューの矢印のY座標を更新
    switch (BFMng->GetMenuSelectIndex())
    {
    default:
        break;
    case 0:
        arrowPositionY = kMenuTextPositionY[0];
        break;
    case 1:
        arrowPositionY = kMenuTextPositionY[1];
        break;
    case 2:
        arrowPositionY = kMenuTextPositionY[2];
        break;
    case 3:
        arrowPositionY = kMenuTextPositionY[3];
        break;
    case 4:
        arrowPositionY = kMenuTextPositionY[4];
        break;
    }

    count = MakeSpriteString(count, kMenuArrowPositionX, arrowPositionY, 32, 54, m_TextList[">"], XMFLOAT3(1.0f, 1.0f, 1.0f));

    for (int i = 0; i < renderCount; i++)
    {
        pipeline->AddRenderObject(m_SpriteList[i].get());
    }

    if (m_AnimationState == AnimationState::Run || m_AnimationState == AnimationState::Init)
    {
        for (int i = 0; i < count; i++)
        {
            pipeline->AddRenderObject(m_WordSpriteList[i].get());
        }
    }
    return true;
}

void MainMenuHUD::FinishAction()
{
}

void DamageEffectHUD::InitAction()
{
    for (int i = 0; i < 3; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
        case 0:
            m_SpriteList[i]->setPosition(kLowDamageEffectPosition.x, kLowDamageEffectPosition.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(kSmallDamageEffectScale, kSmallDamageEffectScale, 0.1f);
            m_SpriteList[i]->SetTextureId(L"DamagedEffectTexture");
            break;
        case 1:
            m_SpriteList[i]->setPosition(kMidDamageEffectPosition.x, kMidDamageEffectPosition.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(kLargeDamageEffectScale, kLargeDamageEffectScale, 0.1f);
            m_SpriteList[i]->SetTextureId(L"DamagedEffectTexture");
            break;
        case 2:
            m_SpriteList[i]->setPosition(kHighDamageEffectPosition.x, kHighDamageEffectPosition.y, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(kSmallDamageEffectScale, kSmallDamageEffectScale, 0.1f);
            m_SpriteList[i]->SetTextureId(L"DamagedEffectTexture");
            break;
        }
    }
}

bool DamageEffectHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
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

void DamageEffectHUD::FinishAction()
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

void SuperiorityGaugeHUD::InitAction()
{
    for (int i = 0; i < 6; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i] = std::make_unique<SpriteCharacter>();
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->SetColor(1.0f, 1.0f, 1.0f, 1);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, m_PatternRect);
        m_SpriteList[i]->setSpriteIndex(0);

        switch (i)
        {
		case 0: //ゲージの背景
            m_SpriteList[i]->setPosition(0.0f, kGagePositionY, OrderInLayer::BackGround);
            m_SpriteList[i]->setScale(310.0f, 100.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"DogtagBaseTexture");
            break;
        case 1: //味方ゲージ
            m_SpriteList[i]->setPosition(0.0f, kGagePositionY, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(300.0f, 300.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"AliesGageTexture");
            break;
		case 2: //敵ゲージ
            m_SpriteList[i]->setPosition(0.0f, kGagePositionY, OrderInLayer::MoveObject);
            m_SpriteList[i]->setScale(300.0f, 300.0f, 0.1f);
            m_SpriteList[i]->SetTextureId(L"EnemyGageTexture");
            break;
		case 3: //ゲージの節　左
            m_SpriteList[i]->setPosition(-75.0f, kGagePositionY, OrderInLayer::Text);
            m_SpriteList[i]->setScale(2.0f, 30.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 0.0f, 1);
            m_SpriteList[i]->SetTextureId(L"Sprite00");
            break;
        case 4: //ゲージの節　中央
            m_SpriteList[i]->setPosition(0.0f, kGagePositionY, OrderInLayer::Text);
            m_SpriteList[i]->setScale(2.0f, 30.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 0.0f, 1);
            m_SpriteList[i]->SetTextureId(L"Sprite00");
            break;
		case 5: //ゲージの節　右
            m_SpriteList[i]->setPosition(75.0f, kGagePositionY, OrderInLayer::Text);
            m_SpriteList[i]->setScale(2.0f, 30.0f, 0.1f);
            m_SpriteList[i]->SetColor(1.0f, 1.0f, 0.0f, 1);
            m_SpriteList[i]->SetTextureId(L"Sprite00");
            break;
        }
    }

    m_AnimationState = AnimationState::Init;
}

bool SuperiorityGaugeHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    switch (m_AnimationState)
    {
    case AnimationState::Init:
        SetGagePercent(BFMng->GetStrengthValues());
		m_AnimationState = AnimationState::Run;
        break;
    case AnimationState::Run:
        if (m_AnimationCount < 0.5f)
        {
			float t = m_AnimationCount / 0.5f; // 0から1への正規化された時間
			m_GageSizeX = m_StartGageSizeX.x + (m_EndGageSizeX.x - m_StartGageSizeX.x) * t; // 線形補間

			XMFLOAT4 aliesGagePatternRect = { 0.0f, 0.0f, m_GageSizeX / 300.0f, 1.0f }; //味方ゲージのパターン矩形を計算
			XMFLOAT4 enemyGagePatternRect = { 0.0f, 0.0f, 1.0f - (m_GageSizeX / 300.0f), 1.0f }; //敵ゲージのパターン矩形を計算

			m_SpriteList[1]->setScale(m_GageSizeX, 300.0f, 0.1f);           //味方ゲージのサイズを更新
            m_SpriteList[1]->SetSpritePattern(0, 1, 1, aliesGagePatternRect);
			m_SpriteList[1]->setPosition(kGageLeftPosition + m_GageSizeX / 2, kGagePositionY, OrderInLayer::MoveObject); //味方ゲージの位置を更新
			m_SpriteList[2]->setScale(300.0f - m_GageSizeX, 300.0f, 0.1f);  //敵ゲージのサイズを更新
			m_SpriteList[2]->setPosition(kGageRightPosition - (300.0f - m_GageSizeX) / 2, kGagePositionY, OrderInLayer::MoveObject); //敵ゲージの位置を更新
            m_SpriteList[2]->SetSpritePattern(0, 1, 1, enemyGagePatternRect);
			m_AnimationCount += m_TimeManager->GetDeltaTime();
        }
        else
        {
            m_AnimationCount = 0.0f;
			SetAnimationState(AnimationState::Finish);
        }
        break;
    default:
        break;
    }

    if (!BFMng->GetBattleCameraEnable() && BFMng->GetHUDEnableCondition())
    {
        for (int i = 0; i < m_SpriteList.size(); i++)
        {
            pipeline->AddRenderObject(m_SpriteList[i].get());
        }
    }
    

    return true;
}

void SuperiorityGaugeHUD::FinishAction()
{
}

void SuperiorityGaugeHUD::SetGagePercent(XMFLOAT2 strengthValues)
{
	m_CurrentGagePercent = m_NextGagePercent;                                       //ゲージの現在の割合を保存
	m_NextGagePercent = strengthValues.x / (strengthValues.x + strengthValues.y);   //ゲージの割合を計算

    m_StartGageSizeX = { 300.0f * m_CurrentGagePercent, 300.0f - 300.0f * m_CurrentGagePercent };   //ゲージの現在のサイズを計算
    m_EndGageSizeX = { 300.0f * m_NextGagePercent , 300.0f - 300.0f * m_NextGagePercent };          //ゲージの現在のサイズを計算
}

void LoadAnimationHUD::InitAction()
{
    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    for (int i = 0; i < 1; i++)
    {
        m_SpriteList.push_back(std::make_unique<SpriteCharacter>());
        m_SpriteList[i]->SetTextureId(L"Sprite00");
        m_SpriteList[i]->SetCameraLabel(L"HUDCamera", 0);
        m_SpriteList[i]->SetGraphicsPipeLine(L"AlphaSprite");
        m_SpriteList[i]->SetSpritePattern(0, 1, 1, pattern);
        m_SpriteList[i]->setSpriteIndex(0);
        m_SpriteList[i]->SetColor(0.0f, 0.0f, 0.0f, 1);
    }

    //背景
    m_SpriteList[0]->setScale(1000.0f, 800.0f, 0.1f);
    m_SpriteList[0]->setPosition(0.0f, 0.0f, 1.0f);
}

bool LoadAnimationHUD::FrameAction()
{
    if (!MyAccessHub::GetMyGameEngine()->GetSceneController()->GetIsLoading())
    {
        return true;
    }

    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");
    int count = 0;

    if (m_AnimationCount < 0.2f)
    {
		m_TextList["ロードテキスト"] = L">データを送信中";
    }
    else if (m_AnimationCount < 0.4f)
    {
        m_TextList["ロードテキスト"] = L">データを送信中*";
    }
    else if (m_AnimationCount < 0.6f)
    {
        m_TextList["ロードテキスト"] = L">データを送信中**";
    }
    else if (m_AnimationCount < 0.8f)
    {
        m_TextList["ロードテキスト"] = L">データを送信中***";
    }
    else
    {
		m_AnimationCount = 0.0f;
    }

    count = MakeSpriteString(count, pos.x, pos.y, 40, 60, m_TextList["ロードテキスト"], XMFLOAT3( 1.0f, 1.0f, 0.7f ));

    for (int i = 0; i < m_SpriteList.size(); i++)
    {
        pipeline->AddRenderObject(m_SpriteList[i].get());
    }

    for (int i = 0; i < count; i++)
    {
        pipeline->AddRenderObject(m_WordSpriteList[i].get());
    }

    m_AnimationCount += m_TimeManager->GetDeltaTime();

    return true;
}

void LoadAnimationHUD::FinishAction()
{
}

void TurnEndHUD::InitAction()
{
    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f, 1.0f);

    m_FontTextureId = L"JPNHUDTexture";
    m_FontWordList = m_WordList.m_chListJ;
    m_SpriteCount = 50;
    SetFont(m_FontTextureId, m_FontWordList);

    m_TextList["ターン終了テキスト"] = L"ターンを終了しますか？";

	MakeSpriteObject(L"TurnEndBackGroundTexture", L"HUDCamera", L"AlphaSprite", pattern, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)); //メーター背景
	MakeSpriteObject(L"TurnEndArrowTexture", L"HUDCamera", L"AlphaSprite", pattern, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));      //メーター矢印
	MakeSpriteObject(L"DogtagBaseTexture", L"HUDCamera", L"AlphaSprite", pattern, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));        //テキスト背景

    //背景
    m_SpriteList[0]->setScale(600.0f, 600.0f, 0.1f);
    m_SpriteList[0]->setPosition(0.0f, -50.0f, OrderInLayer::BackGround);

    //矢印
    m_SpriteList[1]->setScale(600.0f, 600.0f, 0.1f);
    m_SpriteList[1]->setPosition(0.0f, -150.0f, OrderInLayer::MoveObject);

    //テキスト背景
    m_SpriteList[2]->setScale(300.0f, 300.0f, 0.1f);
    m_SpriteList[2]->setPosition(0.0f, 100.0f, OrderInLayer::BackGround);
}

bool TurnEndHUD::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipeline = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    int count = 0;

    count = MakeSpriteString(count, pos.x, pos.y, 40, 60, m_TextList["ターン終了テキスト"], XMFLOAT3(1.0f, 1.0f, 1.0f));

    switch (m_AnimationState)
    {
    default:
		break;
    case AnimationState::Init:
        SetEasingAnimation(m_SpriteList[0].get(), EasingVector::Verticle, kBackGroundPosY[0], kBackGroundPosY[1], kAnimationTime, Tween::EaseInQuad);
        SetEasingAnimation(m_SpriteList[1].get(), EasingVector::Verticle, kArrowPosY[0], kArrowPosY[1], kAnimationTime, Tween::EaseInQuad);
        SetEasingAnimation(m_SpriteList[2].get(), EasingVector::Verticle, kTextBackGroundPosY[0], kTextBackGroundPosY[1], kAnimationTime, Tween::EaseInQuad);

        m_InitAnimationCount += m_TimeManager->GetDeltaTime();

        if (m_InitAnimationCount > kAnimationTime)
        {
			m_InitAnimationCount = 0.0f;
            m_AnimationState = AnimationState::Run;
        }
        break;
    case AnimationState::Run:
        switch (BFMng->GetTurnEndMenuSelectIndex())
        {
        case 0:
            m_ArrowRotation = 0.0f;
            break;
        case 1:
            m_ArrowRotation = -50.0f * (m_AnimationCount / kAnimationTime);
            break;
        case 2:
            m_ArrowRotation = 50.0f * (m_AnimationCount / kAnimationTime);
            break;
        default:
            break;
        }

        m_AnimationCount += m_TimeManager->GetDeltaTime();

        if (m_AnimationCount > kAnimationTime)
        {
            m_AnimationCount = 0.0f;
            m_AnimationState = AnimationState::Finish;
        }

        for (int i = 0; i < count; i++)
        {
            pipeline->AddRenderObject(m_WordSpriteList[i].get());
        }
        break;
    case AnimationState::Finish:
        switch (BFMng->GetTurnEndMenuSelectIndex())
        {
        case 0:
            m_ArrowRotation = 0.0f;
            break;
        case 1:
            m_ArrowRotation = -50.0f;
            break;
        case 2:
            m_ArrowRotation = 50.0f;
            break;
        default:
            break;
        }

        for (int i = 0; i < count; i++)
        {
            pipeline->AddRenderObject(m_WordSpriteList[i].get());
        }
        break;
    }

    m_SpriteList[1]->setRotation(0.0f, 0.0f, m_ArrowRotation);

    if (BFMng->GetMode() == Mode::TurnEndMode)
    {
        for (int i = 0; i < m_SpriteList.size(); i++)
        {
            pipeline->AddRenderObject(m_SpriteList[i].get());
        }
    }

    return true;
}

void TurnEndHUD::FinishAction()
{
}
