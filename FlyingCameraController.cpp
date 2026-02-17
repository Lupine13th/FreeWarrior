#include <MyAccessHub.h>
#include <DirectXMath.h>
#include <chrono>

#include "CameraComponent.h"
#include "FlyingCameraController.h"
#include "BattleFieldManager.h"
#include "EnemyAIManager.h"
#include "KeyBindComponent.h"
#include "TimeManager.h"

using namespace DirectX;

using namespace std::chrono;

void FlyingCameraController::StartGame()	//ゲーム開始時のカメラアニメーション
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	BFMng->UpdateBattleField();
	BFMng->SetCurrentTurn(Turn::Allies);
	Opening = false;
}

void FlyingCameraController::initAction()
{
	auto components = getGameObject()->getComponents();

	m_camera = nullptr;
	for (auto comp : components)
	{
		m_camera = dynamic_cast<CameraComponent*>(comp);
		if (m_camera != nullptr)
		{
			break;
		}
	}

	m_camera->changeCameraPosition(0.0f, 15.0f, -10.0f);
	m_camera->changeCameraFocus(0.0f, 0.0f, 0.0f);

}

bool FlyingCameraController::frameAction()
{
	SceneManager* p_scene = static_cast<SceneManager*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	KeyBindComponent* keycomp = static_cast<KeyBindComponent*>(p_scene->getKeyComponent());
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();

	if (Opening && BFMng->GetCurrentTurn() == Turn::First)
	{
		if (OpeningCount < 2.5f)
		{
			OpeningCount += m_TimeManager->GetDeltaTime();
			m_MovingValue.z = 0.4f;
			MoveCamera();
		}
		else if (OpeningCount > 2.5f)
		{
			m_MovingValue.z = 0.0f;
			MoveCamera();
			BFMng->GetOpeningAnimHUD()->oState = OpeningState::BeforeAnim;
			Opening = false;
			OpeningCount = 0.0f;
		}
	}
	else
	{
		if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS, KeyBindComponent::BUTTON_IDS::MOVE_LEFT))
		{
			m_MovingValue.x -= 0.05f;
			MoveCamera();
		}
		else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS, KeyBindComponent::BUTTON_IDS::MOVE_RIGHT))
		{
			m_MovingValue.x += 0.05f;
			MoveCamera();
		}
		else
		{
			if (m_MovingValue.x > 0.3f)
			{
				m_MovingValue.x -= 0.1f;
				MoveCamera();
			}
			else if (m_MovingValue.x < -0.3f)
			{
				m_MovingValue.x += 0.1f;
				MoveCamera();
			}
			else
			{
				m_MovingValue.x = 0.0f;
				//MoveCamera();
			}
		}

		if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS, KeyBindComponent::BUTTON_IDS::MOVE_FORWARD))
		{
			m_MovingValue.z += 0.05f;
			MoveCamera();
		}
		else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS, KeyBindComponent::BUTTON_IDS::MOVE_BACK))
		{
			m_MovingValue.z -= 0.05f;
			MoveCamera();
		}
		else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS, KeyBindComponent::BUTTON_IDS::Key_E))
		{
			m_ZoomPercent -= kZoomSpeed;
			if (m_ZoomPercent < kMaxZoom)
			{
				m_ZoomPercent = kMaxZoom;
			}
			MoveCamera();
		}
		else if (keycomp->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS, KeyBindComponent::BUTTON_IDS::Key_Q))
		{
			m_ZoomPercent += kZoomSpeed;
			if (m_ZoomPercent > kMinZoom)
			{
				m_ZoomPercent = kMinZoom;
			}
			MoveCamera();
		}
		else
		{
			if (m_MovingValue.z > 0.3f)
			{
				m_MovingValue.z -= 0.1f;
				MoveCamera();
			}
			else if (m_MovingValue.z < -0.3f)
			{
				m_MovingValue.z += 0.1f;
				MoveCamera();
			}
			else
			{
				m_MovingValue.z = 0.0f;
				//MoveCamera();
			}
		}
	}
	

	return true;
}

void FlyingCameraController::finishAction()
{
}

void FlyingCameraController::ChangeCameraPosition()	//カメラ位置の変更
{
	BattleFieldManager* BFMng = MyAccessHub::GetBFManager();
	EnemyAIManager* AIMng = MyAccessHub::GetAIManager();

	if (BFMng->GetCurrentTurn() == Turn::Allies)
	{
		int x = 0;
		int y = 0;

		switch (BFMng->GetMode())
		{
		default:
			break;
		case  Mode::FieldMode:
			x = BFMng->GetSelectPos()[(int)Vector::X];
			y = BFMng->GetSelectPos()[(int)Vector::Y];
			break;
		case  Mode::MenuMode:
			x = BFMng->GetSelectPos()[(int)Vector::X];
			y = BFMng->GetSelectPos()[(int)Vector::Y];
			break;
		case  Mode::MoveMode:
			x = BFMng->GetTargetPos()[(int)Vector::X];
			y = BFMng->GetTargetPos()[(int)Vector::Y];
			break;
		case  Mode::AttackMode:
			x = BFMng->GetTargetPos()[(int)Vector::X];
			y = BFMng->GetTargetPos()[(int)Vector::Y];
			break;
		case  Mode::AbilityMode:
			x = BFMng->GetTargetPos()[(int)Vector::X];
			y = BFMng->GetTargetPos()[(int)Vector::Y];
			break;
		}
		int index = x + y * 10;

		XMFLOAT3 squarePos = BFMng->GetFieldSquaresList()[index]->getGameObject()->getCharacterData()->getPosition();

		// カメラをマスの上に追従
		m_ZoomPercent = kMinZoom;
		m_camera->changeCameraPosition(squarePos.x, squarePos.y + 15.0f * m_ZoomPercent, squarePos.z - 10.0f * m_ZoomPercent);
		m_camera->changeCameraFocus(squarePos.x, squarePos.y, squarePos.z);
		XMStoreFloat3(&m_MovingValue, XMVectorZero());
	}
	else if (BFMng->GetCurrentTurn() == Turn::EnemyMove)
	{
		int x = BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[AIMng->GetMoveAiCount()]->CharaPos]->charaPosX;
		int y = BFMng->GetFieldSquaresList()[BFMng->GetEnemyCharacterList()[AIMng->GetMoveAiCount()]->CharaPos]->charaPosY;
		

		if (BFMng->GetEnemyCharacterList()[AIMng->GetMoveAiCount()]->AIMove == EnemyMove::Attack && AIMng->GetDelayCount() > 1.5f)
		{
			x = BFMng->GetEnemyCharacterList()[AIMng->GetMoveAiCount()]->targetAISquare->charaPosX;
			y = BFMng->GetEnemyCharacterList()[AIMng->GetMoveAiCount()]->targetAISquare->charaPosY;
		}

		int index = x + y * 10;

		XMFLOAT3 squarePos = BFMng->GetFieldSquaresList()[index]->getGameObject()->getCharacterData()->getPosition();

		// カメラをマスの上に追従
		m_ZoomPercent = kMinZoom;
		m_camera->changeCameraPosition(squarePos.x, squarePos.y + 15.0f * m_ZoomPercent, squarePos.z - 10.0f * m_ZoomPercent);
		m_camera->changeCameraFocus(squarePos.x, squarePos.y, squarePos.z);
	}
}

void FlyingCameraController::MoveCamera()	//WASDでのカメラ移動
{
	XMFLOAT3 cameraFocusPosition = m_camera->getCameraFocus();

	if (m_MovingValue.x > kMaxMovingValue)
	{
		m_MovingValue.x = kMaxMovingValue;
	}
	if (m_MovingValue.x < -kMaxMovingValue)
	{
		m_MovingValue.x = -kMaxMovingValue;
	}
	if (m_MovingValue.z > kMaxMovingValue)
	{
		m_MovingValue.z = kMaxMovingValue;
	}
	if (m_MovingValue.z < -kMaxMovingValue)
	{
		m_MovingValue.z = -kMaxMovingValue;
	}

	m_camera->changeCameraPosition(cameraFocusPosition.x + m_MovingValue.x, cameraFocusPosition.y + 15.0f * m_ZoomPercent, cameraFocusPosition.z - 10.0f * m_ZoomPercent + m_MovingValue.z);
	m_camera->changeCameraFocus(cameraFocusPosition.x + m_MovingValue.x, cameraFocusPosition.y, cameraFocusPosition.z + m_MovingValue.z);
} 

void FlyingCameraController::SetBattleCam(Squares* attaking, Squares* attacked)	//バトルカメラ切り替え
{
	m_AttackingCharacterPos = attaking->SqPos;
	m_AttackedCharacterPos = attacked->SqPos;
	
	XMFLOAT3 midpos = {		//中点の座標
		(m_AttackingCharacterPos.x + m_AttackedCharacterPos.x) / 2.0f,
		(m_AttackingCharacterPos.y + m_AttackedCharacterPos.y) / 2.0f,
		(m_AttackingCharacterPos.z + m_AttackedCharacterPos.z) / 2.0f
	};	

	XMFLOAT3 dir = {		//二点のベクトル
		m_AttackingCharacterPos.x - m_AttackedCharacterPos.x,
		m_AttackingCharacterPos.y - m_AttackedCharacterPos.y,
		m_AttackingCharacterPos.z - m_AttackedCharacterPos.z
	};	

	XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };	//上方向のベクトル

	//XZ平面の垂直ベクトルを計算
	XMVECTOR vDir = XMLoadFloat3(&dir);
	XMVECTOR vUp = XMLoadFloat3(&up);
	XMVECTOR vPerp = XMVector3Cross(vDir, vUp);
	vPerp = XMVector3Normalize(vPerp);

	XMFLOAT3 perp;
	XMStoreFloat3(&perp, vPerp);

	XMFLOAT3 cameraPos = {
		midpos.x + perp.x * 15.0f,
		midpos.y + 10.0f,
		midpos.z + perp.z * 15.0f
	};
	
	m_camera->changeCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	SetCameraFocus(m_AttackingCharacterPos);
}

void FlyingCameraController::SetCameraFocus(XMFLOAT3 focusPos)
{
	m_camera->changeCameraFocus(focusPos.x, focusPos.y, focusPos.z);
}
