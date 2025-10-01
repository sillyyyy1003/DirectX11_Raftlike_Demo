#include "UIButtonMove.h"
#include "d3dUtil.h"
#include "DebugLog.h"

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
	if (!m_isActive)return;
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

void UIButtonMove::SetButtonRect(const DirectX::XMFLOAT3& pos, float width, float height)
{
	UIButton::SetButtonRect(pos, width, height);

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

void UIButtonMove::LoadButtonConfig(const char* fileName, const char* buttonName)
{
	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[UIButton] {} Failed to open JSON file: {}", buttonName, fileName);
		return;
	}

	nlohmann::json j;
	ifs >> j;
	auto& ui = j[buttonName];

	DirectX::XMFLOAT3 buttonPos= JsonToXMFLOAT3(ui["position"]);
	float width = ui["width"].get<float>();
	float height = ui["height"].get<float>();
	float moveSpeed = ui["moveSpeed"].get<float>();
	float amplitude = ui["amplitude"].get<float>();

	SetButtonRect(buttonPos,width,height);
	InitMoveParam(moveSpeed, amplitude);

}

void UIButtonMove::LoadButtonConfig(nlohmann::json& j, const char* buttonName)
{
	auto& ui = j[buttonName];

	DirectX::XMFLOAT3 buttonPos = JsonToXMFLOAT3(ui["position"]);
	float width = ui["width"].get<float>();
	float height = ui["height"].get<float>();
	float moveSpeed = ui["moveSpeed"].get<float>();
	float amplitude = ui["amplitude"].get<float>();

	SetButtonRect(buttonPos, width, height);
	InitMoveParam(moveSpeed, amplitude);
}
