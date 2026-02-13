#include "SceneManager.h"
#include "EnumClassList.h"
#include "MyAccessHub.h"
#include "KeyBindComponent.h"
#include "SpriteCharacter.h"
#include "SpriteRenderPipeline.h"
#include "FBXCharacterData.h"
#include "StandardFbxPipeline.h"
#include "SkyDomeComponent.h"
#include "TerrainComponent.h"
#include "CameraComponent.h"
#include "CameraChangerComponent.h"
#include "FlyingCameraController.h"
#include "MenuText.h"
#include "Squares.h"
#include "BattleFieldManager.h"
#include "MenuUI.h"
#include "MonitorSelectUI.h"
#include "EnemyAIManager.h"
#include "TurnUI.h"
#include "ForestTerrain.h"
#include "HillTerrain.h"
#include "RiverTerrain.h"
#include "TowerTerrain.h"
#include "SoldierHUD.h"
#include "MovedCountHUD.h"
#include "LogHUD.h"
#include "LogHUDW.h"
#include "BattleReadyScene.h"
#include "ReadyCharacterHUD.h"
#include "ReadyCharacterHUDW.h"
#include "InFieldCharacterMenu.h"
#include "InFieldCharacterMenuWord.h"
#include "InFieldCharacterMenuImage.h"
#include "DamageHUD.h"
#include "DamageHUDW.h"
#include "OpeningAnimHUD.h"
#include "TurnEndUI.h"
#include "TurnEndW.h"
#include "ResultUI.h"
#include "TimeManager.h"
#include "CharacterEquipment.h"
#include "TitleScene.h"
#include "HUDManager.h"
#include "EffectGenerator.h"
#include "PlayerFBXs.h"
#include "BattleCameraController.h"

void SceneManager::ClearSceneObjects()
{
	if (m_SystemObject == nullptr)
		return;
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();

	auto IsHubManagedObject = [&](GameObject* go)->bool {
		// シングルトン／マネージャーがコンポーネントとしてセットされている場合は削除しない
		// MyAccessHub の getter と比較しているだけなので、将来 getter を増やしたらここに追加すること
		if (go == m_SystemObject.get()) return true;

		for (auto& comp : go->getComponents())
		{
			if (comp == nullptr) continue;
			if (comp == MyAccessHub::GetTimeManager()) return true;
			if (comp == MyAccessHub::GetBFManager()) return true;
			if (comp == MyAccessHub::GetAIManager()) return true;
			if (comp == MyAccessHub::GetHUDManager()) return true;
			if (comp == MyAccessHub::GetFlyingCameraController()) return true;
			// 必要ならここに他のグローバル参照を追加
		}
		return false;
	};

	// list<unique_ptr<GameObject>> を安全にイテレートして、マネージャ系は削除しない
	for (auto ite = m_SceneObjects.begin(); ite != m_SceneObjects.end(); )
	{
		GameObject* go = ite->get();
		if (go == nullptr)
		{
			ite = m_SceneObjects.erase(ite);
			continue;
		}

		if (IsHubManagedObject(go))
		{
			// マネージャに関連するオブジェクトはシーンから除かず保持する（または別途移動する）
			++ite;
			continue;
		}

		// Deleter がシーン側を触って二重操作にならないように一時無効化
		go->SetGameObjectDeleter(nullptr);

		// エンジン側の削除処理を通す
		engine->RemoveGameObject(go);

		// unique_ptr を消して実際に delete
		ite = m_SceneObjects.erase(ite);
	}

	m_terrains.clear();
}

void SceneManager::SetCharaToSquares(FieldCharacter* chara, Squares* square)
{
	square->chara = chara;
	square->ThereCharaID = square->chara->CharaID;
	square->SqAdmin = chara->CharaAdmin;
}

HRESULT SceneManager::initSceneController()
{
	m_scene = static_cast<UINT>(GAME_SCENES::AWAKE);

	m_pObjDeleter = std::make_unique<SceneObjectDeleter>(this);

	HRESULT res = changeGameScene(static_cast<UINT>(GAME_SCENES::INIT));

	if (SUCCEEDED(res))
		m_orderSceneID = m_scene;

	return res;
}

HRESULT SceneManager::changeGameScene(UINT scene)
{
	HRESULT hr = S_OK;
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();

	BattleFieldManager* BFMng = nullptr;
	EnemyAIManager* AIMng = nullptr;
	BattleReadyScene* m_BattleReadyScene = nullptr;
	TimeManager* m_TimeManager = nullptr;
	TitleScene* m_TitleScene = nullptr;
	HUDManager* m_HUDManager = nullptr;
	EffectGenerator* m_EffectGenerator = nullptr;

	if (IsFirstTime)
	{
		BFMng = new	BattleFieldManager();
		AIMng = new	EnemyAIManager();
		m_BattleReadyScene = new BattleReadyScene();
		m_TimeManager = new TimeManager();
		m_TitleScene = new TitleScene();
		m_HUDManager = new HUDManager();
		m_EffectGenerator = new EffectGenerator();

		MyAccessHub::SetBattleFieldManager(BFMng);
		MyAccessHub::SetEnemyAIManager(AIMng);
		MyAccessHub::SetBattleReadyScene(m_BattleReadyScene);
		MyAccessHub::SetTimeManager(m_TimeManager);
		MyAccessHub::SetTitleScene(m_TitleScene);
		MyAccessHub::SetHUDManager(m_HUDManager);
		MyAccessHub::SetEffectGenerator(m_EffectGenerator);

		IsFirstTime = false;
	}
	else
	{
		BFMng = MyAccessHub::GetBFManager();
		AIMng = MyAccessHub::GetAIManager();
		m_BattleReadyScene = MyAccessHub::GetBRScene();
		m_TimeManager = MyAccessHub::GetTimeManager();
		m_TitleScene = MyAccessHub::GetTitleScene();
		m_HUDManager = MyAccessHub::GetHUDManager();
		m_EffectGenerator = MyAccessHub::GetEffectGenerator();
	}
	

	//現在のシーンと引数に設定されているシーンが異なる場合のみ実行
	if (m_scene != scene)
	{
		ClearSceneObjects();

		switch (scene)
		{
		case static_cast<UINT>(GAME_SCENES::INIT):	//ゲームシステム全体の初期化
			//テクスチャと効果音の読み込み
			{
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TitleBackGroundTexture", L"./Resources/textures/HUD/TitleBackGroundImage.png");	//タイトルの背景テクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TitleBackGroundTexturewithLogo", L"./Resources/textures/HUD/TitleBackGroundImagewithLogo.png");	//タイトルの背景テクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"MoraleTexture", L"./Resources/textures/HUD/MoraleBarImage.png");	//士気ゲージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"Sprite00", L"./Resources/textures/Square/SquareTexture.png");	//四角系テクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"SoldierTexture", L"./Resources/textures/HUD/SoldierBarImage.png");	//兵数メーター
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"RebelInfTexture", L"./Resources/textures/HUD/InfantryIconImage.png");	//反乱軍歩兵イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"RebelCavTexture", L"./Resources/textures/HUD/MachinegunnerIconImage.png");	//反乱軍機関銃兵イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"RebelArtTexture", L"./Resources/textures/HUD/ArtilleryIconImage.png");	//反乱軍砲兵イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"RebelSctTexture", L"./Resources/textures/HUD/ScoutIconImage.png");		//反乱軍斥候兵イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"RebelArmTexture", L"./Resources/textures/HUD/ArmoredIconImage.png");	//反乱軍機械化歩兵イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"RadioTexture", L"./Resources/textures/HUD/RadioImage.png");				//条件アニメーションのプリンター部分
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"PaporTexture", L"./Resources/textures/HUD/PaporImage.png");				//条件アニメーションの紙部分(奥行を付けるため)
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TargetWinTexture", L"./Resources/textures/HUD/TargetWImage.png");			//勝利条件
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TargetDefTexture", L"./Resources/textures/HUD/TargetDImage.png");			//敗北条件
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NixieTubesTexture", L"./Resources/textures/HUD/NixieTubesImage.png");		//ニキシー管イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NixieBaseTexture", L"./Resources/textures/HUD/NixieBaseImage.png");		//ニキシー管下部イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NixieGageTexture", L"./Resources/textures/HUD/NixieGageImage.png");		//ニキシー管ゲージイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"MenuBaseTexture", L"./Resources/textures/HUD/MenuBaseImage.png");			//ニキシー管左部イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TerrainHUDTexture", L"./Resources/textures/HUD/TerrainHUDImage.png");		//地形HUD背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"SoldierBarArrowTexture", L"./Resources/textures/HUD/BarArrowImage.png");	//メーターの矢印のイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperTexture", L"./Resources/textures/HUD/NewsPaporImage.png");		//新聞背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"DogtagTexture", L"./Resources/textures/HUD/DogtagImage.png");				//ドックタグイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"DogtagBaseTexture", L"./Resources/textures/HUD/DogtagBaseImage.png");		//ドックタグ背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"BrownTelevisionNoizeTexture", L"./Resources/textures/HUD/BrownTelevisionNoizeTexture.png");		//ブラウン管ノイズイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"BrownTelevisiomnBackGroundImage", L"./Resources/textures/HUD/BrownTelevisiomnBackGroundImage.png");	//ブラウン管背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"BrownTelevisiomnFrameImage", L"./Resources/textures/HUD/BrownTelevisiomnFrameImage.png");			//ブラウン管フレームイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"MuzzleFlashTexture", L"./Resources/textures/HUD/MuzzleFlashTexture.png");	//マズルフラッシュイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"ExplosiveTexture", L"./Resources/textures/HUD/ExplosiveTexture.png");		//爆発(通常)イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"LargeExplosiveTexture", L"./Resources/textures/HUD/LargeExplosiveTexture.png");	//爆発(大)イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TundraTexture", L"./Resources/textures/TerrainSample/BlackSoilTexture.png");		//地面イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TargetTundraTexture", L"./Resources/textures/TerrainSample/TargetBlackSoilTexture.png");	//敵の攻撃範囲イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"WindowTexture", L"./Resources/textures/HUD/WindowTexture.png");			//操作説明UI背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"SideMenuTexture", L"./Resources/textures/HUD/SideMenuTexture.png");		//サイドメニューイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"BattleCameraHUDTexture", L"./Resources/textures/HUD/BattleCameraHUDTexture.png");	//バトルカメラHUDイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"FadeInTexture", L"./Resources/textures/HUD/FadeInTexture.png");			//フェードインイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"AbillityBackGroundTexture", L"./Resources/textures/HUD/AbillityBackGroundTexture.png");	//アビリティ背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"AbillityNoteTexture", L"./Resources/textures/HUD/AbillityNoteTexture.png");	//アビリティ背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"AbillityConcentratedFireTexture", L"./Resources/textures/HUD/AbillityConcentratedFireTexture.png");	//アビリティ背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"AbillityBayonetChargeTexture", L"./Resources/textures/HUD/AbillityBayonetChargeTexture.png");	//アビリティ背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"AbillityScoutTexture", L"./Resources/textures/HUD/AbillityScoutTexture.png");	//アビリティ背景イメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"ScoutEffectTexture", L"./Resources/textures/HUD/ScoutEffectTexture.png");	//偵察エフェクトイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"ScoutedEffectTexture", L"./Resources/textures/HUD/ScoutedEffectTexture.png");	//発見エフェクトイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"CircleTexture", L"./Resources/textures/HUD/CircleTexture.png");			//サークルエフェクトイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"DamagedEffectTexture", L"./Resources/textures/HUD/DamagedEffectTexture.png");			//ダメージエフェクトイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TitleMenuEffectTexture", L"./Resources/textures/HUD/TitleMenuEffectTexture.png");		//タイトルエフェクトイメージ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TitleFlashEffect", L"./Resources/textures/HUD/TitleFlashEffect.png");		//タイトルシーン遷移イメージ

				//勝利の方のアニメーションテクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTexture", L"./Resources/textures/HUD/NewsPaperWinImage.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTexture01", L"./Resources/textures/HUD/NewsPaperWinImage01.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTexture02", L"./Resources/textures/HUD/NewsPaperWinImage02.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTexture03", L"./Resources/textures/HUD/NewsPaperWinImage03.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTexture04", L"./Resources/textures/HUD/NewsPaperWinImage04.png");

				//敗北の方のアニメーションテクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperLoseTexture", L"./Resources/textures/HUD/NewsPaperLoseImage.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperLoseTexture01", L"./Resources/textures/HUD/NewsPaperLoseImage01.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperLoseTexture02", L"./Resources/textures/HUD/NewsPaperLoseImage02.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperLoseTexture03", L"./Resources/textures/HUD/NewsPaperLoseImage03.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperLoseTexture04", L"./Resources/textures/HUD/NewsPaperLoseImage04.png");

				//勝利後のめくるアニメーションテクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTextureAfter01", L"./Resources/textures/HUD/NewsPaperWinAfter01.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTextureAfter02", L"./Resources/textures/HUD/NewsPaperWinAfter02.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTextureAfter03", L"./Resources/textures/HUD/NewsPaperWinAfter03.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTextureAfter04", L"./Resources/textures/HUD/NewsPaperWinAfter04.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTextureAfter05", L"./Resources/textures/HUD/NewsPaperWinAfter05.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinTextureAfter06", L"./Resources/textures/HUD/NewsPaperWinAfter06.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"NewsPaperWinResultTexture", L"./Resources/textures/HUD/NewsPaperWinResultImage.png");

				//現在のターンを表すアニメーションテクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies00", L"./Resources/textures/HUD/TurnHUDAllies00.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies01", L"./Resources/textures/HUD/TurnHUDAllies01.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies02", L"./Resources/textures/HUD/TurnHUDAllies02.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies03", L"./Resources/textures/HUD/TurnHUDAllies03.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies04", L"./Resources/textures/HUD/TurnHUDAllies04.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies05", L"./Resources/textures/HUD/TurnHUDAllies05.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies06", L"./Resources/textures/HUD/TurnHUDAllies06.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDAllies07", L"./Resources/textures/HUD/TurnHUDAllies07.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy00", L"./Resources/textures/HUD/TurnHUDEnemy00.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy01", L"./Resources/textures/HUD/TurnHUDEnemy01.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy02", L"./Resources/textures/HUD/TurnHUDEnemy02.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy03", L"./Resources/textures/HUD/TurnHUDEnemy03.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy04", L"./Resources/textures/HUD/TurnHUDEnemy04.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy05", L"./Resources/textures/HUD/TurnHUDEnemy05.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy06", L"./Resources/textures/HUD/TurnHUDEnemy06.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TurnHUDEnemy07", L"./Resources/textures/HUD/TurnHUDEnemy07.png");
				
				//テキストテクスチャ
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"HUDTexture", L"./Resources/textures/HUD/UITexture.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"JPNHUDTexture", L"./Resources/textures/HUD/JapaneseHUDTexture.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"JPNHUDTextureVT", L"./Resources/textures/HUD/JapaneseHUDTextureVacuumTube.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"JPNHUDTextureNP", L"./Resources/textures/HUD/JapaneseHUDTextureNewsPaper.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"DiginalHUDTexture", L"./Resources/textures/HUD/DigitalHUDTexture.png");
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"TitleTexture", L"./Resources/textures/Title/TitleSample.png");

				engine->GetMeshManager()->createPresetMeshData();


				//==========PipeLineManager
				PipeLineManager* plMng = engine->GetPipelineManager();

				SpriteRenderPipeline* spritePL = new SpriteRenderPipeline();
				spritePL->SetSamplerMode(0);
				spritePL->SetBlendMode(0);
				plMng->AddPipeLineObject(L"Sprite", spritePL);

				spritePL = new SpriteRenderPipeline();
				spritePL->SetSamplerMode(0);
				spritePL->SetBlendMode(1);
				plMng->AddPipeLineObject(L"AlphaSprite", spritePL);

				//FBX
				StandardFbxPipeline* fbxPL = new StandardFbxPipeline();
				fbxPL->SetAnimationMode(false);
				plMng->AddPipeLineObject(L"StaticFBX", fbxPL);//登録

				fbxPL = new StandardFbxPipeline();
				fbxPL->SetAnimationMode(true); //スケルタルメッシュモードへ
				plMng->AddPipeLineObject(L"AnimationFBX", fbxPL);

				HitManager* hitMng = engine->GetHitManager();

				SoundManager* soMng = engine->GetSoundManager();

				int soundId = 0;

				if (!soMng->loadSoundFile(L"./Resources/sounds/反乱軍BGM.wav", soundId))	//反乱軍BGM(0)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/帝国軍BGM.wav", soundId))	//帝国軍BGM(1)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/カーソル移動音.wav", soundId))	//カーソル移動音(2)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/決定ボタン音.wav", soundId))	//決定ボタン音(3)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/準備完了SE.wav", soundId))	//準備完了音(4)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/準備完了決定SE.wav", soundId))	//準備完了決定音(5)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/キャンセル音.wav", soundId))	//キャンセル音(6)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/撃破SE.wav", soundId))	//撃破SE(7)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/タイトルBGM.wav", soundId))	//タイトルBGM(8)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/Ready起動SE.wav", soundId))	//Ready起動SE(9)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/Ready画面BGM.wav", soundId))	//Ready画面BGM(10)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/タイピング音SE.wav", soundId))	//タイピング音SE(11)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/EnterキーSE.wav", soundId))	//EnterキーSE(12)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/プリンターSE.wav", soundId))	//プリンターSE(13)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/銃撃戦SE.wav", soundId))	//銃撃戦SE(14)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/重機関銃SE.wav", soundId))	//重機関銃SE(15)
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/砲撃SE.wav", soundId))	//砲撃SE(16)
					return E_FAIL;

				//システム制御統合オブジェクト登録
				m_SystemObject = make_unique<GameObject>(nullptr);
				m_KeyComponent = new KeyBindComponent();
				m_SystemObject->addComponent(m_KeyComponent);
				engine->AddGameObject(m_SystemObject.get());

				engine->UploadCreatedTextures();

				if (kPlayStates == PlayStates::Debug)
				{
					return changeGameScene(static_cast<UINT>(GAME_SCENES::READY));
				}
				else if (kPlayStates == PlayStates::Release)
				{
					return changeGameScene(static_cast<UINT>(GAME_SCENES::TITLE));
				}
			}
			break;

		case static_cast<UINT>(GAME_SCENES::TITLE):
		{
			GameObject* cameraObj = new GameObject(new CharacterData());
			cameraObj->addComponent(m_TitleScene);

			//タイムマネージャ
			CharacterData* timeManagerData = new CharacterData();
			GameObject* timeManagerObj = new GameObject(timeManagerData);

			timeManagerObj->addComponent(m_TimeManager);

			engine->AddGameObject(timeManagerObj);

			m_TitleScene->SetNextScene(GAME_SCENES::READY);
			AddSceneObject(cameraObj);
			m_CameraComponents[L"HUDCamera"] = m_TitleScene;
			break;
		}
		case static_cast<UINT>(GAME_SCENES::READY):
			{
				GameObject* cameraObj = new GameObject(new CharacterData());

				cameraObj->addComponent(m_BattleReadyScene);

				if (kPlayStates == PlayStates::Debug)
				{
					//タイムマネージャ
					CharacterData* timeManagerData = new CharacterData();
					GameObject* timeManagerObj = new GameObject(timeManagerData);

					timeManagerObj->addComponent(m_TimeManager);

					engine->AddGameObject(timeManagerObj);
				}
				

				ReadyCharacterHUD* RCHUD = new ReadyCharacterHUD();
				ReadyCharacterHUDW* RCHUDW = new ReadyCharacterHUDW();
				InFieldCharacterMenu* IFCM = new InFieldCharacterMenu();
				InFieldCharacterMenuWord* IFCMW = new InFieldCharacterMenuWord();
				InFieldCharacterMenuImage* IFCMI = new InFieldCharacterMenuImage();
				
				cameraObj->addComponent(RCHUD);
				cameraObj->addComponent(RCHUDW);
				cameraObj->addComponent(IFCM);
				cameraObj->addComponent(IFCMW);
				cameraObj->addComponent(IFCMI);

				m_BattleReadyScene->SetNextScene(GAME_SCENES::IN_GAME);

				m_BattleReadyScene->rdcharacterHUD = RCHUD;
				m_BattleReadyScene->rdcharacterHUDW = RCHUDW;
				m_BattleReadyScene->infieldcharacterMenu = IFCM;
				m_BattleReadyScene->infieldcharacterMenuW = IFCMW;

				AddSceneObject(cameraObj);

				m_CameraComponents[L"HUDCamera"] = m_BattleReadyScene;
			}
			break;

		case static_cast<UINT>(GAME_SCENES::IN_GAME):
			{
				PipeLineManager* plMng = engine->GetPipelineManager();
				
				//背景の地形メッシュ生成
				FBXCharacterData* terrainFbx = new FBXCharacterData();
				if (FAILED(terrainFbx->LoadMainFBX(L"./Resources/fbx/TerrainSnowField.fbx", L"TerrainSample")))
					return E_FAIL;
				terrainFbx->setPosition(0.0f, -8.0f, 0.0f);
				terrainFbx->SetGraphicsPipeLine(L"StaticFBX");
				GameObject* terrainObj = new GameObject(terrainFbx);
				TerrainComponent* trCom = new TerrainComponent();
				terrainObj->addComponent(trCom);
				AddSceneObject(terrainObj);
				m_terrains.push_back(trCom);

				//スカイドーム
				FBXCharacterData* skydomeFbx = new FBXCharacterData();
				if (FAILED(skydomeFbx->LoadMainFBX(L"./Resources/fbx/SkyDome001.fbx", L"SkyDome")))
					return E_FAIL;

				GameObject* skydomeObj = new GameObject(skydomeFbx);
				SkyDomeComponent* skCom = new SkyDomeComponent();
				skydomeObj->addComponent(skCom);

				engine->AddGameObject(skydomeObj);


				//フィールドマネージャ
				CharacterData* FMngData = new CharacterData();
				GameObject* FMngObj = new GameObject(FMngData);

				FMngObj->addComponent(BFMng);

				engine->AddGameObject(FMngObj);


				//エフェクトジェネレータ
				CharacterData* effectGenerator = new CharacterData();
				GameObject* effectGeneratorObj = new GameObject(effectGenerator);

				effectGeneratorObj->addComponent(m_EffectGenerator);

				engine->AddGameObject(effectGeneratorObj);

				m_EffectGenerator->GenerateMuzzleFlashEffect(L"MuzzleFlash");
				m_EffectGenerator->GenerateCanonMuzzleFlashEffect(L"CanonMuzzleFlash");
				m_EffectGenerator->GenerateExplosiveEffect(L"Explosive");
				m_EffectGenerator->GenerateLargeExplosiveEffect(L"LargeExplosive01");
				m_EffectGenerator->GenerateLargeExplosiveEffect(L"LargeExplosive02");
				m_EffectGenerator->GenerateScoutEffect(L"Scout");
				m_EffectGenerator->GenerateScoutedEffect(L"Scouted");
				m_EffectGenerator->GenerateCircleEffect(L"Circle00");
				m_EffectGenerator->GenerateCircleEffect(L"Circle01");
				m_EffectGenerator->GenerateCircleEffect(L"Circle02");
				m_EffectGenerator->GenerateCircleEffect(L"Circle03");
				m_EffectGenerator->GenerateCircleEffect(L"Circle04");
				

				//戦場のマスを生成
				for (int Y = 0; Y < 15; Y++)
				{
					for (int X = 0; X < 10; X++)
					{
						GameObject* spriteObj;
						SpriteCharacter* spriteCharacter = new SpriteCharacter();
						Squares* squares = new Squares;
						spriteObj = new GameObject(spriteCharacter);

						spriteCharacter->setTextureId(L"TundraTexture");

						spriteCharacter->AddCameraLabel(L"AttackerCamera");
						spriteCharacter->AddCameraLabel(L"DefenderCamera");

						spriteCharacter->setColor(1, 1, 1, 1);
						spriteCharacter->setPosition(SpposX, 1.0f, SpposY);
						spriteCharacter->setRotation(90.0f, 0.0f, 0.0f);

						squares->charaPosX = X;
						squares->charaPosY = Y;

						squares->SetSquareID(X + (Y * 10));

						if (Y == 14)
						{
							squares->target = true;
							spriteCharacter->setColor(1.0f, 0.3f, 0.3f, 1);
						}

						squares->SqPos = XMFLOAT3(SpposX, 0, SpposY);

						spriteCharacter->SetGraphicsPipeLine(L"AlphaSprite");

						XMFLOAT4 uvPattern = { 0.0f, 0.0f, 1.0f, 1.0f };

						spriteCharacter->SetSpritePattern(0, 10.0f, 10.0f, uvPattern);
						spriteCharacter->setSpriteIndex(0);

						spriteObj->addComponent(squares);

						engine->AddGameObject(spriteObj);

						BFMng->AddFieldSquare(squares);

						SpposX += 5.0f;
					}
					SpposX = -25.0f;
					SpposY += 5.0f;
				}

				BFMng->SetAlliesCharacterList(ally);
				BFMng->SetEnemyCharacterList(enemy);

				for (int i = 0; i < 5; i++)
				{
					SetCharaToSquares(BFMng->GetAlliesCharacterList()[i], BFMng->GetFieldSquaresList()[i * 2]);
					switch (kPlayStates)
					{
					case PlayStates::Debug:
						SetCharaToSquares(BFMng->GetEnemyCharacterList()[i], BFMng->GetFieldSquaresList()[i * 2 + 50]);
						break;
					case PlayStates::Release:
						SetCharaToSquares(BFMng->GetEnemyCharacterList()[i], BFMng->GetFieldSquaresList()[i * 2 + 140]);
						break;
					}
				}
				
				BFMng->SetTerrainData();

				for (int Y = 0; Y < 15; Y++)
				{
					for (int X = 0; X < 10; X++)
					{
						if (BFMng->GetFieldSquaresList()[(X + (Y * 10))]->terrainname == Terrain::Forest)
						{
							//Forest用のFBX表示
							GameObject* ForestObj;
							FBXCharacterData* ForestFbx = new FBXCharacterData();
							ForestTerrain* ForestTer = new ForestTerrain();
							ForestObj = new GameObject(ForestFbx);
							XMFLOAT3 pos = BFMng->GetCharaPos(BFMng->GetFieldSquaresList()[(X + (Y * 10))]);
							ForestFbx->setPosition(pos.x, pos.y, pos.z);
							ForestFbx->setRotation(0.0f, 0.0f, 0.0f);
							ForestObj->addComponent(ForestTer);
							engine->AddGameObject(ForestObj);
						}

						else if (BFMng->GetFieldSquaresList()[(X + (Y * 10))]->terrainname == Terrain::Hills)
						{
							//Hill用のFBX表示
							GameObject* HillObj;
							FBXCharacterData* HillFbx = new FBXCharacterData();
							HillTerrain* HillTer = new HillTerrain();
							HillObj = new GameObject(HillFbx);
							XMFLOAT3 pos = BFMng->GetCharaPos(BFMng->GetFieldSquaresList()[(X + (Y * 10))]);
							HillFbx->setPosition(pos.x, pos.y, pos.z);
							HillFbx->setRotation(0.0f, 0.0f, 0.0f);
							HillObj->addComponent(HillTer);
							engine->AddGameObject(HillObj);
						}

						else if (BFMng->GetFieldSquaresList()[(X + (Y * 10))]->terrainname == Terrain::River)
						{
							//Hill用のFBX表示
							GameObject* RiverObj;
							FBXCharacterData* RiverFbx = new FBXCharacterData();
							RiverTerrain* RiverTer = new RiverTerrain();
							RiverObj = new GameObject(RiverFbx);
							XMFLOAT3 pos = BFMng->GetCharaPos(BFMng->GetFieldSquaresList()[(X + (Y * 10))]);
							RiverFbx->setPosition(pos.x, pos.y, pos.z);
							RiverFbx->setRotation(0.0f, 0.0f, 0.0f);
							RiverObj->addComponent(RiverTer);
							engine->AddGameObject(RiverObj);
						}

						else if (BFMng->GetFieldSquaresList()[(X + (Y * 10))]->terrainname == Terrain::Tower)
						{
							//Tower用のFBX表示
							GameObject* TowerObj;
							FBXCharacterData* TowerFbx = new FBXCharacterData();
							TowerTerrain* TowerTer = new TowerTerrain();
							TowerObj = new GameObject(TowerFbx);
							XMFLOAT3 pos = BFMng->GetCharaPos(BFMng->GetFieldSquaresList()[(X + (Y * 10))]);
							TowerFbx->setPosition(pos.x, pos.y, pos.z);
							TowerFbx->setRotation(0.0f, 0.0f, 0.0f);
							TowerObj->addComponent(TowerTer);
							engine->AddGameObject(TowerObj);
						}
					}
				}

				//====================味方キャラクターの3Dモデルの生成====================
				for (int i = 0; i < BFMng->GetAlliesCharacterList().size(); i++)
				{
					auto characterData = BFMng->GetAlliesCharacterList()[i];
					GameObject* rebelInfObj;
					FBXCharacterData* rebelInfFbx = new FBXCharacterData(); //FBX用CharacterData
					PlayerBase* rebelPlayer = nullptr;
					switch (BFMng->GetAlliesCharacterList()[i]->CharaKind)
					{
					default:
						//rebelPlayer = new UnityChanPlayer();
						break;
					case SoldiersType::infantry:
						rebelPlayer = new InfantryPlayer();
						break;
					case SoldiersType::artillery:
						rebelPlayer = new ArtilleryPlayer();
						break;
					case SoldiersType::machinegunner:
						rebelPlayer = new MachinegunnerPlayer();
						break;
					case SoldiersType::scout:
						rebelPlayer = new ScoutPlayer();
						break;
					case SoldiersType::armored:
						rebelPlayer = new ArmoredPlayer();
						break;
					}

					rebelPlayer->SetAdmin(Admin::Rebel);
					characterData->SetPlayerBase(rebelPlayer);

					rebelInfObj = new GameObject(rebelInfFbx); //FBXCharacterDataを持たせて初期化

					XMFLOAT3 pos = BFMng->GetCharaPos(BFMng->GetFieldSquaresList()[BFMng->GetAlliesCharacterList()[i]->CharaPos]);
					rebelInfFbx->setPosition(pos.x, pos.y ,pos.z);
					rebelInfFbx->setRotation(0.0f, 0.0f, 0.0f);
					BFMng->GetFieldSquaresList()[BFMng->GetAlliesCharacterList()[i]->CharaPos]->fbxD = rebelInfFbx;
					BFMng->GetFieldSquaresList()[BFMng->GetAlliesCharacterList()[i]->CharaPos]->fbxD->playerData = rebelPlayer;

					BFMng->GetAlliesCharacterList()[i]->CharaObj = rebelInfObj; //GameObjectポインタをキャラクターに登録

					rebelInfObj->addComponent(rebelPlayer); //Unityちゃん本体コンポーネントをセット
					engine->AddGameObject(rebelInfObj); //GameObjectをエンジンに登録

					GenerateEquipment(characterData->CharaKind, rebelPlayer, Admin::Rebel);
				}

				//====================敵キャラクターの3Dモデルの生成====================
				for (int i = 0; i < BFMng->GetEnemyCharacterList().size(); i++)
				{
					auto characterData = BFMng->GetEnemyCharacterList()[i];
					GameObject* imperialInfObj;
					FBXCharacterData* imperialInfFbx = new FBXCharacterData(); //FBX用CharacterData
					PlayerBase* imperialPlayer = nullptr;

					switch (BFMng->GetEnemyCharacterList()[i]->CharaKind)
					{
					default:
						//imperialPlayer = new UnityChanPlayer();
						break;
					case SoldiersType::infantry:
						imperialPlayer = new InfantryPlayer();
						break;
					case SoldiersType::artillery:
						imperialPlayer = new ArtilleryPlayer();
						break;
					case SoldiersType::machinegunner:
						imperialPlayer = new MachinegunnerPlayer();
						break;
					case SoldiersType::scout:
						imperialPlayer = new ScoutPlayer();
						break;
					case SoldiersType::armored:
						imperialPlayer = new ArmoredPlayer();
						break;
					}

					imperialPlayer->SetAdmin(Admin::Imperial);
					characterData->SetPlayerBase(imperialPlayer);

					imperialInfObj = new GameObject(imperialInfFbx); //FBXCharacterDataを持たせて初期化

					XMFLOAT3 pos = BFMng->GetCharaPos(BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[i]->CharaPos]);
					imperialInfFbx->setPosition(pos.x, pos.y, pos.z);
					imperialInfFbx->setRotation(0.0f, 180.0f, 0.0f);
					BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[i]->CharaPos]->fbxD = imperialInfFbx;
					BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[i]->CharaPos]->fbxD->playerData = imperialPlayer;

					BFMng->GetEnemyCharacterList()[i]->CharaObj = imperialInfObj; //GameObjectポインタをキャラクターに登録

					imperialInfObj->addComponent(imperialPlayer); //Unityちゃん本体コンポーネントをセット
					engine->AddGameObject(imperialInfObj); //GameObjectをエンジンに登録

					GenerateEquipment(characterData->CharaKind, imperialPlayer, Admin::Imperial);
				}

#pragma region AIManager
				//AIマネージャ登録
				CharacterData* AIMngData = new CharacterData();
				GameObject* AIMngObj = new GameObject(AIMngData);

				AIMngObj->addComponent(AIMng);

				engine->AddGameObject(AIMngObj);
#pragma endregion

#pragma region Camera
				//カメラ
				GameObject* cameraObj;
				cameraObj = new GameObject(new CharacterData());
				CameraComponent* camComp = new CameraComponent();

				m_CameraComponents[L"MainCamera"] = camComp;
				cameraObj->addComponent(camComp);
				engine->AddGameObject(cameraObj);
				engine->SetCameraData(cameraObj->getCharacterData());
				camComp->changeCameraRatio(engine->GetWidth(), engine->GetHeight());
				camComp->SetViewport(0.0f, 0.0f, (float)engine->GetWidth(), (float)engine->GetHeight());
				camComp->changeCameraDepth(0.01f, 1000.0f);
				camComp->changeCameraFOVRadian(DirectX::XMConvertToRadians(45.0f));
				camComp->changeCameraPosition(20.0f, 20.0f, 20.0f);

				FlyingCameraController* flyCam = new FlyingCameraController();
				cameraObj->addComponent(flyCam);

				MyAccessHub::SetFlyingCameraComtroller(flyCam);

				CameraChangerComponent* camChanger = new CameraChangerComponent();
				cameraObj->addComponent(camChanger);
				camChanger->SetCameraController(flyCam);
				//camChanger->SetCameraController(tpCam);
				camChanger->ChangeCameraController(0);
#pragma endregion

#pragma region BattleCamera
				GameObject* attackerCameraObject = new GameObject(new CharacterData());
				BattleCameraController* attackerCameraContoroller = new BattleCameraController();
				attackerCameraContoroller->SetCameraState(BattleCameraType::AttackerCamera);
				m_CameraComponents[L"AttackerCamera"] = attackerCameraContoroller;
				attackerCameraObject->addComponent(attackerCameraContoroller);
				engine->AddGameObject(attackerCameraObject);

				GameObject* defenderCameraObject = new GameObject(new CharacterData());
				BattleCameraController* defenderCameraContoroller = new BattleCameraController();
				defenderCameraContoroller->SetCameraState(BattleCameraType::DefenderCamera);
				m_CameraComponents[L"DefenderCamera"] = defenderCameraContoroller;
				defenderCameraObject->addComponent(defenderCameraContoroller);
				engine->AddGameObject(defenderCameraObject);

				GameObject* scoutingCameraObject = new GameObject(new CharacterData());
				BattleCameraController* scoutingCameraContoroller = new BattleCameraController();
				scoutingCameraContoroller->SetCameraState(BattleCameraType::ScoutingCamera);
				m_CameraComponents[L"ScoutingCamera"] = scoutingCameraContoroller;
				scoutingCameraObject->addComponent(scoutingCameraContoroller);
				engine->AddGameObject(scoutingCameraObject);

				SetActiveCameraCompornent(L"ScoutingCamera", false);

				camChanger->ChangeCamera();

				BFMng->SetCameraChangerCompornent(camChanger);
#pragma endregion

#pragma region UI
				cameraObj = new GameObject(new CharacterData());
				MenuText* m_UIRender = new MenuText();
				MenuUI* m_MoniterRender = new MenuUI();
				MonitorSelectUI* m_MoniSelRender = new MonitorSelectUI();
				TurnUI* m_TurnUI = new TurnUI();
				SoldierHUD* m_SoldierHUD = new SoldierHUD();
				MovedCountHUD* m_MovedCountHUD = new MovedCountHUD();
				LogHUD* m_LogHUD = new LogHUD();
				LogHUDW* m_LogHUDW = new LogHUDW();
				DamageHUD* m_DamageHUD = new DamageHUD();
				DamageHUDW* m_DamageHUDW = new DamageHUDW();
				OpeningAnimHUD* m_OpeningAnimHUD = new OpeningAnimHUD();
				TurnEndUI* m_TurnEndUI = new TurnEndUI();
				TurnEndW* m_TurnEndW = new TurnEndW();
				ResultUI* m_ResultUI = new ResultUI();

				m_CameraComponents[L"HUDCamera"] = m_UIRender;

				cameraObj->addComponent(m_UIRender);
				cameraObj->addComponent(m_MoniterRender);
				cameraObj->addComponent(m_MoniSelRender);
				cameraObj->addComponent(m_TurnUI);
				cameraObj->addComponent(m_SoldierHUD);
				cameraObj->addComponent(m_MovedCountHUD);
				cameraObj->addComponent(m_LogHUD);
				cameraObj->addComponent(m_LogHUDW);
				cameraObj->addComponent(m_DamageHUD);
				cameraObj->addComponent(m_DamageHUDW);
				cameraObj->addComponent(m_OpeningAnimHUD);
				cameraObj->addComponent(m_TurnEndUI);
				cameraObj->addComponent(m_TurnEndW);
				cameraObj->addComponent(m_ResultUI);

				//==================HUDマネージャ追加==================
				cameraObj->addComponent(m_HUDManager);

				BFMng->SetMenuText(m_UIRender);
				BFMng->SetMenuUI(m_MoniterRender);
				BFMng->SetMenuSelectUI(m_MoniSelRender);
				BFMng->SetLogHUD(m_LogHUD);
				BFMng->SetLogHUDText(m_LogHUDW);
				BFMng->SetDamageHUD(m_DamageHUD);
				BFMng->SetDamageHUDText(m_DamageHUDW);
				BFMng->SetOpeningAnimHUD(m_OpeningAnimHUD);
				BFMng->SetResultUI(m_ResultUI);
				BFMng->SetTurnEndUI(m_TurnEndUI);
				BFMng->SetMovedCountHUD(m_MovedCountHUD);
				BFMng->SetTurnUI(m_TurnUI);
				BFMng->SetAbiliteis(new Abilities());

				engine->AddGameObject(cameraObj);

#pragma endregion

#pragma region DEBUG

				//GameObject* debugObject1;
				//FBXCharacterData* debugFbx1 = new FBXCharacterData(); //FBX用CharacterData
				//PlayerBase* debugPlayer1 = new ArtilleryPlayer();
				//debugObject1 = new GameObject(debugFbx1); //FBXCharacterDataを持たせて初期化
				//debugPlayer1->SetAdmin(Admin::Rebel);
				//debugFbx1->setPosition(20.0f, 5.0f, -50.0f);
				//debugFbx1->AddCameraLabel(L"AttackerCamera");
				//debugFbx1->AddCameraLabel(L"DefenderCamera");
				//debugObject1->addComponent(debugPlayer1);
				//engine->AddGameObject(debugObject1);

				//GameObject* debugObject2;
				//FBXCharacterData* debugFbx2 = new FBXCharacterData(); //FBX用CharacterData
				//PlayerBase* debugPlayer2 = new ArtilleryPlayer();
				//debugObject2 = new GameObject(debugFbx2); //FBXCharacterDataを持たせて初期化
				//debugPlayer2->SetAdmin(Admin::Imperial);
				//debugFbx2->setPosition(20.0f, 5.0f, 50.0f);
				//debugFbx2->AddCameraLabel(L"AttackerCamera");
				//debugFbx2->AddCameraLabel(L"DefenderCamera");
				//debugObject2->addComponent(debugPlayer2);
				//engine->AddGameObject(debugObject2);

#pragma endregion
				engine->UploadCreatedTextures();
		}
			break;

		case static_cast<UINT>(GAME_SCENES::GAME_OVER):
			break;

		default:
			return E_FAIL;
		}

		m_scene = scene;

		engine->WaitForGpu();	//GPU待機
	}

	return hr;
}

void SceneManager::RemoveCamera(GameComponent* gc)
{
	std::wstring label = L"";
	for (auto cameraPair : m_CameraComponents)
	{
		if (cameraPair.second == gc)
		{
			label = cameraPair.first;
			break;
		}
	}
	if (label != L"")
	{
		RemoveCamera(label);
	}
}

void SceneManager::RemoveCamera(std::wstring label)
{
	if (m_CameraComponents[label] != nullptr)
	{
		m_CameraComponents.erase(label);
	}
}
void SceneManager::AddSceneObject(GameObject* obj)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();

	obj->SetGameObjectDeleter(m_pObjDeleter.get());

	m_SceneObjects.push_back(unique_ptr<GameObject>(obj));
	engine->AddGameObject(obj);
}
void SceneManager::DeleteSceneObject(GameObject* obj)
{
	auto ite = m_SceneObjects.begin();

	//objを持っている場所を探す
	for (; ite != m_SceneObjects.end(); ite++)
	{
		if (ite->get() == obj)
		{
			break;
		}
	}

	//removeがあるのでループの外で処理
	if (ite != m_SceneObjects.end())
	{
		ite->release();
		m_SceneObjects.remove(*ite);
	}
}

void SceneManager::GenerateEquipment(SoldiersType type, PlayerBase* playerBase, Admin admin)
{
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();
	switch (admin)
	{
	case Admin::Rebel:
		switch (type)
		{
		case SoldiersType::infantry:
		{
			//ARを装備させる
			FBXCharacterData* rebelAssaltRifle = new FBXCharacterData();
			if (FAILED(rebelAssaltRifle->LoadMainFBX(L"./Resources/fbx/RebelAssaltRifle.fbx", L"RebelAssaltRifle")))
				return;
			GameObject* assaltRifleObj = new GameObject(rebelAssaltRifle);
			RebelAssaltRifle* assaltRifle = new RebelAssaltRifle();
			assaltRifleObj->addComponent(assaltRifle);
			engine->AddGameObject(assaltRifleObj);

			playerBase->SetHandEquipment(assaltRifle);
			break;
		}
		case SoldiersType::machinegunner:
		{
			//LMGを装備させる
			FBXCharacterData* rebelMachinegun = new FBXCharacterData();
			if (FAILED(rebelMachinegun->LoadMainFBX(L"./Resources/fbx/RebelMachinegun.fbx", L"RebelMachinegun")))
				return;
			GameObject* machinegunObj = new GameObject(rebelMachinegun);
			RebelMachinegun* machinegun = new RebelMachinegun();
			machinegunObj->addComponent(machinegun);
			engine->AddGameObject(machinegunObj);

			playerBase->SetHandEquipment(machinegun);

			//バックパックを装備させる
			FBXCharacterData* rebelBackpack = new FBXCharacterData();
			if (FAILED(rebelBackpack->LoadMainFBX(L"./Resources/fbx/backpackObject.fbx", L"backpackObject")))
				return;
			GameObject* backpackObj = new GameObject(rebelBackpack);
			Backpack* backpack = new Backpack();
			backpackObj->addComponent(backpack);
			engine->AddGameObject(backpackObj);

			playerBase->SetBackEquipment(backpack);
			break;
		}
		case SoldiersType::scout:
		{
			//SRを装備させる
			FBXCharacterData* rebelScoutRifle = new FBXCharacterData();
			if (FAILED(rebelScoutRifle->LoadMainFBX(L"./Resources/fbx/RebelScoutRifle.fbx", L"RebelScoutRifle")))
				return;
			GameObject* scoutRifleObj = new GameObject(rebelScoutRifle);
			RebelScoutRifle* scoutRifle = new RebelScoutRifle();
			scoutRifleObj->addComponent(scoutRifle);
			engine->AddGameObject(scoutRifleObj);

			playerBase->SetHandEquipment(scoutRifle);
			break;
		}
		default:
			break;
		}
		break;
	case Admin::Imperial:
		switch (type)
		{
		case SoldiersType::infantry:
		{
			FBXCharacterData* imperialAssaltRifle = new FBXCharacterData();
			if (FAILED(imperialAssaltRifle->LoadMainFBX(L"./Resources/fbx/ImperialAssaltRifle.fbx", L"ImperialAssaltRifle")))
				return;
			GameObject* assaltRifleObj = new GameObject(imperialAssaltRifle);
			ImperialAssaltRifle* assaltRifle = new ImperialAssaltRifle();
			assaltRifleObj->addComponent(assaltRifle);
			engine->AddGameObject(assaltRifleObj);

			playerBase->SetHandEquipment(assaltRifle);
			break;
		}
		case SoldiersType::scout:
		{
			//SRを装備させる
			FBXCharacterData* imperialScoutRifle = new FBXCharacterData();
			if (FAILED(imperialScoutRifle->LoadMainFBX(L"./Resources/fbx/ImperialScoutRifle.fbx", L"ImperialScoutRifle")))
				return;
			GameObject* scoutRifleObj = new GameObject(imperialScoutRifle);
			ImperialScoutRifle* scoutRifle = new ImperialScoutRifle();
			scoutRifleObj->addComponent(scoutRifle);
			engine->AddGameObject(scoutRifleObj);

			playerBase->SetHandEquipment(scoutRifle);
			break;
		}
		default:
			break;
		}
		break;
	}
}

void SceneObjectDeleter::ExecuteDeleter(GameObject* go)
{
	myScene->DeleteSceneObject(go);
}
