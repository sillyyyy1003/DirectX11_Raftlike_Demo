#include "FirstPersonCamera.h"

#include <imgui/Include/imgui.h>

#include "KInput.h"

using namespace DirectX;


void FirstPersonCamera::Init()
{
    ////Random
    //mNoise.SetSeed(1);
    //mNoise.SetFrequency(10.f);
    //mNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

void FirstPersonCamera::Update(float dt)
{
    UpdateState();


    if (mState == CAM_NONE) return;

    switch (mState)
    {
    case CAM_FREE:
        // マウス移動量
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        XMFLOAT2 mouseMove = DirectX::XMFLOAT2((float)cursorPos.x - mOldPos.x, (float)cursorPos.y - mOldPos.y);
        mOldPos = cursorPos;
        UpdateFlight(mouseMove, dt);
        break;
    case CAM_MOVE:
        UpdateMove(dt);
        break;
    case CAM_SHAKE:
        UpdateShake(dt);
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

void FirstPersonCamera::LookDown()
{
    //XMFLOAT3 rotation = m_transform.GetRotationInRadian();
    //rotation.x = -XM_PI / 2;
    //m_transform.SetRotationInRadian(rotation);
}

void FirstPersonCamera::Pitch(float rad)
{
    //XMFLOAT3 rotation = m_transform.GetRotationInRadian();
    //// LIMIT ROTATE DEGREE TO ±85°
    //rotation.x += rad;
    //if (rotation.x > XM_PI * 85 / 18)
    //    rotation.x = XM_PI * 85 / 18;
    //else if (rotation.x < -XM_PI * 85 / 18)
    //    rotation.x = -XM_PI * 85 / 18;

    //m_transform.SetRotationInRadian(rotation);
}

void FirstPersonCamera::RotateY(float rad)
{
    //XMFLOAT3 rotation = m_transform.GetRotationInRadian();
    //rotation.y = XMScalarModAngle(rotation.y + rad);
    //m_transform.SetRotationInRadian(rotation);
}

void FirstPersonCamera::MoveUpward(float d)
{
    m_transform.Translate(m_transform.GetUpAxis(), d);
}




void FirstPersonCamera::LockCamera()
{
    isLockAngle = true;
    isLockPos = true;
}




void FirstPersonCamera::ZoomIn(float dt)
{
    if (mAccumulateTime <= mDuration)
    {
        //累積時間
        float step = dt / mDuration;
        mAccumulateTime += dt;
        m_transform.Translate(mDirection, step * mDistance);

        Pitch(mTargetRotation.x * step);

    }
    else
    {
        //運動停止
        isMoveToTarget = false;
        //時間リセット
        mAccumulateTime = 0.0f;

    }
}

void FirstPersonCamera::Shake(float dt)
{
    mAccumulateTime += dt;

    //float shakeX = mShakingAmplitude.x * mNoise.GetNoise(mAccumulateTime, 1.0f);
    //float shakeY = mShakingAmplitude.y * mNoise.GetNoise(mAccumulateTime, 2.0f);

    //SetPosition(mDefaultPosition.x + shakeX, mDefaultPosition.y + shakeY, mDefaultPosition.z);

}

void FirstPersonCamera::SetCameraState(FirstPersonCamera::CameraKind state)
{
    mState = state;
}

void FirstPersonCamera::SetShake(float amplitude, float frequency, float duration)
{
    //isShaking = true;
    ////揺れ時間を設定
    //mDuration = duration;

    ////周波数設定
    //mNoise.SetFrequency(frequency);

    ////揺れは幅を設定
    //mShakingAmplitude = { amplitude,amplitude };
    ////元の位置を確定
    //mDefaultPosition = this->GetPos();
}

void FirstPersonCamera::StopShake()
{
    isShaking = false;

    mAccumulateTime = 0.f;

    mDuration = 0.f;

}


void FirstPersonCamera::UpdateState()
{
    //Zoom in の場合
    if (isMoveToTarget)
    {
        mState = CAM_MOVE;
        return;
    }

    //カメラ揺れの場合
    if (isShaking)
    {
        mState = CAM_SHAKE;
        return;
    }

    // 操作の場合
    CameraKind prev = (CameraKind)mState;

    if (KInput::IsKeyPress(VK_MENU))//ALT
    {
        mState = CAM_NONE;
        //if (KInput::IsKeyTrigger('R'))ResetCamera();//カメラの位置を元に戻す
        
    }
    else if (KInput::IsKeyPress(VK_RBUTTON))
    {
        mState = CAM_FREE;
    }
    else
    {
        mState = CAM_NONE;
    }

    if (prev != mState)
        GetCursorPos(&mOldPos);



}

void FirstPersonCamera::UpdateFlight(DirectX::XMFLOAT2 mouseMove, float dt)
{
    // if (!isLockAngle)
    // {
         //横回転
    //float angleX = 360.0f * mouseMove.x / WIN_WIDTH;
    //RotateY(angleX * dt * mMoveSpeed);
    ////縦回転
    //float angleY = 180.0f * mouseMove.y / WIN_HEIGHT;
    //Pitch(angleY * dt * mMoveSpeed);
    // }

    // if (isLockPos)return;


     // キー入力で移動
    if (KInput::IsKeyPress('W'))//Move Forward
        Walk(dt * mMoveSpeed);
    if (KInput::IsKeyPress('S'))//Move Backward
        Walk(dt * -mMoveSpeed);
    if (KInput::IsKeyPress('A'))//Move Left
        Strafe(dt * -mMoveSpeed);
    if (KInput::IsKeyPress('D'))//Move Right
        Strafe(dt * mMoveSpeed);
    if (KInput::IsKeyPress('Q'))
        MoveUpward(dt * mMoveSpeed);
    if (KInput::IsKeyPress('E'))
        MoveUpward(dt * -mMoveSpeed);

    // Wheel入力
    //if (gD3D->GetMoveUnit() != 0)
    //{
    //    MoveForward(gD3D->GetMoveUnit() * 0.2f);
    //    gD3D->SetMoveUnit(0);
    //}

}

void FirstPersonCamera::UpdateMove(float dt)
{
    ZoomIn(dt);
}

void FirstPersonCamera::UpdateShake(float dt)
{
    //延々と揺らす
    if (mDuration < 0)
    {
        Shake(dt);

    }
    else
    {
        if (mAccumulateTime <= mDuration)
        {
            Shake(dt);

        }
        else
        {
            //時間リセット
            mAccumulateTime = 0.0f;
            //揺れを停止
            isShaking = false;
            //元の位置に戻す
            SetPosition(mDefaultPosition);
        }

    }

}
