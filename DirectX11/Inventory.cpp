#include "Inventory.h"
#include "DebugLog.h"
#include "KInput.h"

Inventory::Inventory(int maxSlots):
	m_slots(maxSlots)
{
}

int Inventory::Insert(ItemInstance* instance)
{
	// error check if instance is void or has count <0 
	if (!instance || instance->GetCount() <= 0 || !instance->GetProto())
		return 0;

	auto proto = instance->GetProto();
	const int maxStack = proto->GetMaxStack();
	int remaining = instance->GetCount();
	int inserted = 0;

	// insert item to stackable slots
	if(instance->IsStackable())
	{
		for(auto& slot:m_slots)
		{
			if (slot && (*slot)->IsStackable()	// object is stackable
				&& (*slot)->GetProto() == proto	// has same object	
				&& (*slot)->GetCount() < maxStack)	//slot is not fully stacked
			{
				int canAdd = maxStack - (*slot)->GetCount();	
				if (canAdd > 0)
				{
					int toAdd = std::min(remaining, canAdd);// Calculate add number
					(*slot)->AddCount(toAdd);	// Add count
					remaining -= toAdd;	// Calculate remain
					inserted += toAdd;	// Calculate insert num
					if (remaining == 0) return inserted;
				}
			}
		}
	}

	for (auto& slot : m_slots)
	{
		if (!slot.has_value())
		{
			int toInsert = std::min(remaining, maxStack);
			ItemPtr newInstance = std::make_shared<ItemInstance>();
			newInstance->InitItem(instance->GetProto(), toInsert, instance->GetDurability());
			slot = newInstance;

			remaining -= toInsert;
			inserted += toInsert;
			if (remaining == 0) return inserted;
		}
	}

	if (remaining > 0)
	{
		DebugLog::Log("[Inventory]] Inventory full, cannot insert all items. Remaining: " + std::to_string(remaining));
	}

	return inserted;
}

size_t Inventory::GetUsedSlotCount() const
{
	size_t count = 0;
	for (const auto& slot : m_slots)
	{
		if (slot.has_value())  // this slot is stacked
			++count;
	}
	return count;
}

size_t  Inventory::GetMaxSlots() const
{
	return m_slots.size();
}

void Inventory::Update(float tick)
{

#ifdef _DEBUG
	// Output all objects in inventory
	if(KInput::IsKeyTrigger(VK_F11))
	{
		DebugLog::Log("[Inventory] Used slots : {}", GetUsedSlotCount());
		for (size_t i = 0; i < GetUsedSlotCount(); i++)
		{

			ItemPtr item = m_slots[i].value();
			size_t index = i;

			std::string name = item->GetName();
			DebugLog::Log("[Inventory] Slot {}: Name: {}, Number: {}.", i, name, item->GetCount());
		}
	}
#endif
}

void Inventory::UseItem(int index, Player* player)
{
	if (!m_slots[index].has_value())return;

	m_slots[index].value()->GetProto()->OnUse(player);	// Call OnUse function of item

	// if item is stackable, decrease count
	m_slots[index].value()->DecreaseCount(1);	// Decrease count by 1

	// if count is 0, remove item from slot
	if (m_slots[index].value()->GetCount() <= 0)
	{
		m_slots[index].reset();	// Remove item from slot
	}
}

void Inventory::ConsumeItem(int index, Player* player, int consumeCount)
{
	if (!m_slots[index].has_value())return;

	switch(m_slots[index].value()->GetProto()->GetItemType())
	{
	default:
		m_slots[index].value()->GetProto()->OnUse(player);
	case Item::ItemType::Food:
		auto proto = m_slots[index].value()->GetProto(); // shared_ptr<const Item>
		auto food = dynamic_cast<const Food*>(proto.get()); // 注意是 const Food*
		if (food)
		{
			food->OnEat(player);
		}
		break;
		//todo: more to set...
	}


	// if item is stackable, decrease count
	m_slots[index].value()->DecreaseCount(consumeCount);	// Decrease count by 1

	// if count is 0, remove item from slot
	if (m_slots[index].value()->GetCount() <= 0)
	{
		m_slots[index].reset();	// Remove item from slot
	}
}


