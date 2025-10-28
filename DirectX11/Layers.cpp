#include "Layers.h"
#include "HookInstance.h"
#include "ItemInstances.h"
#include "PhysicsManager.h"

namespace
{
	enum ObjectState :int
	{
		Active = 0,         // Spawned and waiting to be picked up
		InActive = 1,       // Can be spawned
		WaitToRecycle = 2,  // Marked for recycling
		Hooked = 3          // hooked by player
	};

}

void ObjectContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold,
	ContactSettings& ioSettings)
{
	DebugLog::Log("[Physics] A contact was added.");
	//========ここでOnCollisionEnterのロジックを追加する
	if ((inBody1.GetObjectLayer() == Layers::DRIFT && inBody2.GetObjectLayer() == Layers::TOOL))
	{
		DebugLog::Log("[Physics] A contact was added.");
		if (GameObject* object = PhysicsManager::Instance().GetPhysicsComponent(inBody1.GetID())->GetGameObject())
		{
			auto item = dynamic_cast<ItemInstance*>(object);

			// Set item state
			item->SetState(Hooked);


			GameObject* hook = PhysicsManager::Instance().GetPhysicsComponent(inBody2.GetID())->GetGameObject();
			auto hookItem = dynamic_cast<HookInstance*>(hook);

			hookItem->AddItem(item);

		}

	}
	else if ((inBody1.GetObjectLayer() == Layers::TOOL && inBody2.GetObjectLayer() == Layers::DRIFT))
	{
		DebugLog::Log("[Physics] A contact was added.");
		if (GameObject* object = PhysicsManager::Instance().GetPhysicsComponent(inBody2.GetID())->GetGameObject())
		{
			auto item = dynamic_cast<ItemInstance*>(object);

			// Set item state
			item->SetState(Hooked);
			DebugLog::Log("[Physics] A contact was added.");

			GameObject* hook = PhysicsManager::Instance().GetPhysicsComponent(inBody2.GetID())->GetGameObject();
			auto hookItem = dynamic_cast<HookInstance*>(hook);

			hookItem->AddItem(item);
		}
	}

}

void ObjectContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2,
                                               const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	//DebugLog::Log("[Physics] A contact was persisted.");
	//========ここでOnCollisionStayのロジックを追加する

	/*
	if ((inBody1.GetObjectLayer() == Layers::DRIFT && inBody2.GetObjectLayer() == Layers::TOOL))
	{
		if (GameObject* object = PhysicsManager::Instance().GetPhysicsComponent(inBody1.GetID())->GetGameObject())
		{
			auto item = dynamic_cast<ItemInstance*>(object);
			// Set item pos
			GameObject* hook = PhysicsManager::Instance().GetPhysicsComponent(inBody2.GetID())->GetGameObject();
			item->SetPosition(hook->GetTransform().GetPosition());

			// if hook is getting idle recycle item
			auto hookItem = dynamic_cast<HookInstance*>(hook);
			if (hookItem->GetHookState() == HookInstance::Idle)
			{
				item->SetState(WaitToRecycle);
			}
		}	

	}
	else if ((inBody1.GetObjectLayer() == Layers::TOOL && inBody2.GetObjectLayer() == Layers::DRIFT))
	{
		if (GameObject* object = PhysicsManager::Instance().GetPhysicsComponent(inBody2.GetID())->GetGameObject())
		{
			auto item = dynamic_cast<ItemInstance*>(object);
			// Set item pos
			GameObject* hook = PhysicsManager::Instance().GetPhysicsComponent(inBody1.GetID())->GetGameObject();
			item->SetPosition(hook->GetTransform().GetPosition());

			// if hook is getting idle recycle item
			auto hookItem = dynamic_cast<HookInstance*>(hook);
			if (hookItem->GetHookState() == HookInstance::Idle)
			{
				item->SetState(WaitToRecycle);
			}
		}
	}
	*/

}

void MyPlayerContactListener::OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2,
                                             const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal,
                                             CharacterContactSettings& ioSettings)
{
	// もし水中に
	if (PhysicsManager::Instance().GetBodyInterface().GetObjectLayer(inBodyID2) == Layers::WATER_SENSOR)
	{
		m_isInWater = true;
		DebugLog::Log("[PlayerContactListener] Character is in water now");
		return;
	}

}

void MyPlayerContactListener::OnContactRemoved(const CharacterVirtual* inCharacter, const BodyID& inBodyID2,
	const SubShapeID& inSubShapeID2)
{

	// もし水中に
	if (PhysicsManager::Instance().GetBodyInterface().GetObjectLayer(inBodyID2) == Layers::WATER_SENSOR)
	{
		m_isInWater = false;
		DebugLog::Log("[PlayerContactListener] Character gets out of water now");
		return;
	}
		
}
