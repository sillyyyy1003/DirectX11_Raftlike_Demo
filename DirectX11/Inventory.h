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

	/// @brief Consume Item from inventory for player
	/// @param index 
	/// @param player 
	/// @param consumeCount 
	void ConsumeItem(int index, Player* player, int consumeCount = 1);

	/// @brief Remove Item from inventory
	/// @param index index of item in inventory
	/// @param count 
	/// @return 
	bool RemoveItem(int index, int count);

	/// @brief Check inventory has enough item or not
	/// @param itemName key to check slots match or not
	/// @param count item number needed
	/// @return true>> has enough item(maybe in different slots), false>> 
	bool HasEnoughItem(std::string& itemName, int count);

	/// @brief Remove Item needed in inventory
	/// @param itemName key to check slots match or not
	/// @param count remove number
	/// @return true>>successfully removed
	bool RemoveItem(const std::string& itemName, int count);

	ItemInstance* GetCurrentItem(int index) const;
	void UpdateItemOfPlayer(int index, Player* player);

private:

	std::vector<Slot> m_slots;
};

