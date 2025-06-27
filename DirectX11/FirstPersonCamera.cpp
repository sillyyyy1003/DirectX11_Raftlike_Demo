#include "FirstPersonCamera.h"
#include <imgui.h>
#include "D3DApp.h"
#include "KInput.h"

using namespace DirectX;


FirstPersonCamera::FirstPersonCamera():
m_moveSpeed(5.f),
m_state(0),
m_oldPos(0,0),
m_defaultPosition(0, 0, 0),
m_windowSize(WIN_WIDTH,WIN_HEIGHT)
{
}

void FirstPersonCamera::Init()
{

}

void FirstPersonCamera::Update(float dt)
{
    UpdateState();

#if defined(_DEBUG) || defined(DEBUG)
    if (ImGui::Begin("Camera Option"))
    {
        ImGui::Text("Position");
        ImGui::Text("Float3 : x: %2f  y: %2f  z: %2f", this->GetPos().x, this->GetPos().y, this->GetPos().z);

        ImGui::Text("Rotation");
        DirectX::XMFLOAT3 rot = { XMConvertToDegrees(m_transform.GetRotation().x),XMConvertToDegrees(m_transform.GetRotation().y),XMConvertToDegrees(m_transform.GetRotation().z) };
        ImGui::Text("Float3 : x: %2f  y: %2f  z: %2f", rot.x, rot.y, rot.z);

        ImGui::InputFloat("MoveSpeed", &m_moveSpeed);
    }
    ImGui::End();
#endif

    if (m_state == CAM_NONE) return;

    switch (m_state)
    {
    case CAM_FREE:
        // マウス移動量
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        XMFLOAT2 mouseMove = DirectX::XMFLOAT2((float)cursorPos.x - m_oldPos.x, (float)cursorPos.y - m_oldPos.y);
        m_oldPos = cursorPos;
        UpdateFlight(mouseMove, dt);
        break;
    default:;
    }
}

void FirstPersonCamera::SetPosition(float x, float y, float z)
{
    CameraBase::SetPos(XMFLOAT3(x, y, z));

}

void FirstPersonCamera::SetPosition(const XMFLOAT3& pos)
{
    CameraBase::SetPos(pos);
}

void FirstPersonCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
    m_transform.SetPosition(pos);
    m_transform.LookAt(target, up);

}

void FirstPersonCamera::LookAt(const DirectX::XMFLOAT3& target)
{
    DirectX::XMFLOAT3 up = { 0,1,0 };
    m_transform.LookAt(target, up);

}

void FirstPersonCamera::LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up)
{
    m_transform.SetPosition(pos);
    m_transform.LookTo(to, up);

}

void FirstPersonCamera::Strafe(float d)
{
    m_transform.Translate(m_transform.GetRightAxis(), d);
}

void FirstPersonCamera::Walk(float d)
{
    XMFLOAT3 rightAxis = m_transform.GetRightAxis();
    XMVECTOR rightVec = XMLoadFloat3(&rightAxis);
    XMVECTOR frontVec = XMVector3Normalize(XMVector3Cross(rightVec, g_XMIdentityR1));
    XMFLOAT3 front;
    XMStoreFloat3(&front, frontVec);
    m_transform.Translate(front, d);
}

void FirstPersonCamera::MoveForward(float d)
{
    m_transform.Translate(m_transform.GetForwardAxis(), d);
}

void FirstPersonCamera::Pitch(float rad)
{
    XMFLOAT3 rotation = m_transform.GetRotation();
    // 将绕x轴旋转弧度限制在[-7pi/18, 7pi/18]之间
    rotation.x += rad;
    if (rotation.x > XM_PI * 7 / 18)
        rotation.x = XM_PI * 7 / 18;
    else if (rotation.x < -XM_PI * 7 / 18)
        rotation.x = -XM_PI * 7 / 18;

    m_transform.SetRotation(rotation);
}

void FirstPersonCamera::RotateY(float rad)
{
    XMFLOAT3 rotation = m_transform.GetRotation();
    rotation.y = XMScalarModAngle(rotation.y + rad);
    m_transform.SetRotation(rotation);
}

void FirstPersonCamera::MoveUpward(float d)
{
    m_transform.Translate(m_transform.GetUpAxis(), d);
}


void FirstPersonCamera::Shake(float dt)
{
  

}

void FirstPersonCamera::SetCameraState(FirstPersonCamera::CameraKind state)
{
    m_state = state;
}



void FirstPersonCamera::SetTarget(const DirectX::XMFLOAT3& target)
{
    m_transform.LookAt(target);
}

void FirstPersonCamera::UpdateState()
{
    // 操作の場合
    CameraKind prev = (CameraKind)m_state;

	if (KInput::IsKeyPress(VK_RBUTTON))
    {
        m_state = CAM_FREE;
    }
    else
    {
        m_state = CAM_NONE;
    }

    if (prev != m_state)GetCursorPos(&m_oldPos);

}

void FirstPersonCamera::UpdateFlight(DirectX::XMFLOAT2 mouseMove, float dt)
{

    //横回転
	float angleX = 360.0f * mouseMove.x / m_windowSize.x;
	RotateY(angleX * dt * m_moveSpeed);
	//縦回転
	float angleY = 180.0f * mouseMove.y / m_windowSize.y;
    Pitch(angleY * dt * m_moveSpeed);


     // キー入力で移動
    if (KInput::IsKeyPress('W'))//Move Forward
        Walk(dt * m_moveSpeed);
    if (KInput::IsKeyPress('S'))//Move Backward
        Walk(dt * -m_moveSpeed);
    if (KInput::IsKeyPress('A'))//Move Left
        Strafe(dt * -m_moveSpeed);
    if (KInput::IsKeyPress('D'))//Move Right
        Strafe(dt * m_moveSpeed);
    if (KInput::IsKeyPress('Q'))
        MoveUpward(dt * m_moveSpeed);
    if (KInput::IsKeyPress('E'))
        MoveUpward(dt * -m_moveSpeed);

}


