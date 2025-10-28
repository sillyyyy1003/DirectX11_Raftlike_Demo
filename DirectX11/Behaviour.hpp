#pragma once
#include "PhysicsComponent.h"

class Player;
/// Eat behavior interface 


class Interactable
{
public:
	virtual void InteractWith(BodyID& rigidBody, Player* player) = 0;
	virtual void OnUse(Player* player) = 0;
	virtual void OnUseRelease(Player* player) = 0;
	virtual void OnUseHold(Player* player,float deltaTime) = 0;
	virtual void OnUseStart(Player* player) = 0;
	virtual ~Interactable() = default;

	virtual bool HasCharge() { return false; };
	virtual float GetChargeProgress() { return 0.0f; }
};