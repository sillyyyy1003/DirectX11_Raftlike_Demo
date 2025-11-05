#include "Item.h"

#include "Player.h"

namespace 
{
	static constexpr int FoodMaxStack = 10;
	static constexpr float SaltyWaterScaler = -0.5f;
	
}
Item::Item(ItemType _type, bool stackable, int maxStack, float maxDurability):
	m_itemType(_type),
	m_isStackable(stackable),
	m_maxStack(maxStack),
	m_maxDurability(maxDurability)
{
}

//=======Item Instance
ItemInstance::ItemInstance():
	GameObject(GameObjectType::Item)
{
}

void ItemInstance::InitItem(std::shared_ptr<const Item> _proto, int _count, float _durability)
{
	m_protoPtr = _proto;
	m_count = _count;
	if (_proto && _proto->HasDurability())
		m_durability = (_durability == -1) ? m_protoPtr->GetMaxDurability() : _durability;

}

void ItemInstance::DecreaseCount(int count)
{
	if (count <= 0) return;
	m_count -= count;
	if (m_count < 0) m_count = 0; // Ensure count does not go below zero
}

void ItemInstance::SetState(int state)
{
	m_objectState = state;
}


void ItemInstance::Update(float dt)
{
	GameObject::Update(dt);
}

//========BuildTool===========
Utility::Utility():
	Item(ItemType::Utility,false,0,0)
{
}



//========Food===========
Food::Food(float _foodValue) :
	Item(ItemType::Food, 
		true,
		FoodMaxStack,
		1.f)// 一回だけ
{
	m_foodValue = _foodValue;
}


//=========BaseMaterial============
BaseMaterial::BaseMaterial():
	Item(ItemType::BaseMaterial)
{
}



//=========Cup============
Cup::Cup(float value):
	Item(ItemType::Cup, false, 1, -1),
	m_recoverValue(value)
{
}

//=========WaterPurifier============
WaterPurifier::WaterPurifier(float timeThreshold):
	Item(ItemType::WaterPurifier, false, 1, -1),
	m_timeThreshold(timeThreshold)
{
}

//==========Spear============
Spear::Spear(float damage, float durability):
	Item(ItemType::Weapon, false,1,durability),
	m_damage(damage)
{
}

//==========Hook=============
Hook::Hook(float maxSpeed, float minSpeed,  float chargeTime, float chargeSpeed, float durability):
	Item(ItemType::Hook,false, 1, durability),
	m_maxSpeed(maxSpeed),
	m_minSpeed(minSpeed),
	m_chargeTimeLimit(chargeTime),
	m_chargeSpeed(chargeSpeed)
{
}

//==========Loot=============
Loot::Loot(int maxItemCount ,const std::vector<std::string>& items):
	Item(ItemType::Loot,false,1,-1),
	m_maxItemCount(maxItemCount)
{
	m_items = items;
}


