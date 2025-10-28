#include "RenderComponent.h"

void RenderComponent::Render(Transform& transform)
{
	assert(m_pMaterial != nullptr);
	assert(m_pEffect != nullptr);
	assert(m_pModel != nullptr);

	////Set WVP
	m_pEffect->SetConstantBuffer(transform, m_pMaterial);

	//Apply
	m_pEffect->Apply();

	//描画
	m_pModel->Draw();
}

void UIRenderComponent::Render(Transform& transform)
{
	assert(m_pEffect != nullptr);
	assert(m_pModel != nullptr);
	assert(m_pMaterial != nullptr);

	m_pEffect->SetConstantBuffer(transform, m_pMaterial);

	//Effect Bind
	m_pEffect->Apply();

	//Model描画
	m_pModel->Draw();
}
