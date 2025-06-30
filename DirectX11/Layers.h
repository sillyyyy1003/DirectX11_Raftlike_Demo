#pragma once
#include <Jolt.h>
#include <Physics/Body/BodyActivationListener.h>
#include <Physics/Collision/ContactListener.h>
#include <Physics/Collision/BroadPhase/BroadPhaseLayer.h>

#include "DebugLog.h"
using namespace JPH;

namespace Layers
{
	static constexpr ObjectLayer PLAYER = 0;
	static constexpr ObjectLayer ENEMY = 1;
	static constexpr ObjectLayer WEAPON = 2;
	static constexpr ObjectLayer ITEM = 3;
	static constexpr ObjectLayer BOAT = 4;
	static constexpr ObjectLayer BUILDING = 5;
	static constexpr ObjectLayer TOOL = 6;	//Hook


	//static constexpr ObjectLayer ISLAND = 5;
	static constexpr ObjectLayer NUM_LAYERS = 7;
};

namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::PLAYER:
			return inObject2 == Layers::ENEMY || inObject2 == Layers::ITEM ||
				inObject2 == Layers::BOAT || inObject2 == Layers::BUILDING;	// Player collides with everything except Weapon (to avoid self-collision with thrown weapons)
		case Layers::ENEMY:
			return inObject2 == Layers::PLAYER || inObject2 == Layers::BOAT || inObject2 == Layers::WEAPON; //Enemy collides with Boat Player, Building, Weapon, Boat
		case Layers::WEAPON:
			return inObject2 == Layers::ENEMY;
		case Layers::ITEM:
			return inObject2 == Layers::PLAYER || inObject2 == Layers::BOAT || inObject2 == Layers::BUILDING|| inObject2==Layers::TOOL;
			// Item collides with Player and boat, Building, Tool
		case Layers::BOAT:
			return inObject2 == Layers::PLAYER || inObject2 == Layers::ENEMY ||
				inObject2 == Layers::ITEM || inObject2 == Layers::BUILDING;// Boat collides with everything except weapon
		case Layers::BUILDING:
			return inObject2 == Layers::PLAYER || inObject2 == Layers::BOAT||inObject2==Layers::ITEM; // Building collides with Player, Enemy, Boat
		case Layers::TOOL:
			return inObject2==Layers::ITEM;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};



class BPLayerInterfaceImpl final:public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		m_objectToBroadPhase[Layers::PLAYER] = BroadPhaseLayers::MOVING;
		m_objectToBroadPhase[Layers::ENEMY] = BroadPhaseLayers::MOVING;
		m_objectToBroadPhase[Layers::WEAPON] = BroadPhaseLayers::MOVING;
		m_objectToBroadPhase[Layers::ITEM] = BroadPhaseLayers::MOVING;
		m_objectToBroadPhase[Layers::BOAT] = BroadPhaseLayers::NON_MOVING;  //船は動かないので非動的に設定
		m_objectToBroadPhase[Layers::BUILDING] = BroadPhaseLayers::NON_MOVING; //建物も動かないので非動的に設定
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual  BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return m_objectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED


private:
	BroadPhaseLayer m_objectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::PLAYER:
		case Layers::ENEMY:
		case Layers::WEAPON:
		case Layers::BOAT:
			return true;
		case Layers::ITEM:
		case Layers::TOOL:
			return inLayer2 == BroadPhaseLayers::NON_MOVING;
		case Layers::BUILDING:
			return false;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};


class MyContactListener : public ContactListener
{
public:
	// See: ContactListener
	/// @brief a cheaper way to not collide two objects, but if you want to ignore a contact before it is created, you can use this callback.
	/// @param inBody1 
	/// @param inBody2 
	/// @param inBaseOffset 
	/// @param inCollisionResult 
	/// @return 
	virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
	{
		//cout << "Contact validate callback" << endl;
		DebugLog::Log("Contact validate callback\n");
		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	/// @brief OnContactAdded is called when a contact is added between two bodies.
	/// @param inBody1 
	/// @param inBody2 
	/// @param inManifold 
	/// @param ioSettings 
	virtual void			OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		//cout << "A contact was added" << endl;
		DebugLog::Log("[Physics] A contact was added.");
		//========ここでOnCollisionEnterのロジックを追加する

	}

	/// @brief OnContactPersisted is called when a contact is persisted between two bodies.
	/// @param inBody1 
	/// @param inBody2 
	/// @param inManifold 
	/// @param ioSettings 
	virtual void			OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		DebugLog::Log("[Physics] A contact was persisted.");
		//cout << "A contact was persisted" << endl;
		//========ここでOnCollisionStayのロジックを追加する
		


	}

	/// @brief is called when a contact is removed between two bodies. OnCollisionExit
	/// @param inSubShapePair 
	virtual void			OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		
		DebugLog::Log("[Physics] A contact was removed.");
		//cout << "A contact was removed" << endl;
		//========ここでOnCollisionExitのロジックを追加する
	}
};

// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
	/// @brief OnEnabled is called when a body is enabled in the world.
	/// @param inBodyID 
	/// @param inBodyUserData 
	virtual void		OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		//cout << "A body got activated" << endl;
		DebugLog::Log("[Physics] A body got activated.");
		
	}

	virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		//cout << "A body went to sleep" << endl;
		DebugLog::Log("[Physics] A body went to sleep.");
	}
};
