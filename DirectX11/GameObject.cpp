#include "GameObject.h"

GameObject::GameObject() :
	m_transform(Transform())
{
}

void GameObject::Update(float dt)
{
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

