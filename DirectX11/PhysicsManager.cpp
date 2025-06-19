#include <cstdarg>
#include "PhysicsManager.h"
#include "DebugLog.h"

PhysicsManager& PhysicsManager::Instance()
{
	static PhysicsManager instance; // Guaranteed to be destroyed.
	return instance;				// Instantiated on first use.
}

void PhysicsManager::TraceImpl(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	DebugLog::Log("[Jolt] {}", buffer);
}
#ifdef JPH_ENABLE_ASSERTS
bool PhysicsManager::AssertFailedImpl(const char* expr, const char* msg, const char* file, uint line)
{
	if (msg)
		DebugLog::LogError("[Jolt Assert] {}:{} - ({}) {}", file, line, expr, msg);
	else
		DebugLog::LogError("[Jolt Assert] {}:{} - ({})", file, line, expr);

	return true;
}
#endif // JPH_ENABLE_ASSERTS

void PhysicsManager::Init()
{

	// Register allocation hook.
	RegisterDefaultAllocator();

	// Install trace and assert callbacks
	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl);

	// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
	Factory::sInstance = new Factory();

	// Register all physics types with the factory
	RegisterTypes();

	//pre-allocating 10 MB to avoid having to do allocations during the physics update.
	m_tempAllocator = make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

	m_jobSystem = make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	//Init Physics System
	m_pPhysicsSystem = make_unique<PhysicsSystem>();
	m_pPhysicsSystem->Init(JoltPhysics::cNumBodies, JoltPhysics::cNumBodyMutexes, JoltPhysics::cMaxBodyPairs, JoltPhysics::cMaxContactConstraints, m_broadPhaseLayerInterface, m_objectVsBroadPhaseLayerFilter, m_objectVsObjectLayerFilter);

	// A body activation listener gets notified when bodies activate and go to sleep
	m_pBodyActivationListener = make_shared<MyBodyActivationListener>();
	m_pPhysicsSystem->SetBodyActivationListener(m_pBodyActivationListener.get());

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	m_pContactListener = make_shared<MyContactListener>();
	m_pPhysicsSystem->SetContactListener(m_pContactListener.get());

}

void PhysicsManager::UnInit()
{
	BodyInterface& bodyInterface = GetBodyInterface();

	//======�S�Ẵ{�f�B���폜���Ă���APhysicsSystem��j������K�v������܂��B========
	{
		for (auto id : m_bodies)
		{
			bodyInterface.RemoveBody(id);	//Remove
			bodyInterface.DestroyBody(id);	//Destroy
		}
	}
	//=======Unregisters all types with the factory and cleans up the default material
	UnregisterTypes();

	delete Factory::sInstance;	// Delete the factory instance
	Factory::sInstance = nullptr;	// Set the instance to nullptr

	m_tempAllocator.reset();	// Reset the temp allocator
	m_jobSystem.reset();		// Release the job system
	m_pPhysicsSystem.reset();	// Release the physics system
	m_pContactListener.reset();	// Release the contact listener
	m_pBodyActivationListener.reset();	// Release the body activation listener
}

BodyInterface& PhysicsManager::GetBodyInterface()
{
	return m_pPhysicsSystem->GetBodyInterface();
}

BodyID PhysicsManager::CreateRigidBody(const BodyCreationSettings& settings, EActivation activation)
{
	BodyInterface& bodyInterface = GetBodyInterface();
	BodyID id = bodyInterface.CreateAndAddBody(settings, activation);

	// If the body ID is valid, add it to the list
	if (!id.IsInvalid())//true: id is invalid , false: id is valid
	{
		m_bodies.push_back(id);
	}
	return id;
}

void PhysicsManager::DeleteRigidBody(BodyID id)
{
	//valid check
	if (id.IsInvalid())
	{
		// Log a warning if the ID is invalid
		DebugLog::LogWarning("'[Jolt]:ID:{} is invalid", id.GetIndex());
	}

	BodyInterface& bodyInterface = GetBodyInterface();

	bodyInterface.RemoveBody(id);	//Need to Remove the body from the world before destroying it, otherwise it will crash.
	bodyInterface.DestroyBody(id);	//Destroy the body

	//Erase the body from the list
	std::erase(m_bodies, id);	//every id has its own param, so we can use std::erase to remove it from the list.

}




