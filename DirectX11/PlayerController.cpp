#include "PlayerController.h"
#include "KInput.h"
#include "Player.h"


PlayerController::PlayerController(Player* player):
m_pPlayer(player),
m_moveSpeed(1.f),
m_oldPos(0,0),
m_windowSize(WIN_WIDTH,WIN_HEIGHT)
{
    m_centerPos = gD3D->GetWindowCenterPos(); 
}

void PlayerController::Update(float dt)
{
    if (!m_isControllable)return;
    //Cursor Move
   {
	    POINT cursorPos;
	    GetCursorPos(&cursorPos);

	    float dx = static_cast<float>(cursorPos.x - m_centerPos.x);
	    float dy = static_cast<float>(cursorPos.y - m_centerPos.y);

	    float angleX = 360.0f * dx / m_windowSize.x;
	    float angleY = 180.0f * dy / m_windowSize.y;

	    m_pPlayer->RotateY(angleX * dt * m_moveSpeed);
	    m_pPlayer->Pitch(angleY * dt * m_moveSpeed);

	    SetCursorPos(m_centerPos.x, m_centerPos.y);
    }

    // キー入力で移動
    if (KInput::IsKeyPress('W'))//Move Forward
        m_pPlayer->Walk(dt * m_moveSpeed);
    if (KInput::IsKeyPress('S'))//Move Backward
        m_pPlayer->Walk(dt * -m_moveSpeed);
    if (KInput::IsKeyPress('A'))//Move Left
        m_pPlayer->Strafe(dt * -m_moveSpeed);
    if (KInput::IsKeyPress('D'))//Move Right
        m_pPlayer->Strafe(dt * m_moveSpeed);


}

void PlayerController::UpdateWindowSize(DirectX::XMFLOAT2 windowSize)
{
    m_windowSize = windowSize;
}

void PlayerController::UpdateWindowCenter(POINT center)
{
    m_centerPos = center;
}
