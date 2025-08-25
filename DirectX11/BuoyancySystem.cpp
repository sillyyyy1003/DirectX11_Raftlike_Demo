#include "BuoyancySystem.h"
#include <Jolt/Physics/PhysicsScene.h>
#include <Physics/Body/BodyLock.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include "Layers.h"
#include "PhysicsManager.h"
#include <Core/QuickSort.h>


namespace 
{
	static constexpr float		ObjectBuoyancy = 2.5f;
	static constexpr float		ObjectLinearDrag = 0.5f;
	static constexpr float		ObjectAngularDrag = 0.1f;
	static constexpr float		WaterLevel = 0.f;
	
}

BuoyancySystem::BuoyancySystem()
{
}

void BuoyancySystem::Init(float waterWidth, float waterHeight)
{
	// Add listener to physics system
	PhysicsManager::Instance().GetPhysicsSystem()->SetContactListener(this);

	// Create water sensor. We use this to detect which bodies entered the water
	// aware that box shape use half extent
	// aware that water sensor pos should be under the horizon which is -0.5*height
	BodyCreationSettings waterSensor(new BoxShape(Vec3(waterWidth / 2.f, waterHeight / 2.f, waterWidth / 2.f)), Vec3(0, -waterHeight / 2.f, 0), Quat::sIdentity(), EMotionType::Static, Layers::WATER_SENSOR);
	waterSensor.mIsSensor = true;
	// waterSensor doesn't have a rigid body so no need to add to physics bodies container
	m_waterSensor = PhysicsManager::Instance().GetBodyInterface().CreateAndAddBody(waterSensor, EActivation::Activate);

}

void BuoyancySystem::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold,
	ContactSettings& ioSettings)
{
	// When a body enters the water add it to the list of bodies in the water
	lock_guard<Mutex> lock(m_bodiesInWaterMutex);
	if (inBody1.GetID() == m_waterSensor && inBody2.GetMotionType() == EMotionType::Dynamic)
	{
		m_bodiesInWater.push_back(inBody2.GetID());
		DebugLog::Log("[BuoyancySystem] : Add object to water!");
	}
		
	else if (inBody2.GetID() == m_waterSensor && inBody1.GetMotionType() == EMotionType::Dynamic)
	{
		DebugLog::Log("[BuoyancySystem] : Add object to water!");
		m_bodiesInWater.push_back(inBody1.GetID());

	}

	QuickSort(m_bodiesInWater.begin(), m_bodiesInWater.end());// Sort to make deterministic (OnContactAdded is called from multiple threads and the order is not guaranteed)

}

void BuoyancySystem::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	lock_guard<Mutex> lock(m_bodiesInWaterMutex);
	if (inSubShapePair.GetBody1ID() == m_waterSensor)
	{
		DebugLog::Log("[BuoyancySystem] : Remove object from water! There are {} objects in water.",m_bodiesInWater.size());
		m_bodiesInWater.erase(std::find(m_bodiesInWater.begin(), m_bodiesInWater.end(), inSubShapePair.GetBody2ID()));
	}
	else if(inSubShapePair.GetBody1ID()==m_waterSensor)
	{
		DebugLog::Log("[BuoyancySystem] : Remove object from water! There are {} objects in water.", m_bodiesInWater.size());
		m_bodiesInWater.erase(std::find(m_bodiesInWater.begin(), m_bodiesInWater.end(), inSubShapePair.GetBody1ID()));
	}
		
}

void BuoyancySystem::PreUpdate(float dt)
{
	
	lock_guard<Mutex> lock(m_bodiesInWaterMutex);

	for (const BodyID& id : m_bodiesInWater)
	{
		BodyLockWrite body_lock(PhysicsManager::Instance().GetPhysicsSystem()->GetBodyLockInterface(), id);
		Body& body = body_lock.GetBody();
		if(body.IsActive())
		{
			// Assume water y height=0& the surface is flat
			// todo : make the surface move!
			RVec3 surface_position = body.GetCenterOfMassPosition();
			surface_position.SetY(WaterLevel);
			Vec3 surface_normal = { 0,1,0 };

			float buoyancy, linear_drag, angular_drag;
			buoyancy = ObjectBuoyancy;
			linear_drag = ObjectLinearDrag;
			angular_drag = ObjectAngularDrag;

			body.ApplyBuoyancyImpulse(
				surface_position,
				surface_normal,
				buoyancy,
				linear_drag,
				angular_drag,
				Vec3::sZero(),
				PhysicsManager::Instance().GetPhysicsSystem()->GetGravity(),
				dt);
		}

	}
	
}

