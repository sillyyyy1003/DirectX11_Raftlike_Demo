#include "UICharge.h"

void UICharge::Init(Material* mat, IEffect* effect, Primitive* model)
{
	m_pUiRender = std::make_unique<UIRender>();
	m_pUiRender->Init(mat, effect, model);
}

void UICharge::SetScale(DirectX::XMFLOAT2 scale)
{
	m_pUiRender->GetTransform().SetScale({ scale.x, scale.y, 1.0f });
}

void UICharge::SetPosition(DirectX::XMFLOAT3 pos)
{
	m_pUiRender->GetTransform().SetPosition(pos);
}

void UICharge::SetProgress(float progress)
{
	m_pUiRender->GetRenderComponent()->GetMaterial()->SetEmission({ 1, 1, 1, progress });
}

void UICharge::Draw()
{
	m_pUiRender->Draw();
}
