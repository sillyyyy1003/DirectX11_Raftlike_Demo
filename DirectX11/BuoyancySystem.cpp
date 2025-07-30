#include "BuoyancySystem.h"

#include <Jolt/Physics/PhysicsScene.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include "Layers.h"
#include "PhysicsManager.h"

BuoyancySystem::BuoyancySystem()
{
}

void BuoyancySystem::Init(float waterWidth, float waterHeight)
{
	// Add listener to system
	PhysicsManager::Instance().GetPhysicsSystem()->SetContactListener(this);

	// Create water sensor. We use this to detect which bodies entered the water
	BodyCreationSettings waterSensor(new BoxShape(Vec3(waterWidth,waterHeight,waterWidth)), RVec3::sZero(), Quat::sIdentity(), EMotionType::Static, Layers::SENSOR);
	waterSensor.mIsSensor = true;
	m_waterSensor = PhysicsManager::Instance().GetBodyInterface().CreateAndAddBody(waterSensor, EActivation::Activate);
}

