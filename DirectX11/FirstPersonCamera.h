#pragma once
#include "Windows.h"
#include "CameraBase.h"
#include "Transform.h"

/// @brief 一人称カメラ
class FirstPersonCamera : public CameraBase
{
private:

    float m_moveSpeed;                          //移動速度
    int m_state;
    POINT m_oldPos;
    DirectX::XMFLOAT3 m_defaultPosition;       //初期位置
    DirectX::XMFLOAT2 m_windowSize;            //Window Size

public:

    enum CameraKind
    {
        /// No Interaction
        CAM_NONE,
        /// MOVE FREE
        CAM_FREE,
    };

    FirstPersonCamera();
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

    /// @brief カメラ状態を設定する
    void SetCameraState(FirstPersonCamera::CameraKind state);


    /// @brief 目標位置を撮る
    /// @param target 
    void SetTarget(const DirectX::XMFLOAT3& target) override;


    /// @brief WindowSizeを変更する
    /// @param windowSize 
    void SetWindowSize(DirectX::XMFLOAT2 windowSize) { m_windowSize = windowSize; };

private:
    void UpdateState();
    void UpdateFlight(DirectX::XMFLOAT2 mouseMove, float dt);
    void Shake(float dt);
};

