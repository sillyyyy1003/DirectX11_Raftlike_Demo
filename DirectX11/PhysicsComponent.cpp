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

void PhysicsComponent::Init(const BodyCreationSettings& settings, EActivation activation)
{
	
	m_bodyID = PhysicsManager::Instance().GetBodyInterface().CreateAndAddBody(settings, activation);

}

void PhysicsComponent::init(BodyID id)
{
	m_bodyID = id;
}


void PhysicsComponent::SetPosition(const DirectX::XMFLOAT3& pos, JPH::EActivation eActivation)
{
	PhysicsManager::Instance().GetBodyInterface().SetPosition(m_bodyID, RVec3(pos.x, pos.y, pos.z),eActivation);

}

void PhysicsComponent::SetPosition(float x, float y, float z, JPH::EActivation eActivation)
{
	SetPosition(DirectX::XMFLOAT3(x, y, z), eActivation);
}

void PhysicsComponent::SetPosition(float* pos, JPH::EActivation eActivation)
{
	SetPosition(DirectX::XMFLOAT3(pos[0], pos[1], pos[2]), eActivation);
}

void PhysicsComponent::SetRotation(const DirectX::XMFLOAT3& rot, JPH::EActivation eActivation)
{
	PhysicsManager::Instance().GetBodyInterface().SetRotation(m_bodyID, Quat::sEulerAngles(RVec3(rot.x, rot.y, rot.z)), eActivation);
}

void PhysicsComponent::SetRotation(float x, float y, float z, JPH::EActivation eActivation)
{
	SetRotation(DirectX::XMFLOAT3(x, y, z), eActivation);
}

void PhysicsComponent::SetRotation(float* rot, JPH::EActivation eActivation)
{
	SetRotation(DirectX::XMFLOAT3(rot[0], rot[1], rot[2]), eActivation);
}

void PhysicsComponent::SyncPhysicsToTransform(Transform& transform)
{
	transform.SetPosition(GetPosition());	//Sync Position
	transform.SetRotation(GetRotation());	//Sync Rotation
}

void PhysicsComponent::SyncTransformToPhysics(const Transform& transform)
{
	this->SetPosition(transform.GetPosition());
	this->SetRotation(transform.GetRotation());
}

DirectX::XMFLOAT3 PhysicsComponent::GetPosition() const
{
	BodyInterface& bi = PhysicsManager::Instance().GetBodyInterface();
	DirectX::XMFLOAT3 pos = { bi.GetPosition(m_bodyID).GetX(), bi.GetPosition(m_bodyID).GetY(), bi.GetPosition(m_bodyID).GetZ() };
	return pos;
}

DirectX::XMFLOAT3 PhysicsComponent::GetRotation() const
{
	BodyInterface& bi = PhysicsManager::Instance().GetBodyInterface();
	DirectX::XMFLOAT3 rot = { bi.GetRotation(m_bodyID).GetX(), bi.GetRotation(m_bodyID).GetY(), bi.GetRotation(m_bodyID).GetZ() };
	return rot;
}

void PhysicsComponent::AddForce(const DirectX::XMFLOAT3& vec3)
{
	BodyInterface& bi = PhysicsManager::Instance().GetBodyInterface();
	bi.AddForce(m_bodyID, RVec3(vec3.x, vec3.y, vec3.z), JPH::EActivation::Activate);
}

void PhysicsComponent::AddForce(float x, float y, float z)
{
	AddForce(DirectX::XMFLOAT3(x, y, z));
}

void PhysicsComponent::AddForce(float* vec3)
{
	AddForce(DirectX::XMFLOAT3(vec3[0], vec3[1], vec3[2]));
}

void PhysicsComponent::AddForce(const RVec3& vec3)
{
	BodyInterface& bi = PhysicsManager::Instance().GetBodyInterface();
	bi.AddForce(m_bodyID, vec3, JPH::EActivation::Activate);

}

void PhysicsComponent::SetVelocity(const DirectX::XMFLOAT3& vec3)
{
	BodyInterface& bi = PhysicsManager::Instance().GetBodyInterface();
	bi.SetLinearVelocity(m_bodyID, RVec3(vec3.x, vec3.y, vec3.z));
	DebugLog::Log("This object velocity is {},{},{}", bi.GetLinearVelocity(m_bodyID).GetX(), bi.GetLinearVelocity(m_bodyID).GetY(), bi.GetLinearVelocity(m_bodyID).GetZ());
}

void PhysicsComponent::SetVelocity(float x, float y, float z)
{
	SetVelocity(DirectX::XMFLOAT3(x, y, z));

}

void PhysicsComponent::SetVelocity(float* vec3)
{
	SetVelocity(DirectX::XMFLOAT3(vec3[0], vec3[1], vec3[2]));
}

float PhysicsComponent::GetMass() const
{
	return m_mass;

}

