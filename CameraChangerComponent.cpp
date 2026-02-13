#include "SceneManager.h"
#include "CameraChangerComponent.h"

void CameraChangerComponent::initAction()
{
	SceneManager* scene = dynamic_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	m_keyBind = dynamic_cast<KeyBindComponent*>(scene->getKeyComponent());
	m_currentCamera = -1;
}

bool CameraChangerComponent::frameAction()
{
	if (m_keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_DOWN, KeyBindComponent::BUTTON_IDS::MOUSE_P))
	{
		ChangeCamera();
	}
	return true;
}

void CameraChangerComponent::finishAction()
{
	m_cameraComponents.clear();
}

void CameraChangerComponent::SetCameraController(GameComponent* camCon)
{
	m_cameraComponents.push_back(camCon);
	camCon->setActive(false);
}

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

void CameraChangerComponent::ChangeCamera()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	if (p_scene->getCameraComponent(L"AttackerCamera") != nullptr)
	{
		SetMainCamera();
	}
	else
	{
		SetBattleCamera();
	}
}

void CameraChangerComponent::SetBattleCamera()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	p_scene->SetActiveCameraCompornent(L"AttackerCamera", true);
	p_scene->SetActiveCameraCompornent(L"DefenderCamera", true);
	p_scene->SetActiveCameraCompornent(L"MainCamera", false);
	p_scene->SetActiveCameraCompornent(L"ScoutingCamera", false);
	BFMng->SetBattleCameraEnable(true);
}

void CameraChangerComponent::SetMainCamera()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	p_scene->SetActiveCameraCompornent(L"AttackerCamera", false);
	p_scene->SetActiveCameraCompornent(L"DefenderCamera", false);
	p_scene->SetActiveCameraCompornent(L"MainCamera", true);
	//p_scene->SetActiveCameraCompornent(L"ScoutingCamera", true);
	BFMng->SetBattleCameraEnable(false);
}


