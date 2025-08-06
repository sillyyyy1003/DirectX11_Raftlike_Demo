#pragma once
#include <optional>
#include <vector>
#include "Item.h"

class Item;
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

	void Update(float tick);
private:

	std::vector<Slot> m_slots;


};

