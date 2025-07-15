#include "UIButton.h"


UIButton::UIButton(ID2D1RenderTarget* renderTarget):
	UIElement(renderTarget),
	m_isPressed(false),
	m_isHovered(false),
	m_onClick(nullptr),
	m_buttonRect({})
{
}

bool UIButton::HitTest(float x, float y) const
{
	return x >= m_buttonRect.left && x <= m_buttonRect.right
		&& y >= m_buttonRect.top && y <= m_buttonRect.bottom;
}

void UIButton::OnMouseEnter()
{
	m_isHovered = true;
	SetMeshDiffuseColor({ 1,0,0,1 }); // Change color to red when hovered
	
}

void UIButton::OnMouseExit()
{
	m_isHovered = false;
	SetMeshDiffuseColor({ 0,1,0,1 }); // Change color to green when hovered
}

void UIButton::OnMouseUp(float x, float y)
{
	m_isPressed = false;
}

void UIButton::OnMouseDown(float x, float y)
{
	m_isPressed = true;
}

void UIButton::OnClick(float x, float y)
{
	if(m_onClick)
		m_onClick();
}

void UIButton::SetButton(const DirectX::XMFLOAT3& pos, float width, float height)
{
	// Set Render element size
	SetPosition(pos);
	SetScale({ width, height, 1.0f });

	// Set rectangle for button hit testing
	float x = 0, y = 0;
	x = pos.x + WIN_WIDTH / 2;
	y = WIN_HEIGHT / 2 - pos.y;
	m_buttonRect.left = (LONG)(x - width / 2);
	m_buttonRect.right = (LONG)(x + width / 2);
	m_buttonRect.top = (LONG)(y - height / 2);
	m_buttonRect.bottom = (LONG)(y + height / 2);
}

