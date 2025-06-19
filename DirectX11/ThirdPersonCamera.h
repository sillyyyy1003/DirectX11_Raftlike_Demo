#pragma once
#include "CameraBase.h"

class ThirdPersonCamera:
	public CameraBase
{
public:
    ThirdPersonCamera() = default;
    ~ThirdPersonCamera() override;

	/// @brief ターゲットの位置を取得
    DirectX::XMFLOAT3 GetTargetPosition() const;

	/// @brief ターゲットとの距離を取得
    float GetDistance() const;

	/// @brief ターゲットに沿って中心にX軸を中心に回転[0, pi/3]
    void RotateX(float rad);

	/// @brief 水平面でY軸を中心に回転
    void RotateY(float rad);

	/// @brief ターゲットの位置に向かってカメラを移動
    void Approach(float dist);

	///@brief X軸を中心に回転角度設定
    void SetRotationX(float rad);

    ///@brief Y軸を中心に回転角度設定
    void SetRotationY(float rad);

	/// @brief ターゲットの位置を設定
    void SetTarget(const DirectX::XMFLOAT3& target) override;

    /// @brief 初期の距離設定
    void SetDistance(float dist);

	/// @brief 最小最大許有距離を設定
    void SetDistanceMinMax(float minDist, float maxDist);

    void Update(float dt) override {};
private:
    DirectX::XMFLOAT3 m_target = {};
    float m_distance = 0.0f;
    // 最小允许距离，最大允许距离
    float m_minDist = 0.0f, m_maxDist = 0.0f;
};

