#include "Inventory.h"

Inventory::Inventory(int maxSlots):
	m_slots(maxSlots)
{
}

int Inventory::Insert(std::shared_ptr<ItemInstance> instance)
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
			//newInstance->SetPresence(ScenePresence::InInventory);
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


