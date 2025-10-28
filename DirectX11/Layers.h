#pragma once
#include <Jolt.h>
#include <Physics/Body/BodyActivationListener.h>
#include <Physics/Character/CharacterVirtual.h>
#include <Physics/Collision/ContactListener.h>
#include <Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include "BuoyancySystem.h"
#include "DebugLog.h"

using namespace JPH;

namespace Layers
{
	static constexpr ObjectLayer BOAT = 0;  // 船
	static constexpr ObjectLayer ENEMY = 1;  // 敵
	static constexpr ObjectLayer PLAYER = 2;  // プレイヤー
	static constexpr ObjectLayer BUILDING = 3;  // 建物
	static constexpr ObjectLayer DRIFT = 4;  // 漂流物
	static constexpr ObjectLayer ITEM = 5;  // Drop
	static constexpr ObjectLayer TOOL = 6;  // ツール only for hook
	static constexpr ObjectLayer WEAPON = 7;  // 武器
	static constexpr ObjectLayer WATER_SENSOR = 8;  // 水センサー
	static constexpr ObjectLayer NUM_LAYERS = 9;
};

namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer STATIC(0);     // 建築
	static constexpr BroadPhaseLayer ACTOR(1);      // プレイヤー・敵
	static constexpr BroadPhaseLayer BOAT(2);       // 船
	static constexpr BroadPhaseLayer DYNAMIC(3);    // Drop・漂流物

	static constexpr BroadPhaseLayer PROJECTILE(4); // 武器
	static constexpr BroadPhaseLayer SENSOR(5);     // 水
	static constexpr BroadPhaseLayer KINEMATIC(6);  // HOOK/ツール
	static constexpr uint NUM_LAYERS(7);
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
			return inObject2 == Layers::ENEMY ||
				inObject2 == Layers::BOAT ||
				inObject2 == Layers::BUILDING ||
				inObject2 == Layers::ITEM||
				inObject2 == Layers::WATER_SENSOR;

		case Layers::ENEMY:
			return inObject2 == Layers::PLAYER ||
				inObject2 == Layers::BOAT ||
				inObject2 == Layers::WATER_SENSOR;

		case Layers::BOAT:
			return inObject2 == Layers::PLAYER ||
				inObject2 == Layers::ENEMY ||
				inObject2 == Layers::TOOL ||
				inObject2 == Layers::WATER_SENSOR;

		case Layers::BUILDING:
			return inObject2 == Layers::PLAYER ||
				inObject2 == Layers::TOOL;

		case Layers::DRIFT:
			return inObject2 == Layers::WATER_SENSOR ||
				inObject2 == Layers::TOOL;

		case Layers::ITEM:
			return inObject2 == Layers::BOAT ||
				inObject2 == Layers::WATER_SENSOR|| inObject2 == Layers::PLAYER;

		case Layers::TOOL:
			return inObject2 == Layers::DRIFT ||
				inObject2 == Layers::BOAT ||
				inObject2 == Layers::BUILDING; /*||
				inObject2 == Layers::WATER_SENSOR;*/

		case Layers::WATER_SENSOR:
			return inObject2 == Layers::PLAYER ||
				inObject2 == Layers::ENEMY ||
				inObject2 == Layers::BOAT ||
				inObject2 == Layers::DRIFT ||
				inObject2 == Layers::ITEM; /*||
				inObject2 == Layers::TOOL;*/

		case Layers::WEAPON:
			return inObject2 == Layers::ENEMY;

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
		m_objectToBroadPhase[Layers::PLAYER] = BroadPhaseLayers::ACTOR;
		m_objectToBroadPhase[Layers::ENEMY] = BroadPhaseLayers::ACTOR;
		m_objectToBroadPhase[Layers::BOAT] = BroadPhaseLayers::BOAT;
		m_objectToBroadPhase[Layers::BUILDING] = BroadPhaseLayers::STATIC;
		m_objectToBroadPhase[Layers::DRIFT] = BroadPhaseLayers::DYNAMIC;
		m_objectToBroadPhase[Layers::ITEM] = BroadPhaseLayers::DYNAMIC;
		m_objectToBroadPhase[Layers::TOOL] = BroadPhaseLayers::KINEMATIC;
		m_objectToBroadPhase[Layers::WATER_SENSOR] = BroadPhaseLayers::SENSOR;
		m_objectToBroadPhase[Layers::WEAPON] = BroadPhaseLayers::PROJECTILE;
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
		case (BroadPhaseLayer::Type)BroadPhaseLayers::STATIC:      return "STATIC";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::ACTOR:       return "ACTOR";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::BOAT:        return "BOAT";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::DYNAMIC:     return "DYNAMIC";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::PROJECTILE:  return "PROJECTILE";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::SENSOR:      return "SENSOR";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::KINEMATIC:      return "KINEMATIC";
		default: JPH_ASSERT(false); return "INVALID";
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
			return inLayer2 == BroadPhaseLayers::ACTOR ||
				inLayer2 == BroadPhaseLayers::BOAT ||
				inLayer2 == BroadPhaseLayers::STATIC ||
				inLayer2 == BroadPhaseLayers::SENSOR ||
				inLayer2 == BroadPhaseLayers::DYNAMIC;

		case Layers::ENEMY:
			return inLayer2 == BroadPhaseLayers::ACTOR ||
				inLayer2 == BroadPhaseLayers::BOAT ||
				inLayer2 == BroadPhaseLayers::SENSOR;

		case Layers::BOAT:
			return inLayer2 == BroadPhaseLayers::ACTOR ||
				inLayer2 == BroadPhaseLayers::DYNAMIC ||
				inLayer2 == BroadPhaseLayers::SENSOR;

		case Layers::BUILDING:
			return inLayer2 == BroadPhaseLayers::ACTOR ||
				inLayer2 == BroadPhaseLayers::DYNAMIC;

		case Layers::DRIFT:
			return inLayer2 == BroadPhaseLayers::SENSOR ||
				inLayer2 == BroadPhaseLayers::DYNAMIC ||
				inLayer2 == BroadPhaseLayers::KINEMATIC;

		case Layers::ITEM:
			return inLayer2 == BroadPhaseLayers::BOAT ||
				inLayer2 == BroadPhaseLayers::SENSOR||
				inLayer2==BroadPhaseLayers::ACTOR;

		case Layers::TOOL:
			return inLayer2 == BroadPhaseLayers::DYNAMIC ||
				inLayer2 == BroadPhaseLayers::STATIC;

		case Layers::WATER_SENSOR:
			return inLayer2 == BroadPhaseLayers::ACTOR ||
				inLayer2 == BroadPhaseLayers::BOAT ||
				inLayer2 == BroadPhaseLayers::DYNAMIC;

		case Layers::WEAPON:
			return inLayer2 == BroadPhaseLayers::ACTOR;

		default:
			JPH_ASSERT(false);
			return false;
        }
	}
};

class ContactListenerDispatcher : public ContactListener
{
public:
	void AddListener(ContactListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void OnContactAdded(const Body& inBody1, const Body& inBody2,
		const ContactManifold& inManifold,
		ContactSettings& ioSettings) override
	{
		for (auto* listener : m_listeners)
		{
			listener->OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
		}
	}

	void OnContactPersisted(const Body& inBody1, const Body& inBody2,
		const ContactManifold& inManifold,
		ContactSettings& ioSettings) override
	{
		for (auto* listener : m_listeners)
		{
			listener->OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
		}
	}

	void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		for (auto* listener : m_listeners)
		{
			listener->OnContactRemoved(inSubShapePair);
		}
	}

private:
	std::vector<ContactListener*> m_listeners;
};



class ObjectContactListener : public ContactListener
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
		//DebugLog::Log("Contact validate callback\n");
		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	/// @brief OnContactAdded is called when a contact is added between two bodies.
	/// @param inBody1 
	/// @param inBody2 
	/// @param inManifold 
	/// @param ioSettings 
	virtual void			OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;

	/// @brief OnContactPersisted is called when a contact is persisted between two bodies.
	/// @param inBody1 
	/// @param inBody2 
	/// @param inManifold 
	/// @param ioSettings 
	virtual void			OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;

	/// @brief is called when a contact is removed between two bodies. OnCollisionExit
	/// @param inSubShapePair 
	virtual void			OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		
		DebugLog::Log("[Physics] A contact was removed.");

		//========ここでOnCollisionExitのロジックを追加する
	}
};

class ExcludeLayerFilter : public ObjectLayerFilter
{
public:
	explicit ExcludeLayerFilter()
	{
	}

	virtual bool ShouldCollide(ObjectLayer inLayer) const override
	{
		return inLayer != Layers::PLAYER;
	}

};



class MyPlayerContactListener:
	public CharacterContactListener
{
public:
	bool GetInWater()const { return m_isInWater; }

	// Called whenever the character collides with a body.
	virtual void			OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

	// Called whenever the character persists colliding with a body.
	virtual void			OnContactPersisted(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override
	{
		
	}

	// Called whenever the character loses contact with a body.
	virtual void			OnContactRemoved(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2) override;

protected:
	// Common function to be called when contacts are added/persisted
	void					OnContactCommon(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
	{
		
	}

private:
	bool m_isInWater = false;


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
		//DebugLog::Log("[Physics] A body got activated.");
		
	}

	virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		//cout << "A body went to sleep" << endl;
		//DebugLog::Log("[Physics] A body went to sleep.");
	}
};
