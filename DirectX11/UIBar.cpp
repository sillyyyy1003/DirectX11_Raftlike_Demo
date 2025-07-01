#include "UIBar.h"

UIBar::UIBar() :
	m_backgroundMesh(nullptr),
	m_barMesh(nullptr),
	m_scale(0,0,1),
	m_position(0,0,0)
{
}

void UIBar::Init(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& scale, Material* bgMaterial,
	Material* barMaterial)
{
}

void UIBar::UpdateUI(float volume)
{
}

void UIBar::Draw(const char* string)
{
}

void UIBar::Update(float dt)
{
}


