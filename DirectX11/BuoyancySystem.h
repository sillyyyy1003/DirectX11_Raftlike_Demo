#pragma once
#include <DirectXCollision.h>
#include <Jolt.h>
#include <Core/Mutex.h>
#include <Physics/Body/BodyManager.h>
#include <Physics/Collision/ContactListener.h>
#include "RenderComponent.h"

using namespace JPH;

class BuoyancySystem:
	public ContactListener
{
public:
	BuoyancySystem();
	~BuoyancySystem() = default;

	void Init(float waterWidth, float waterHeight);
	virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
	virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;

	void PreUpdate(float dt);


private:

	Mutex m_bodiesInWaterMutex;
	BodyIDVector m_bodiesInWater;
	BodyID m_waterSensor;
};

