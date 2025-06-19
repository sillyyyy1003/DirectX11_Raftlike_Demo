#pragma once
#include <DirectXMath.h>
#include "Transform.h"

class CameraBase
{
protected:

	// Frustum Param
	float m_nearZ = 0.1f;
	float m_farZ = 0.0f;
	float m_aspect = 0.f;
	float m_fovY = 0.0f;//Default:90°


public:

	Transform m_transform = {};

	CameraBase();
	virtual ~CameraBase() = default;
	virtual void Update(float dt) = 0;

	/// @brief 位置取得
	DirectX::XMFLOAT3 GetPos() const noexcept { return m_transform.GetPosition(); };
	/// @brief 位置設定
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept { m_transform.SetPosition(pos); };

	/// @brief 上方向取得
	const DirectX::XMFLOAT3 GetUpDir()noexcept { return m_transform.GetUpAxis(); };

	const DirectX::XMMATRIX GetViewXM()noexcept;
	const DirectX::XMMATRIX GetProjXM(bool isReversed = false)noexcept;

	/// @brief ビュー行列を取得
	/// @param isTranspose 行列の行と列を交換するか
	/// @return 
	const  DirectX::XMFLOAT4X4 GetViewXMF(bool isTranspose = true)noexcept;

	/// @brief 投影行列を取得
	/// @param isTranspose 行列の行と列を交換するか
	/// @return 
	const DirectX::XMFLOAT4X4 GetProjXMF(bool isTranspose = true) noexcept;

	/// @brief 目標位置を撮る
	/// @param target 
	virtual void SetTarget(const DirectX::XMFLOAT3& target){};

};
