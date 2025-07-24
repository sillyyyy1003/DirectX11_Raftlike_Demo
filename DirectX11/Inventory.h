#pragma once
#include <optional>
#include <vector>
#include "Item.h"

class Item;
/// @brief プレイヤーの持ち物を管理するクラス 
class Inventory:
	public Component
{
public:
	Inventory();
	~Inventory() override = default;

private:

	typedef std::vector<std::optional<ItemInstance>> Slots;
	Slots m_slots;

};

