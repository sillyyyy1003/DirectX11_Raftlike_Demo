#pragma once
#include <iostream>
#include <Jolt.h>
#include <RegisterTypes.h>
#include <Core/Factory.h>
#include <Core/TempAllocator.h>
#include <Core/JobSystemThreadPool.h>
#include <Physics/PhysicsSettings.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include <Physics/Collision/Shape/SphereShape.h>
#include <Physics/Body/BodyCreationSettings.h>
#include <Physics/Body/BodyActivationListener.h>
#include "Layers.h"

#pragma comment(lib, "Jolt.lib")

// All Jolt symbols are in the JPH namespace
using namespace JPH;
using namespace std;

namespace JoltPhysics
{
	static constexpr uint MaxBodies = 1024;	//max amount of rigid bodies 
	static constexpr uint NumBodyMutexes = 0;	//many mutexes to allocate to protect rigid bodies from concurrent access
	static constexpr uint MaxBodyPairs = 1024;	//max amount of body pairs that can be queued at any tim
	static constexpr uint MaxContactConstraints = 1024;	//maximum size of the contact constraint buffer

	static constexpr uint cNumBodies = 10240;
	static constexpr uint cNumBodyMutexes = 0; // Autodetect
	static constexpr uint cMaxBodyPairs = 65536;
	static constexpr uint cMaxContactConstraints = 20480;
}


/// @brief Using Jolt Physics to manage all the collision & physics
class PhysicsManager
{
public:
	/// @brief singleton 
	static PhysicsManager& Instance();

	/// @brief Trace function for logging
	static void TraceImpl(const char* fmt, ...);

#ifdef JPH_ENABLE_ASSERTS
	/// @brief Assert function for error checking
	static bool AssertFailedImpl(const char* expr, const char* msg, const char* file, uint line);
#endif // JPH_ENABLE_ASSERTS

	/// @brief 初期化
	void Init();

	/// @brief UnInit
	void UnInit();

	void Update(float dt);

	///The main way to interact with the bodies in the physics system is through the body interface. 
	BodyInterface& GetBodyInterface();

	BodyID CreateRigidBody(const BodyCreationSettings& settings, EActivation activation);

	void DeleteRigidBody(BodyID id);


private:
	PhysicsManager() = default;
	~PhysicsManager() = default;
	PhysicsManager(const PhysicsManager&) = delete;
	PhysicsManager& operator=(const PhysicsManager&) = delete;

	// Jolt
	int m_collisionSteps = 1;	//// How many collision detection steps per physics update
	std::unique_ptr<TempAllocator> m_tempAllocator;	// Temporary allocator for Jolt
	std::unique_ptr<JobSystem> m_jobSystem;	// Job system for Jolt
	std::unique_ptr<PhysicsSystem> m_pPhysicsSystem;	// Physics system
	std::shared_ptr<MyContactListener> m_pContactListener;	// Contact listener for collision events
	std::shared_ptr<MyBodyActivationListener> m_pBodyActivationListener;	// Body activation listener for body state changes

	BPLayerInterfaceImpl m_broadPhaseLayerInterface;
	ObjectLayerPairFilterImpl m_objectVsObjectLayerFilter;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	PhysicsSettings m_physicsSettings;	// Physics settings

	std::vector<BodyID> m_bodies;		// List of bodies in the physics system


};

