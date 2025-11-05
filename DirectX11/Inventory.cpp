#include "Inventory.h"
#include "DebugLog.h"
#include "ItemDataBase.h"
#include "ItemInstances.h"
#include "KInput.h"
#include "LootInstance.h"
#include "Player.h"

Inventory::Inventory(int maxSlots):
	m_slots(maxSlots)
{
}

int Inventory::Insert(ItemInstance* instance)
{
	// bug check
	if (!instance || instance->GetCount() <= 0 || !instance->GetProto())
		return 0;

	auto proto = instance->GetProto();

	// if is loot item
	if (proto->GetItemType() == Item::ItemType::Loot)
	{
		int insertedTotal = 0;

		LootInstance* loot = dynamic_cast<LootInstance*>(instance);
		if (!loot)
		{
			DebugLog::Log("[Inventory] Invalid LootInstance cast");
			return 0;
		}

		const auto& lootContent = loot->GetLootContent();
		if (lootContent.empty())
		{
			DebugLog::Log("[Inventory] LootInstance is empty");
			return 0;
		}

		for (const auto& [itemName, itemCount] : lootContent)
		{
			auto itemProto = ItemDataBase::Instance().GetItem(itemName.c_str());
			if (!itemProto)
			{
				DebugLog::Log("[Inventory] Unknown item in Loot: " + itemName);
				continue;
			}

			// Create item instance
			ItemPtr itemInstance = ItemDataBase::Instance().CreateItemInstance(itemName.c_str(), itemCount);
			if (!itemInstance)
			{
				DebugLog::Log("[Inventory] Failed to create item instance for: " + itemName);
				continue;
			}

			// insert single item 
			int inserted = InsertSingleItem(itemInstance.get());
			insertedTotal += inserted;

			if (inserted < itemCount)
			{
				DebugLog::Log("[Inventory] Partial insert for {}. Inserted: {}/{}", itemName, std::to_string(inserted), std::to_string(itemCount));
			}
		}

		return 1;	// destroy loot item any way;
	}

	// regular item
	return InsertSingleItem(instance);
}

int Inventory::InsertSingleItem(ItemInstance* instance)
{
	if (!instance || instance->GetCount() <= 0 || !instance->GetProto())
		return 0;
	auto proto = instance->GetProto();
	const int maxStack = proto->GetMaxStack();
	int remaining = instance->GetCount();
	int inserted = 0;

	// insert item to stackable slots
	if (instance->IsStackable())
	{
		for (auto& slot : m_slots)
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
			ItemPtr newInstance = ItemDataBase::Instance().CreateItemInstance(instance->GetName().c_str(), toInsert);
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


void Inventory::ConsumeItem(int index, Player* player, int consumeCount)
{
	
}

bool Inventory::RemoveItem(int index, int count)
{
	if (!m_slots[index].has_value()) return false;

	m_slots[index].value()->DecreaseCount(count);
	if (m_slots[index].value()->GetCount() <= 0)
	{
		m_slots[index].reset();
	}
	return true;
}

bool Inventory::RemoveCurrentSlotItem(int count)
{
	if (!m_slots[m_currentIndex].has_value()) return false;

	m_slots[m_currentIndex].value()->DecreaseCount(count);
	if (m_slots[m_currentIndex].value()->GetCount() <= 0)
	{
		m_slots[m_currentIndex].reset();
	}
	return true;
}

bool Inventory::HasEnoughItem(std::string& itemName, int count)
{
	int total = 0;
	for (const auto& slot : m_slots)
	{
		if (slot.has_value() && // has item
			slot.value()) //item is not nullptr
		{
			auto proto = slot.value()->GetProto();// Get item proto

			if (proto && proto->GetName() == itemName)	//if proto is not nullptr && itemName match
			{
				total += slot.value()->GetCount();	// calculate slot count is enough for needing
				if (total >= count)	//if is enough, end loop ;
				{
					return true;
				}
			}
		}
	}
	return false;	// inventory doesn't have enough item for needing
}

bool Inventory::RemoveItem(const std::string& itemName, int count)
{
	int remaining = count;
	for (auto& slot : m_slots)
	{
		if (slot.has_value() && // has item
			slot.value()) //item is not nullptr
		{
			auto proto = slot.value()->GetProto();// Get item proto
			if (proto && proto->GetName() == itemName)//if proto is not nullptr && itemName match
			{
				int count = slot.value()->GetCount();
				if (count <= remaining)
				{
					remaining -= count;
					slot.reset(); // clear slot
				}
				else
				{
					slot.value()->DecreaseCount(remaining);
					return true;
				}
				if (remaining <= 0) return true;
			}
		}
	}
	return remaining <= 0;	
}


ItemInstance* Inventory::GetCurrentItem(int index) const
{
	if (m_slots[index].has_value() && m_slots[index].value())
		return m_slots[index].value().get();
	return nullptr;
}


void Inventory::UpdateItemOfPlayer(Player* player)
{
	if (m_slots[m_currentIndex].has_value() && m_slots[m_currentIndex].value())
	{
		player->SetItemInHand(m_slots[m_currentIndex].value().get(),m_pItemEffect);
	}
	else
	{
		player->SetItemInHand(nullptr,nullptr);
	}
}

int Inventory::GetTotalCount(std::string& itemName)
{
	int total = 0;
	for (const auto& slot : m_slots)
	{
		if (slot.has_value() && // has item
			slot.value()) //item is not nullptr
		{
			auto proto = slot.value()->GetProto();// Get item proto

			if (proto && proto->GetName() == itemName)	//if proto is not nullptr && itemName match
			{
				total += slot.value()->GetCount();	// calculate slot count is enough 
			}
		}
	}

	return total;
}


