#include "PlayerController.h"
#include "KInput.h"
#include "Player.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include "PhysicsManager.h"
#include "GameObject.h"

namespace
{

    static constexpr float RayDistance = 4.f;
}


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
    if (!m_isActive)return;
    if (!m_isControllable)return;


    // Show cursor
    {
        if (KInput::IsKeyRelease(VK_LCONTROL))
        {
            if (!m_isCursorLocked)
            {
                m_isCursorLocked = true;
                ShowCursor(FALSE);
            }
            else
            {
                m_isCursorLocked = false;
                ShowCursor(TRUE);
            }

        }
    }

    //Cursor SetMoveDir
    {
        if(m_isCursorLocked)
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
            m_pPlayerCharacter->SetRotation({ 0,m_pPlayer->GetTransform().GetRotation().y,0 });

            SetCursorPos(m_centerPos.x, m_centerPos.y);
        }

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
        if (KInput::IsKeyRelease(VK_SPACE))
            m_pPlayerCharacter->Jump();
    }


    //F key to Get the object
    {
        if(KInput::IsKeyRelease('F'))
        {
            BodyID id;
            if(GetRayHitBodyID(id))
            {
                m_pPlayer->InteractWithObject(id);
            }
        }

    }

    // LMB to use item in inventory 
    {
       if(m_isCursorLocked)
       {
           // On Use Start
           if (KInput::IsKeyTrigger(VK_LBUTTON))
           {
               if (m_pPlayer->GetItemInHand())
                   m_pPlayer->GetItemInHand()->OnUseStart(m_pPlayer);
           }

           // On Using
           if (KInput::IsKeyPress(VK_LBUTTON))
           {
               if (m_pPlayer->GetItemInHand())
                   m_pPlayer->GetItemInHand()->OnUseHold(m_pPlayer, dt);
           }


           // On Use End
           if (KInput::IsKeyRelease(VK_LBUTTON))
           {
               if (m_pPlayer->GetItemInHand())
                   m_pPlayer->GetItemInHand()->OnUseRelease(m_pPlayer);
           }

       }
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

void PlayerController::SetControllable(bool isControllable)
{
	m_isControllable = isControllable;
}

void PlayerController::SetActive(bool isActive)
{
	m_isActive=isActive;
}

bool PlayerController::GetRayHitBodyID(BodyID& id) const
{ //Get Camera pos & forward vector;
    CameraBase* camera = m_pPlayer->GetCameraController()->GetCamera();
    DirectX::XMFLOAT3 originPos = camera->GetPos();
    DirectX::XMFLOAT3 forwardVec = camera->m_transform.GetForwardAxis();

    float distance = RayDistance;

    RVec3 origin = {
        originPos.x,
        originPos.y,
        originPos.z
    };

    RVec3 direction = {
        forwardVec.x,
        forwardVec.y,
        forwardVec.z
    };

    JPH::RRayCast rayCast(origin, direction * distance);
    JPH::RayCastResult result;
    ExcludeLayerFilter layerFilter;

    if (PhysicsManager::Instance().GetPhysicsSystem()->GetNarrowPhaseQuery().CastRay(rayCast, result, {}, { layerFilter }, {}))
    {
        id = result.mBodyID;
        return true;
    }else
    {
        return false;
    }
}
