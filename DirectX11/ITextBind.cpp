#include "ITextBind.h"
#include "Player.h"
#include "Component.hpp"
#include "HungerComponent.h"

std::string UIFormat::FormatHunger(Player* player)
{
	return "HP: " + std::to_string(player->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->GetCurrentHungerValue());

}
