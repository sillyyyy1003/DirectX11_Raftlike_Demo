#pragma once

class Player;
/// Eat behavior interface 
class IEatBehavior
{
public:
	virtual void OnEat(Player* player) const = 0;
	virtual ~IEatBehavior() = default;
};

