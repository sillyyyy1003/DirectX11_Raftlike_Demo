#include "GameObject.h"

GameObject::GameObject()
{
	m_transform = Transform();
	m_pRenderComponent = std::make_unique<RenderComponent>();
}

void GameObject::Update(float dt)
{
	//==========Update Physics
	if(GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)!=nullptr)
	{
		PhysicsComponent* physics = GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics);
		physics->SyncPhysicsToTransform(m_transform);
	}

	//==========Update Input


	//==========Update GameLogic

}



void GameObject::Draw()
{
	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->Render(m_transform);

#if defined(_DEBUG) || defined(DEBUG)
	// Draw debug information if needed
	if(GetComponent<RenderComponent>(MyComponent::ComponentType::DebugRender) != nullptr)
	{
		// Transform設定
		Transform debugTransform = m_transform;
		debugTransform.SetScale(m_debugCollisionScale);
		GetComponent<RenderComponent>(MyComponent::ComponentType::DebugRender)->Render(debugTransform);
	}
#endif 




}

