#include "GameObject.h"

GameObject::GameObject()
{
	m_transform = Transform();
	m_pRenderComponent = std::make_unique<RenderComponent>();
}

void GameObject::Update(float dt)
{
	//==========Update Physics

	//==========Update Input

	//==========Update GameLogic

}



void GameObject::Draw()
{
	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->Render(m_transform);
}

