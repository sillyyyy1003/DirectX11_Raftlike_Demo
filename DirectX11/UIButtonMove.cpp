#include "UIButtonMove.h"

namespace  
{
	static constexpr DirectX::XMFLOAT4 HoverAmbient = { 1,1,1,1 };
	static constexpr DirectX::XMFLOAT4 DefaultAmbient = { 0.8f,0.8f,0.8f,1.f };
}

UIButtonMove::UIButtonMove():
	m_moveSpeed(0),
	m_moveAmplitude(0),
	m_tick(0),
	m_isMove(false),
	m_defaultPos({0,0,0})
{
}

void UIButtonMove::OnMouseEnter()
{
	UIButton::OnMouseEnter();
}

void UIButtonMove::OnMouseExit()
{
	UIButton::OnMouseExit();
}

void UIButtonMove::OnMouseUp(float x, float y)
{
	UIButton::OnMouseUp(x, y);
}

void UIButtonMove::OnMouseDown(float x, float y)
{
	UIButton::OnMouseDown(x, y);
}

void UIButtonMove::InitMoveParam(float moveSpeed, float amplitude)
{
	m_moveSpeed = moveSpeed;
	m_moveAmplitude = amplitude;
}

void UIButtonMove::Update(float dt)
{
	if (!m_isMove)return;

	m_tick += dt * m_moveSpeed;
	float floatDis = m_moveAmplitude * sin(m_tick); //Calculate current move
	DirectX::XMFLOAT3 pos = {
		m_defaultPos.x,
		m_defaultPos.y + floatDis,
		m_defaultPos.z
	};

	SetPosition(pos);
}

void UIButtonMove::SetButton(const DirectX::XMFLOAT3& pos, float width, float height)
{
	UIButton::SetButton(pos, width, height);

	// Default Pos
	m_defaultPos = pos;

	//Set default ambient Color
	m_pUiRender->GetRenderComponent()->GetMaterial()->SetAmbient(DefaultAmbient);
}


void UIButtonMove::ActiveMove()
{
	m_isMove = true;
	m_pUiRender->GetRenderComponent()->GetMaterial()->SetAmbient(HoverAmbient);
}

void UIButtonMove::DeActiveMove()
{
	m_isMove = false;
	m_pUiRender->GetRenderComponent()->GetMaterial()->SetAmbient(DefaultAmbient);	//Set button color
	SetPosition(m_defaultPos);	//Reset button position
}
