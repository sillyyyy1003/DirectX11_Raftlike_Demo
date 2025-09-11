#pragma once
#include "UIButton.h"

/// @brief Button that would float when hovered & move when pressed
class UIButtonMove :
    public UIButton
{
public:
	UIButtonMove();
	~UIButtonMove() override = default;
	void OnMouseEnter() override;
	void OnMouseExit() override;
	void OnMouseUp(float x, float y) override;
	void OnMouseDown(float x, float y) override;

	/// @brief 移動関連のパラメーターを設定
	/// @param moveSpeed 移動速度
	/// @param amplitude 移動幅
	void InitMoveParam(float moveSpeed, float amplitude);
	void Update(float dt) override;
	void SetButton(const DirectX::XMFLOAT3& pos, float width, float height) override;

	/// @brief Active button move & change to brighter color
	void ActiveMove();

	/// @brief DeActive button move & change to darker color
	void DeActiveMove();

	void LoadButtonConfig(const char* fileName, const char* buttonName);
	void LoadButtonConfig(nlohmann::json& j, const char* buttonName);
private:
	float m_moveSpeed;				// 移動速度
	float m_moveAmplitude;			// 移動幅
	float m_tick;
	bool m_isMove;

	DirectX::XMFLOAT3 m_defaultPos;

	
};


