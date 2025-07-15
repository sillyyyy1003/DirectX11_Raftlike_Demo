#pragma once
#include "UIElement.h"

/// @brief Basic Ui Button class with mesh& text
class UIButton:
	public UIElement
{
public:
	UIButton(ID2D1RenderTarget* renderTarget);
	~UIButton() override = default;

	bool HitTest(float x, float y) const override;
	void OnMouseEnter() override;
	void OnMouseExit() override;
	void OnMouseUp(float x, float y) override;
	void OnMouseDown(float x, float y) override;
	void OnClick(float x, float y) override;

	/// @brief Set button size& pos
	/// @param pos button position in world space
	/// @param width rectangle width
	/// @param height rectangle height
	void SetButton(const DirectX::XMFLOAT3& pos, float width, float height);

private:
	RECT m_buttonRect;	// Button rectangle
	bool m_isHovered;	// Button hovered state
	bool m_isPressed;	// Button pressed state
	std::function<void()> m_onClick;	// Click event handler
};

