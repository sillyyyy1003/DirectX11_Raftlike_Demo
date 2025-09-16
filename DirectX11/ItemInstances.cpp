#include "ItemInstances.h"

#include <Physics/PhysicsSystem.h>

#include "AudioManager.h"
#include "HungerComponent.h"
#include "PhysicsManager.h"
#include "Player.h"
namespace
{
	static constexpr float SaltyScaler = -0.5f; // 塩水の副作用
}

void FoodInstance::InteractWith(BodyID& rigidBody, Player* player)
{
	// food will interact with kitch
	// Check rigid body is building layer or not. Because food only interactive with kitchen which is in building layer
	
	
}

void FoodInstance::OnUse(Player* player)
{
	float recoverValue = dynamic_cast<const Food*>(GetProto().get())->GetFoodValue();
	player->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->RestoreHunger(recoverValue);
#ifdef _DEBUG
	DebugLog::Log("Eat food");
#endif
}

CupInstance::CupInstance()
{
}

void CupInstance::OnUse(Player* player)
{
	float recoverValue = dynamic_cast<const Cup*>(GetProto().get())->GetRecoverValue();
	switch (m_cupState)
	{
	case SaltyWater:
		recoverValue *= SaltyScaler;	// 塩水の時逆効果
		m_cupState = Empty;
#ifdef _DEBUG
		DebugLog::Log("Drink salty water");
#endif
		AudioManager::Instance().Play("SE_Drink", false);
		break;
	case FreshWater:
		m_cupState = Empty;
#ifdef _DEBUG
		DebugLog::Log("Drink fresh water");
#endif
		AudioManager::Instance().Play("SE_Drink", false);
		break;
	case Empty:
		recoverValue = 0.f;
		break;
	}

	player->GetComponent<ThirstComponent>(MyComponent::ComponentType::Thirst)->RestoreThirst(recoverValue);

}
void CupInstance::InteractWith(BodyID& rigidBody, Player* player)
{

	if (PhysicsManager::Instance().GetBodyInterface().GetObjectLayer(rigidBody) == Layers::WATER_SENSOR)
	{
		if (m_cupState == Empty)
		{
			DebugLog::Log("Get Salty water");
			m_cupState = SaltyWater;// if cup is empty , fill with salty water. Else do nothing
		}
		return;
	}

	// Check rigid body is building layer or not. Because cup only interactive with water purifier which is in building layer
	PhysicsComponent* component = PhysicsManager::Instance().GetPhysicsComponent(rigidBody);
	if (component != nullptr)
	{
		GameObject* object = component->GetGameObject();
		if (dynamic_cast<ItemInstance*>(object)->GetProto()->GetItemType()==Item::ItemType::WaterPurifier)
		{
			dynamic_cast<PurifierInstance*>(object)->InteractWithCup(this);
			return;
		}
	}


}

void PurifierInstance::InteractWithCup(CupInstance* cup)
{
	switch (m_waterPurifierState)
	{
	case Idle:
		if (cup->GetCupState() == CupInstance::SaltyWater)
		{
			m_waterPurifierState = Purifying;
			m_timer = 0;
			cup->SetCupState(CupInstance::Empty);
		}
		break;
	case Purifying:
		// do nothing
		break;
	case Finished:
		if (cup->GetCupState() == CupInstance::Empty)
		{
			cup->SetCupState(CupInstance::FreshWater);
			m_waterPurifierState = Idle;
		}
		break;
	}

}

void PurifierInstance::Update(float dt)
{
	ItemInstance::Update(dt);

	// if is purifying increase timer
	if (m_waterPurifierState==Purifying)
	{
		m_timer += dt;

		// check if timer exceed threshold
		float timeThreshold = dynamic_cast<const WaterPurifier*>(GetProto().get())->GetPurifyThreshold();
		// if exceed finish timer
		if (m_timer > timeThreshold)
			m_waterPurifierState = Finished;
	}
}
