#include "Item.h"

#include "Player.h"

namespace 
{
	static constexpr int FoodMaxStack = 10;
	static constexpr int BasicMaterialMaxStack = 20; //ツールはスタックしない
	
}
Item::Item(ItemType _type, bool stackable, int maxStack, float maxDurability):
	m_itemType(_type),
	m_isStackable(stackable),
	m_maxStack(maxStack),
	m_maxDurability(maxDurability)
{
}

ItemInstance::ItemInstance():
	GameObject(GameObjectType::Item)
{
}

//=======Item Instance
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

void ItemInstance::Update(float dt)
{
	// Set Item object
	switch (m_itemState)
	{
	case Active:break;
	case InActive: DeActivate(); break;
	case Collected: DeActivate(); break;
	}
	GameObject::Update(dt);
}

//========BuildTool===========
Utility::Utility():
	Item(ItemType::Utility,false,0,0)
{
}


void Utility::OnUse(Player* player) const
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

void Food::OnUse(Player* player) const
{
	player->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->RestoreHunger(m_foodValue);
}

//=========BaseMaterial============
BaseMaterial::BaseMaterial():
	Item(ItemType::BaseMaterial)
{
}

void BaseMaterial::OnUse(Player* player) const
{
	/* No action on use */
}



