#include "BuoyancySystem.h"
#include <Jolt/Physics/PhysicsScene.h>
#include <Physics/Body/BodyLock.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include "Layers.h"
#include "PhysicsManager.h"
#include <Core/QuickSort.h>


namespace 
{
	static constexpr float		ObjectBuoyancy = 4.f;
	static constexpr float		ObjectLinearDrag = 0.5f;
	static constexpr float		ObjectAngularDrag = 0.1f;
	static constexpr float		WaterLevel =0.f;

	static constexpr int		WaterSlices = 50;	// Water mesh slice
	static constexpr float		MinWaterHeight = -0.05f;
	static constexpr float		MaxWaterHeight = 0.05f;

	
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

	m_transform.SetPosition(0, WaterLevel, 0);
}

void BuoyancySystem::Init(float waterWidth, float waterHeight, Material* mat, IEffect* effect)
{
	Init(waterWidth, waterHeight);

	// Init mesh
	m_pMesh = std::make_unique<PlaneMesh>();
	m_pMesh->Init(WaterSlices, waterWidth, m_vertices);	// Init mesh data& copy vertices data;

	m_pRenderComponent = make_unique<RenderComponent>();
	m_pRenderComponent->Init(mat, effect, m_pMesh.get());

	
}

void BuoyancySystem::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold,
                                    ContactSettings& ioSettings)
{
	// When a body enters the water add it to the list of bodies in the water
	lock_guard<Mutex> lock(m_bodiesInWaterMutex);
	
	if (inBody1.GetID() == m_waterSensor && inBody2.GetMotionType() == EMotionType::Dynamic)
	{
		if(inBody2.IsActive())
		{
			// if body is already in the list, do nothing
			if(std::find(m_bodiesInWater.begin(), m_bodiesInWater.end(), inBody2.GetID())!=m_bodiesInWater.end())
			{
				DebugLog::LogWarning("[BuoyancySystem] : Object is already in the list!BodyId is {}.",inBody2.GetID().GetIndex());
				return;
			}
			// push back body to the list
			m_bodiesInWater.push_back(inBody2.GetID());
			DebugLog::Log("[BuoyancySystem] : Add object to water! Object num is {}. BodyId is {}", m_bodiesInWater.size(), inBody2.GetID().GetIndex());
		}else
		{
			DebugLog::Log("[BuoyancySystem] : Object is inactive!");
			return;
		}
		
	}	
	else if (inBody2.GetID() == m_waterSensor && inBody1.GetMotionType() == EMotionType::Dynamic)
	{
		if(inBody1.IsActive())
		{
			// if body is already in the list, do nothing
			if (std::find(m_bodiesInWater.begin(), m_bodiesInWater.end(), inBody1.GetID()) != m_bodiesInWater.end())
			{
				DebugLog::LogWarning("[BuoyancySystem] : Object is already in the list!BodyId is {}.", inBody1.GetID().GetIndex());
				return;
			}
			// push back body to the list
			DebugLog::Log("[BuoyancySystem] : Add object to water! Object num is {}. BodyId is {}", m_bodiesInWater.size(),inBody1.GetID().GetIndex());
			m_bodiesInWater.push_back(inBody1.GetID());
		}else
		{
			DebugLog::Log("[BuoyancySystem] : Object is inactive!");
			return;
		}
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
	m_time += dt;

	//Update mesh data & water surface
	for(auto& vtx:m_vertices)
	{
		DirectX::XMFLOAT3 pos = GetWaterSurfacePosition(vtx.pos);
		vtx.pos = pos;	//Update vertex pos

		//todo: Adjust vertex normal according to the triangle vertex in
	}

	// Write vertices;
	m_pMesh->GetMeshBuffer()->Write(m_vertices.data());

	lock_guard<Mutex> lock(m_bodiesInWaterMutex);

	for (const BodyID& id : m_bodiesInWater)
	{
		BodyLockWrite body_lock(PhysicsManager::Instance().GetPhysicsSystem()->GetBodyLockInterface(), id);
		Body& body = body_lock.GetBody();


		if(body.IsActive())
		{

			// Use center of mass position to determine water surface position (you could test multiple points on the actual shape of the boat to get a more accurate result)
			RVec3 surface_position = GetWaterSurfacePosition(body.GetCenterOfMassPosition());
			
			// Crude way of approximating the surface normal
			RVec3 p2 = GetWaterSurfacePosition(body.GetCenterOfMassPosition() + Vec3(0, 0, 1));
			RVec3 p3 = GetWaterSurfacePosition(body.GetCenterOfMassPosition() + Vec3(1, 0, 0));
			Vec3 surface_normal = Vec3(p2 - surface_position).Cross(Vec3(p3 - surface_position)).Normalized();

			float buoyancy, linear_drag, angular_drag;
			buoyancy = ObjectBuoyancy;	// 浮力
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

void BuoyancySystem::Draw()
{
	m_pRenderComponent->Render(m_transform);
}

DirectX::XMFLOAT3 BuoyancySystem::GetWaterSurfacePosition(const DirectX::XMFLOAT3& pos) const
{
	return { pos.x,MinWaterHeight + sin(0.1f * float(pos.z) + m_time) * (MaxWaterHeight - MinWaterHeight),pos.z };
}

RVec3 BuoyancySystem::GetWaterSurfacePosition(RVec3Arg inXZPosition) const
{
	return RVec3(inXZPosition.GetX(), MinWaterHeight + sin(0.1f * float(inXZPosition.GetZ()) + m_time) * (MaxWaterHeight - MinWaterHeight), inXZPosition.GetZ());
}

