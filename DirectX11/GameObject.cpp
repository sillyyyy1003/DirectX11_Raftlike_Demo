#include "GameObject.h"

#include "MaterialManager.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

GameObject::GameObject(GameObjectType type) :
	m_transform(Transform()),
	m_isActive(true),
	m_objectType(type)
{
	
}

GameObject::~GameObject()
{
	// clear all components reference
	for (auto& [type, comp] : m_components)
	{
		comp.reset(); // 释放每个 Component
	}
	m_components.clear(); // Clear all components
}

void GameObject::Update(float dt)
{
	if (!m_isActive)return;

	//==========Update Physics
	if(GetComponent<PhysicsComponent>()!=nullptr)
	{
		PhysicsComponent* physics = GetComponent<PhysicsComponent>();
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
	if (GetComponent<RenderComponent>() != nullptr)
		GetComponent<RenderComponent>()->Render(m_transform);
	
	

#if defined(_DEBUG) || defined(DEBUG)
	// Draw debug information if needed
	if(DebugRenderComponent* debugRender = GetComponent<DebugRenderComponent>())
	{
		if (PhysicsComponent * physics = GetComponent<PhysicsComponent>() )
		{
			DirectX::XMFLOAT3 debugScale = physics->GetBodySize();
	
			
			// Transform設定
			Transform debugTransform = m_transform;
			debugTransform.SetScale(debugScale);
			debugTransform.SetPosition(physics->GetPosition());
			debugRender->Render(debugTransform);
		}
	}
#endif 

}

void GameObject::SetPosition(const DirectX::XMFLOAT3& pos)
{
	// Dynamic physics のみ物理で位置変更
	if (GetComponent<PhysicsComponent>() != nullptr)
	{
		PhysicsComponent* physics = GetComponent<PhysicsComponent>();
		if (physics->GetEmotionType() == EMotionType::Dynamic)
		{
			physics->SetPosition(pos);
			return;
		}
	}
	m_transform.SetPosition(pos);
}
