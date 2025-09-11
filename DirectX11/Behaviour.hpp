#pragma once
#include "PhysicsComponent.h"

class Player;
/// Eat behavior interface 
class IEatBehavior
{
public:
	virtual void OnEat(Player* player) const = 0;
	virtual ~IEatBehavior() = default;
};

class IToolBehavior
{
public:
	virtual void OnUse(Player* player)= 0;
	virtual ~IToolBehavior() = default;
};

class Interactable
{
public:
	virtual void InteractWith(BodyID& rigidBody, Player* player) = 0;
	virtual ~Interactable() = default;
};