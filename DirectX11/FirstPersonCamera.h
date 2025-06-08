#pragma once
#include "Windows.h"
#include "CameraBase.h"
#include "Transform.h"

/// @brief 一人称カメラ
class FirstPersonCamera : public CameraBase
{
private:

    float m_moveSpeed = 5.0f;    //移動速度

    int m_state = 0;
    POINT m_oldPos = { 0,0 };

    bool isLockPos = false;     // 位置移動可能？
    bool isLockAngle = false;   // 角度固定？

    //初期位置
    DirectX::XMFLOAT3 m_defaultPosition = {};

    //カメラ揺れで使われる変数
    bool isShaking = false;
    DirectX::XMFLOAT2 mShakingAmplitude;    //揺れの幅


public:

    enum CameraKind
    {
        /// No Interaction
        CAM_NONE,
        /// MOVE FREE
        CAM_FREE,
        /// Camera Shaking
        CAM_SHAKE,
    };

    FirstPersonCamera() = default;
    ~FirstPersonCamera() override = default;

    void Init();

    void Update(float dt) override;

    /// @brief カメラの位置設定
    void SetPosition(float x, float y, float z);
    void SetPosition(const DirectX::XMFLOAT3& pos);

    /// @brief カメラの向きを設定
    /// @param pos カメラの位置
    /// @param target ターゲットの位置
    /// @param up 上方向
    void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);


    void LookAt(const DirectX::XMFLOAT3& target);

    /// @brief ある方向に沿って観察
    /// @param pos カメラの位置
    /// @param to 方向
    /// @param up 上方向
    void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);

    /// @brief 平行移動
    /// @param d 距離
    void Strafe(float d);

    /// @brief 直行
    /// @param d 距離
    void Walk(float d);

    /// @brief 前に直行
    /// @param d 距離
    void MoveForward(float d);


    /// @brief 上下観察
    /// @param rad +:UP -:DOWN
    void Pitch(float rad);

    /// @brief 左右観察
    /// @param rad +:LEFT -:RIGHT
    void RotateY(float rad);

    void MoveUpward(float d);

    /// @brief カメラの角度をロックする
    void LockCameraAngle(bool isLockAngle) { this->isLockAngle = isLockAngle; };
    bool GetAngleLock() { return isLockAngle; };

    /// @brief カメラの位置をロックする
    void LockCameraPos(bool isLockPos) { this->isLockPos = isLockPos; };
    bool GetPosLock() { return isLockPos; };


    void LockCamera();


    /// @brief カメラ状態を設定する
    void SetCameraState(FirstPersonCamera::CameraKind state);

    /// @brief カメラの揺れを設定する
    /// @param amplitude 揺れ幅
    /// @param duration 揺れ時間(-1の場合は永遠)
    /// @param frequency 周波数
    void SetShake(float amplitude, float frequency, float duration = -1);

    bool GetShake() const { return isShaking; };



private:
    void UpdateState();
    void UpdateFlight(DirectX::XMFLOAT2 mouseMove, float dt);
    void Shake(float dt);
};

