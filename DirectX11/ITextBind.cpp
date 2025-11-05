#include "ITextBind.h"
#include "Player.h"
#include "Component.hpp"
#include "HungerComponent.h"
#include "Inventory.h"

std::string UIFormat::FormatHunger(Player* player)
{
	return "HP: " + std::to_string(player->GetComponent<HungerComponent>()->GetCurrentHungerValue());

}

std::string UIFormat::FormatItemNumber(Inventory* inventory, int slotIndex)
{
	// slotIndexが無効な場合は空文字列を返す
	if (slotIndex < 0 || slotIndex >= inventory->GetMaxSlots())
	{
		return "";
	}

	auto& slot = inventory->GetSlots()[slotIndex];
	if (!slot.has_value() || !(*slot))
	{
		return "";	// スロットが空の場合は空文字列を返す
	}

	return std::to_string((*slot)->GetCount());
}


