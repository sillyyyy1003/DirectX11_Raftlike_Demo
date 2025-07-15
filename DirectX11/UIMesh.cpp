#include "UIMesh.h"

UIMesh::UIMesh():
	m_pRender(nullptr)
{
}

void UIMesh::Init(IEffect* effect, Material* material, Primitive* model)
{
	m_pRender = std::make_unique<UIRender>();
	m_pRender->SetEffect(effect);
	m_pRender->SetMaterial(material);
	m_pRender->SetModel(model);
}

void UIMesh::Draw()
{
	m_pRender->Draw();
}

void UIMesh::SetPosition(const DirectX::XMFLOAT3& pos)
{
	m_pRender->GetTransform().SetPosition(pos);
}

void UIMesh::SetScale(const DirectX::XMFLOAT3& scale)
{
	m_pRender->GetTransform().SetScale(scale);
}
