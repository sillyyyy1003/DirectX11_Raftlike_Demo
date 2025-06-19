#include "CameraController.h"
#include <memory>
#include "Cube.h"
#include "FirstPersonCamera.h"
#include "KInput.h"
#include "ThirdPersonCamera.h"

CameraController::CameraController() :
	m_mode(CameraMode::Free),
	m_state(CameraKind::CamNone),
	m_oldPos(0, 0),
	m_moveSpeed(5.f),
	m_pCurrentCamera(nullptr),
	m_windowSize(WIN_WIDTH, WIN_HEIGHT)
{
	m_pFirstPersonCamera = std::make_shared<FirstPersonCamera>();
	m_pThirdPersonCamera = std::make_shared<ThirdPersonCamera>();
	m_pCurrentCamera = m_pFirstPersonCamera.get();
}

void CameraController::Update(float dt)
{
#ifdef _DEBUG

	if (KInput::IsKeyTrigger(VK_F1))
	{
		// FreeCameraに切り替え
		m_mode = CameraMode::Free;
		// 位置をリセット
		m_pFirstPersonCamera->m_transform.SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f));
		// 今使っているカメラのをFirstPersonCameraに切り替え
		m_pCurrentCamera = m_pFirstPersonCamera.get();
	}
	if (KInput::IsKeyTrigger(VK_F2))
	{
		m_mode = CameraMode::FirstPerson;
		// 今使っているカメラのをFirstPersonCameraに切り替え
		m_pCurrentCamera = m_pFirstPersonCamera.get();
	}
	/*if (KInput::IsKeyTrigger(VK_F3))
	{
	
		m_mode = CameraMode::ThirdPerson;
		// 今使っているカメラのをFirstPersonCameraに切り替え
		m_pCurrentCamera = m_pThirdPersonCamera.get();
	}
	*/

#endif
	switch (m_mode)
	{
	case CameraMode::Free: UpdateFreeCamera(dt); break;
	case CameraMode::ThirdPerson:UpdateThirdPerson(dt); break;
	case CameraMode::FirstPerson:break;
	default:return;
	}
	
}


void CameraController::UpdateCameraTransform(const Transform& transform)
{
	m_pFirstPersonCamera->SetPos(transform.GetPosition());
	m_pFirstPersonCamera->m_transform.SetRotation(transform.GetRotation());
}

void CameraController::UpdateThirdPerson(float dt)
{
	//=======カメラの状態を更新
	// 操作の場合
	CameraKind prev = (CameraKind)m_state;

	if (KInput::IsKeyPress(VK_RBUTTON))
	{
		m_state = CameraKind::CamFree;
	}
	else
	{
		m_state = CameraKind::CamNone;
	}

	if (prev != m_state)GetCursorPos(&m_oldPos);

	if (m_state == CameraKind::CamNone) return;

	//=======Cursorの位置を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	DirectX::XMFLOAT2 mouseMove = DirectX::XMFLOAT2((float)cursorPos.x - m_oldPos.x, (float)cursorPos.y - m_oldPos.y);
	m_oldPos = cursorPos;


	//=======入力応じる変更
	float angleX = 360.0f * mouseMove.x / m_windowSize.x;
	float angleY = 180.0f * mouseMove.y / m_windowSize.y;
	std::dynamic_pointer_cast<ThirdPersonCamera>(m_pThirdPersonCamera)->RotateX(angleX * dt * m_moveSpeed);
	std::dynamic_pointer_cast<ThirdPersonCamera>(m_pThirdPersonCamera)->RotateY(angleY * dt * m_moveSpeed);

	if (gD3D->GetWheelMoveUnit() != 0)
	{
		std::dynamic_pointer_cast<ThirdPersonCamera>(m_pThirdPersonCamera)->Approach(gD3D->GetWheelMoveUnit() * 0.2f);
		gD3D->SetWheelMoveUnit(0);
	}
}

void CameraController::UpdateFreeCamera(float dt)
{
	m_pFirstPersonCamera->Update(dt);
}
