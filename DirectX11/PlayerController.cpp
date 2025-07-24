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
    //Cursor SetMoveDir
    {
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        float dx = static_cast<float>(cursorPos.x - m_centerPos.x);
        float dy = static_cast<float>(cursorPos.y - m_centerPos.y);

        float angleX = 360.0f * dx / m_windowSize.x;
        float angleY = 180.0f * dy / m_windowSize.y;

        //プレイヤーのY回転計算
        m_pPlayer->RotateY(angleX * dt);
        //カメラの視角回転計算
        m_pPlayer->Pitch(angleY * dt);
        //Colliderの回転はY軸だけ
        m_pPlayerCharacter->SetRotation({0,m_pPlayer->GetTransform().GetRotation().y,0});

        SetCursorPos(m_centerPos.x, m_centerPos.y);
    }

    // 前後左右移動
	{
	    Vec3 dir = { 0,0,0 };

	    if (KInput::IsKeyPress('W'))//SetMoveDir Forward
	        dir.SetZ(1.f);
	    if (KInput::IsKeyPress('S'))//SetMoveDir Backward
	        dir.SetZ(-1.f);
	    if (KInput::IsKeyPress('A'))//SetMoveDir Left
	        dir.SetX(-1.f);
	    if (KInput::IsKeyPress('D'))//SetMoveDir Right
	        dir.SetX(1.f);

	    m_pPlayerCharacter->SetMoveDir(dir);//移動方向を設定する
    }

    // ジャンプ
    {
        if (KInput::IsKeyTrigger(VK_SPACE))
            m_pPlayerCharacter->Jump();
    }

}

void PlayerController::UpdateWindowSize(DirectX::XMFLOAT2 windowSize)
{
    m_windowSize = windowSize;
}

void PlayerController::UpdateWindowCenter(POINT center)
{
    m_centerPos = center;
}
