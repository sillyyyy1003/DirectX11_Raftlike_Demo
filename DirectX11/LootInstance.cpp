#include "LootInstance.h"
#include "ItemDataBase.h"
#include "RandomManager.h"

namespace 
{
	static constexpr float RareRate = 0.2f;
	static constexpr float BaseRate = 0.5f;
	static constexpr int MinBaseItemCount = 3;	// 天井
}

void LootInstance::InitItem(std::shared_ptr<const Item> _proto, int _count, float _durability)
{
	ItemInstance::InitItem(_proto, _count, _durability);

	const Loot* lootPtr = dynamic_cast<const Loot*>(GetProto().get());
	int maxCount = lootPtr->GetMaxCount();
	auto& rng = RandomManager::Instance().GetGenerator("Loot");
	const auto& itemTypes = lootPtr->GetItemTypes();
	if (itemTypes.empty()) return;

	std::vector<std::string> baseItems;
	std::vector<std::string> rareItems;

	// divide item to base material & rare items
	for (const auto& itemName : itemTypes)
	{
		auto itemPtr = ItemDataBase::Instance().GetItem(itemName.c_str());
		if (!itemPtr) continue;

		if (itemPtr->GetItemType() == Item::ItemType::BaseMaterial)
			baseItems.push_back(itemName);
		else
			rareItems.push_back(itemName);
	}

	//==========Base material->At least generate 1
	bool hasBaseGenerated = false;
	for (const auto& itemName : baseItems)
	{
		if (rng.Chance(BaseRate))
		{
			int itemCount = rng.GetRandomInt(1, maxCount);
			m_lootContent.emplace(itemName, itemCount);
			hasBaseGenerated = true;
		}
	}

	// doesn't generate any items, forced to genrate one
	if (!hasBaseGenerated && !baseItems.empty())
	{
		int forcedIndex = rng.GetRandomInt(0, static_cast<int>(baseItems.size()) - 1);
		int itemCount = rng.GetRandomInt(MinBaseItemCount, maxCount);
		m_lootContent.emplace(baseItems[forcedIndex], itemCount);
	}

	//==========Rare items
	for (const auto& itemName : rareItems)
	{
		if (rng.Chance(RareRate))
		{
			int itemCount = rng.GetRandomInt(1, maxCount);
			m_lootContent.emplace(itemName, itemCount);
		}
	}
}
