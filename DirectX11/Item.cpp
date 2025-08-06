#include "Item.h"

namespace 
{
	static constexpr int FoodMaxStack = 20;
	
}
Item::Item(ItemType _type, bool stackable, int maxStack, float maxDurability):
	m_itemType(_type),
	m_isStackable(stackable),
	m_maxStack(maxStack),
	m_maxDurability(maxDurability)
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

//========BuildTool===========
Builder::Builder():
	Item(ItemType::Builder,false,0,0)
{
}

Food::Food(float _foodValue) :
	Item(ItemType::Food, 
		true,
		FoodMaxStack,
		1.f)// 一回だけ
{
	m_foodValue = _foodValue;
}

