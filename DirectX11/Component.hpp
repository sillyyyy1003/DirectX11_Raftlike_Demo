#pragma once
#include <cstdint>

class GameObject;

namespace MyComponent
{
	enum class ComponentType :uint16_t
	{
		None = 0,
		DebugRender = 1 << 0,		// Debug Render Component
		Render = 1 << 1,			// Render Component
		Physics = 1 << 2,			// Physics Component
		Hunger = 1 << 3,			// Hunger Component
		Thirst = 1 << 4,			// Thirst Component
		LivingEntity = 1 << 5,		// Living Entity Component
	};

}


/// @brief Base class for all components in the game object system
class Component
{
public:
	virtual ~Component() = default;
	virtual void SetGameObject(GameObject* gameObject) {}
};

