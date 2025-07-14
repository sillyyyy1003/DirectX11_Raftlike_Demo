#pragma once
#include <cstdint>

namespace MyComponent
{
	enum class ComponentType :uint16_t
	{
		None = 0,
		DebugRender = 1 << 0,		// Debug Render Component
		Physics = 1 << 1,			// Physics Component
		Hunger = 1 << 2,			// Hunger Component
		Thirst = 1 << 3,			// Thirst Component
	};

}


/// @brief Base class for all components in the game object system
class Component
{
public:
	virtual ~Component() = default;
};

