#include "GameObject.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"

GameObject::GameObject() :
	m_transform(Transform()),
	m_isActive(true)
{
}

void GameObject::Update(float dt)
{
	if (!m_isActive)return;

	//==========Update Physics
	if(GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)!=nullptr)
	{
		PhysicsComponent* physics = GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics);
		switch(physics->GetEmotionType())
		{
		case JPH::EMotionType::Dynamic:
			physics->SyncPhysicsToTransform(m_transform);
			break;
		case JPH::EMotionType::Kinematic:
			physics->SyncTransformToPhysics(m_transform);
			break;
		case JPH::EMotionType::Static:
			physics->SyncTransformToPhysics(m_transform);
			break;
		}
	}

	//==========Update Input


	//==========Update GameLogic

}



void GameObject::Draw()
{
	if (!m_isActive)return;
	if (GetComponent<RenderComponent>(MyComponent::ComponentType::Render) != nullptr)
		GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->Render(m_transform);

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

