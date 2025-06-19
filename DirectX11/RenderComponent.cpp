#include "RenderComponent.h"

void RenderComponent::Render(Transform& transform)
{
	assert(m_pMaterial != nullptr);
	assert(m_pEffect != nullptr);
	assert(m_pModel != nullptr);

	//Set WVP
	m_pEffect->SetWVPMatrix(transform);

	//Set Light
	m_pEffect->SetDirLight();

	//Set Material
	m_pEffect->SetMaterial(m_pMaterial);

	//Set Camera
	m_pEffect->SetCameraCB();

	//Apply
	m_pEffect->Apply();

	//•`‰æ
	m_pModel->Draw();
}

void UIRenderComponent::Render(Transform& transform)
{
	assert(m_pEffect != nullptr);
	assert(m_pModel != nullptr);
	assert(m_pMaterial != nullptr);

	//WVP
	m_pEffect->SetWVPMatrix(transform);

	//Material/texture
	m_pEffect->SetMaterial(m_pMaterial);

	//Effect Bind
	m_pEffect->Apply();

	//Model•`‰æ
	m_pModel->Draw();
}
