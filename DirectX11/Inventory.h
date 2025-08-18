#pragma once
#include <optional>
#include <vector>
#include "Item.h"

/// @brief プレイヤーの持ち物を管理するクラス 
class Inventory:
	public Component
{
	using ItemPtr = std::shared_ptr<ItemInstance>;
	using Slot = std::optional<ItemPtr>;

public:
	Inventory(int maxSlots);
	~Inventory() override = default;

	/// @brief Insert item to player's inventory
	/// @param instance 挿入したいアイテムインスタンス
	/// @return 挿入した数
	int Insert(ItemInstance* instance);

	size_t GetUsedSlotCount() const;
	size_t GetMaxSlots() const;
	std::vector<Slot>& GetSlots() { return m_slots; }

	void Update(float tick);
	void UseItem(int index, Player* player);

private:

	std::vector<Slot> m_slots;
};

