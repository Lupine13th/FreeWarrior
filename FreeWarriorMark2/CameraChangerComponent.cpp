#include "SceneManager.h"
#include "CameraChangerComponent.h"

void CameraChangerComponent::InitAction()
{
	SceneManager* scene = dynamic_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	m_keyBind = dynamic_cast<KeyBindComponent*>(scene->getKeyComponent());
	m_currentCamera = -1;
}

bool CameraChangerComponent::FrameAction()
{
	//if (m_keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::key_P))	//デバッグ用　Pキーでカメラを手動切り替え
	//{
	//	ChangeCamera();
	//}
	return true;
}

void CameraChangerComponent::FinishAction()
{
	m_cameraComponents.clear();
}

//メインカメラ追加(メインカメラは現状一つしかないから、初期化用)
void CameraChangerComponent::SetCameraController(GameComponent* camCon)
{
	m_cameraComponents.push_back(camCon);
	camCon->setActive(false);
}

//メインカメラ切り替え(メインカメラは現状一つしかないから、初期化用)
void CameraChangerComponent::ChangeCameraController(int index)	
{
	if (index >= m_cameraComponents.size()) return;

	GameComponent* cam = m_cameraComponents[index];
	if (cam != nullptr)
	{
		//新しいCameraをONにして現在のCameraをOFF
		if (m_currentCamera > -1)
			m_cameraComponents[m_currentCamera]->setActive(false);

		m_currentCamera = index;
		cam->setActive(true);
	}
}

//メイン・バトルカメラ切り替え
void CameraChangerComponent::ChangeCamera()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	if (p_scene->getCameraComponent(L"AttackerCamera") != nullptr)
	{
		SetMainCamera();
	}
	else
	{
		SetBattleCamera();
	}
}

//バトルカメラ起動・メインカメラ削除
void CameraChangerComponent::SetBattleCamera()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	p_scene->SetActiveCameraCompornent(L"AttackerCamera", true);
	p_scene->SetActiveCameraCompornent(L"DefenderCamera", true);
	p_scene->SetActiveCameraCompornent(L"MainCamera", false);
	p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
	BFMng->SetBattleCameraEnable(true);
}

//メインカメラ起動・バトルカメラ削除
void CameraChangerComponent::SetMainCamera()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::GetMyGameEngine()->GetSceneController());
	p_scene->SetActiveCameraCompornent(L"AttackerCamera", false);
	p_scene->SetActiveCameraCompornent(L"DefenderCamera", false);
	p_scene->SetActiveCameraCompornent(L"MainCamera", true);
	p_scene->SetActiveCameraCompornent(L"ScoutingCamera", true);
	BFMng->SetBattleCameraEnable(false);
}


