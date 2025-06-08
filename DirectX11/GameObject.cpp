#include "GameObject.h"

GameObject::GameObject() :m_pEffect(nullptr), m_pMaterial(nullptr), m_pModel(nullptr)
{
}

void GameObject::Update(float dt)
{
	m_transform.Rotate({ 0.f,DirectX::XMConvertToRadians(20.f * dt),0.f });
}

void GameObject::Draw()
{
	assert(m_pMaterial != nullptr);
	assert(m_pEffect != nullptr);

	//Set WVP
	m_pEffect->SetWVPMatrix(m_transform);

	//Set Light
	m_pEffect->SetDirLight();

	//Set Material
	m_pEffect->SetMaterial(m_pMaterial);

	//Set Camera
	m_pEffect->SetCameraMatrix();

	//Set Texture...

	//Apply
	m_pEffect->Apply();

	//描画
	m_pModel->Draw();
}
