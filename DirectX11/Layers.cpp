#include "Layers.h"

#include "PhysicsManager.h"

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
