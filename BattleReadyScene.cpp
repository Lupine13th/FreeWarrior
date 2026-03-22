#include "BattleReadyScene.h"

#include <MyAccessHub.h>
#include <D3D12Helper.h>
#include <DirectXMath.h>
#include <chrono>

using namespace std::chrono;

#include "SceneManager.h"
#include "SpriteCharacter.h"
#include "KeyBindComponent.h"	
#include "FieldCharacter.h"

void BattleReadyScene::SetNextScene(GAME_SCENES nextSc)
{
    m_nextScene = nextSc;
}

void BattleReadyScene::SetCharacterData(FieldCharacter* readyChara, FieldCharacter* nextChara, Squares* square, int index, int squareIndex)
{
    nextChara = readyChara;
	square->ThereCharaID = squareIndex;
	square->SqAdmin = nextChara->CharaAdmin;
	square->chara = nextChara;
	square->chara->CharaPos = squareIndex;
}

void BattleReadyScene::ResetCount()
{
	lightingcount = 0.0f;
	lighting = true;
}

void BattleReadyScene::InitAction()
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

    m_Sprite = std::make_unique<SpriteCharacter>();
    m_Sprite->SetTextureId(L"Sprite00");
    m_Sprite->SetGraphicsPipeLine(L"AlphaSprite");
    m_Sprite->SetCameraLabel(L"HUDCamera", 0);

    m_TimeManager = MyAccessHub::GetTimeManager();

    XMFLOAT4 pattern(0.0f, 0.0f, 1.0f / 512.0f, 1.0f / 512.0f);
    m_Sprite->SetSpritePattern(0, 10, 10, pattern);
    m_Sprite->setScale(100.0f, 60.0f, 0.1f);
    m_Sprite->setSpriteIndex(0);
    m_Sprite->setPosition(0.0f, 0.0f, 2.0f);
    m_Sprite->SetColor(0.0f, 0.0f, 0.0f, 1);

    //味方待機部隊設定
	InFieldAlliesCharacterList[0] = new Infantry;
    InFieldAlliesCharacterList[0]->CharaAdmin = Admin::Rebel;
    InFieldAlliesCharacterList[0]->CharaName = L"反乱軍歩兵隊";

	InFieldAlliesCharacterList[1] = new Artillery;
    InFieldAlliesCharacterList[1]->CharaAdmin = Admin::Rebel;
    InFieldAlliesCharacterList[1]->CharaName = L"反乱軍砲兵隊";

	InFieldAlliesCharacterList[2] = new MachineGunner;
    InFieldAlliesCharacterList[2]->CharaAdmin = Admin::Rebel;
    InFieldAlliesCharacterList[2]->CharaName = L"反乱軍機関銃兵隊";

	InFieldAlliesCharacterList[3] = new Scout;
    InFieldAlliesCharacterList[3]->CharaAdmin = Admin::Rebel;
    InFieldAlliesCharacterList[3]->CharaName = L"反乱軍偵察兵隊";

	InFieldAlliesCharacterList[4] = new Armored;
    InFieldAlliesCharacterList[4]->CharaAdmin = Admin::Rebel;
    InFieldAlliesCharacterList[4]->CharaName = L"反乱軍機械化歩兵隊";

    //敵部隊設定
    RDEnemyCharacterList[0] = new Artillery;
    RDEnemyCharacterList[0]->CharaAdmin = Admin::Imperial;
    RDEnemyCharacterList[0]->CharaAI = AIroutine::Defence;
    RDEnemyCharacterList[0]->CharaName = L"第1帝国軍砲兵隊";

    RDEnemyCharacterList[1] = new Infantry;
    RDEnemyCharacterList[1]->CharaAdmin = Admin::Imperial;
    RDEnemyCharacterList[1]->CharaAI = AIroutine::Attack;
    RDEnemyCharacterList[1]->CharaName = L"第1帝国軍歩兵隊";

    RDEnemyCharacterList[2] = new Scout;
    RDEnemyCharacterList[2]->CharaAdmin = Admin::Imperial;
	RDEnemyCharacterList[2]->CharaAI = AIroutine::Attack;
    RDEnemyCharacterList[2]->CharaName = L"帝国軍斥候隊";

    RDEnemyCharacterList[3] = new Infantry;
    RDEnemyCharacterList[3]->CharaAdmin = Admin::Imperial;
    RDEnemyCharacterList[3]->CharaAI = AIroutine::Attack;
    RDEnemyCharacterList[3]->CharaName = L"第2帝国軍歩兵隊";

    RDEnemyCharacterList[4] = new Artillery;
    RDEnemyCharacterList[4]->CharaAdmin = Admin::Imperial;
    RDEnemyCharacterList[4]->CharaAI = AIroutine::Defence;
    RDEnemyCharacterList[4]->CharaName = L"第2帝国軍砲兵隊";

	engine->GetSoundManager()->playBGM(10);
	engine->GetSoundManager()->play(9);
}

bool BattleReadyScene::FrameAction()
{
    MyGameEngine* engine = MyAccessHub::GetMyGameEngine();
    GraphicsPipeLineObjectBase* pipe = engine->GetPipelineManager()->GetPipeLineObject(L"AlphaSprite");

    pipe->AddRenderObject(m_Sprite.get());

    SceneManager* scene = static_cast<SceneManager*>(engine->GetSceneController());
    KeyBindComponent* keyBind = static_cast<KeyBindComponent*>(scene->getKeyComponent());

    lightingcount += m_TimeManager->GetDeltaTime();     //点滅カウント

    //0.5秒ごとに点滅
    if (lightingcount > 0.5f && lightingcount < 1.0f)  
    {
        lighting = false;
    }
    else if (lightingcount > 1.0f)
    {
        lighting = true;
        lightingcount = 0.0f;
    }

    switch (sta)
    {
    case 0:
        switch (m_ReadySceneState)
        {
        case ReadySceneState::Init:
            m_InitCount += m_TimeManager->GetDeltaTime();
            if (m_InitCount > kMaxInitCount)    //メニュー表示までのアニメーション
            {
                m_ReadySceneState = ReadySceneState::Menu;
                m_InitCount = 0.0f;
            }
            break;
        case ReadySceneState::Menu:
            if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK) && !ready)     //準備完了ボタン以外の時にスペースキーを入力
            {
                engine->GetSoundManager()->play(12);
                ResetCount();
                infieldcharacterMenuW->ResetAnimation();
                m_ReadySceneState = ReadySceneState::InField;
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK))    //十字キー下入力時
            {
                menuIndex++;
                if (menuIndex > 5)
                {
                    menuIndex = 5;
                }
                if (menuIndex == 5)
                {
                    ready = true;
                }
                ResetCount();
                engine->GetSoundManager()->play(11);
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD)) //十字キー上入力時
            {
                menuIndex--;
                if (menuIndex < 0)
                {
                    menuIndex = 0;
                }
                if (menuIndex != 5)
                {
                    ready = false;
                }
                ResetCount();
                engine->GetSoundManager()->play(11);
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK) && ready) //準備完了ボタンの時にスペースキー入力時
            {
                //味方と敵のリストにそれぞれ入れていく
                for (int i = 0; i < RDAlliesCharacterList.size(); i++)  
                {
                    if (RDAlliesCharacterList[i] != nullptr)
                    {
                        RDAlliesCharacterList[i]->CharaPos = i * 2;
                        RDAlliesCharacterList[i]->CharaID = i;
                        scene->ally[i] = RDAlliesCharacterList[i];
                    }
                }
                for (int i = 0; i < RDEnemyCharacterList.size(); i++)
                {
                    if (RDEnemyCharacterList[i] != nullptr)
                    {
                        switch (scene->kPlayStates)
                        {
                        case PlayStates::Debug:                             //デバッグ時、敵が近くなる
                            RDEnemyCharacterList[i]->CharaPos = i * 2 + 50;
                            break;
                        case PlayStates::Release:
                            RDEnemyCharacterList[i]->CharaPos = i * 2 + 140;
                            break;
                        }
                        RDEnemyCharacterList[i]->CharaID = i;
                        scene->enemy[i] = RDEnemyCharacterList[i];
                    }
                }
                engine->GetSoundManager()->play(5);
                m_ReadySceneState = ReadySceneState::Loading;
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_DEBUG))   //エンターキー入力時
            {
                for (int i = 0; i < 5; i++)
                {
                    if (InFieldAlliesCharacterList[i] != nullptr && !InFieldAlliesCharacterList[i]->Selected)
                    {
                        InFieldAlliesCharacterList[i]->Selected = true;
                        RDAlliesCharacterList[i] = InFieldAlliesCharacterList[i];
                    }
                }
            }
            break;
        case ReadySceneState::InField:  //出撃する部隊を選択する画面
            if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_OK))                       //スペースキー入力時         
            {
                if (InFieldAlliesCharacterList[m_InFieldCharacterMenuIndex] != nullptr && !InFieldAlliesCharacterList[m_InFieldCharacterMenuIndex]->Selected)   //その部隊が選択済みでは無い
                {
                    InFieldAlliesCharacterList[m_InFieldCharacterMenuIndex]->Selected = true;
                    RDAlliesCharacterList[menuIndex] = InFieldAlliesCharacterList[m_InFieldCharacterMenuIndex];
                    rdcharacterHUDW->ResetAnimation();
                    engine->GetSoundManager()->play(12);
                    ResetCount();
                    m_ReadySceneState = ReadySceneState::Menu;
                }
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_BACK) && !ready)      //十字キー下入力時  
            {
                m_InFieldCharacterMenuIndex++;
                if (m_InFieldCharacterMenuIndex > 9)
                {
                    m_InFieldCharacterMenuIndex = 9;
                }
                engine->GetSoundManager()->play(11);
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::UI_MOVE_FORWARD) && !ready)   //十字キー上入力時  
            {
                m_InFieldCharacterMenuIndex--;
                if (m_InFieldCharacterMenuIndex < 0)
                {
                    m_InFieldCharacterMenuIndex = 0;
                }
                engine->GetSoundManager()->play(11);
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::BTN_CANCEL))                  //エスケープキー入力時  
            {
                engine->GetSoundManager()->play(12);
                rdcharacterHUDW->ResetAnimation();
                ResetCount();

                m_ReadySceneState = ReadySceneState::Menu;
            }
            break;
        case ReadySceneState::Loading:  //ロード画面
            m_LoadCount += m_TimeManager->GetDeltaTime();
            if (m_LoadCount > kMaxLoadCount)
            {
                m_ReadySceneState = ReadySceneState::Finish;
                m_LoadCount = 0.0f;
            }
            break;
        case ReadySceneState::Finish:   //シーン切り替え開始
            //sta = 1;
            MyAccessHub::GetMyGameEngine()->GetSceneController()->OrderNextScene((UINT)m_nextScene);
            m_FinishCount += m_TimeManager->GetDeltaTime();
            if (m_FinishCount > kMaxFinishCount)
            {
                m_FinishCount = 0.0f;
            }
            break;
        default:
            break;
        }
        break;

    default:
        
        break;
    }

	return true;
}

void BattleReadyScene::FinishAction()
{
}
