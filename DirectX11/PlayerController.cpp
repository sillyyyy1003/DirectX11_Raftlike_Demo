#include "PlayerController.h"
#include "KInput.h"
#include "Player.h"


PlayerController::PlayerController(Player* player, PlayerCharacter* playerCharacter):
    m_pPlayer(player),
    m_oldPos(0, 0),
    m_windowSize(WIN_WIDTH, WIN_HEIGHT),
	m_pPlayerCharacter(playerCharacter)
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

        m_pPlayer->RotateY(angleX * dt);       //プレイヤーのY回転計算
        m_pPlayer->Pitch(angleY * dt);         //カメラの視角回転計算
        m_pPlayerCharacter->SetRotation({0,m_pPlayer->GetTransform().GetRotation().y,0});//Colliderの回転はY軸だけ

        SetCursorPos(m_centerPos.x, m_centerPos.y);
    }

    // キー入力で移動
    //if (KInput::IsKeyPress('W'))//Move Forward
    //    m_pPlayer->Walk(dt);
    //if (KInput::IsKeyPress('S'))//Move Backward
    //    m_pPlayer->Walk(-dt);
    //if (KInput::IsKeyPress('A'))//Move Left
    //    m_pPlayer->Strafe(-dt );
    //if (KInput::IsKeyPress('D'))//Move Right
    //    m_pPlayer->Strafe(dt);
    //if (KInput::IsKeyPress(VK_SPACE))   //Jump
    //    m_pPlayer->Jump();

    Vec3 dir = { 0,0,0 };
    if (KInput::IsKeyPress('W'))dir.SetZ(1.f);//Move Forward
    if (KInput::IsKeyPress('S'))dir.SetZ(-1.f);//Move Backward
    if (KInput::IsKeyPress('A'))dir.SetX(-1.f);//Move Left
    if (KInput::IsKeyPress('D'))dir.SetX(1.f);//Move Right

    m_pPlayerCharacter->Move(dir);

}

void PlayerController::UpdateWindowSize(DirectX::XMFLOAT2 windowSize)
{
    m_windowSize = windowSize;
}

void PlayerController::UpdateWindowCenter(POINT center)
{
    m_centerPos = center;
}
