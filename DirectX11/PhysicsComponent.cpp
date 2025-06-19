#include "PhysicsComponent.h"
#include <Physics/Body/Body.h>

#include "PhysicsManager.h"
#include "PhysicsManager.h"

PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::SetPosition(const DirectX::XMFLOAT3& pos, JPH::EActivation eActivation)
{
	PhysicsManager::Instance().GetBodyInterface().SetPosition(m_bodyID, RVec3(pos.x, pos.y, pos.z),eActivation);

}

void PhysicsComponent::SetRotation(const DirectX::XMFLOAT3& rot, JPH::EActivation eActivation)
{
	PhysicsManager::Instance().GetBodyInterface().SetRotation(m_bodyID, Quat::sEulerAngles(RVec3(rot.x, rot.y, rot.z)), eActivation);
}

