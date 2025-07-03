#pragma once
#include <cstdint>

namespace MyComponent
{
	enum class ComponentType :uint16_t
	{
		None = 0,
		DebugRender = 1 << 0,	// Debug Render Component
		Physics = 1 << 1,		// Physics Component
		HungerManager = 1 << 2,		//Hunger Manager
	};

}


/// @brief Base class for all components in the game object system
class Component
{
public:
	virtual ~Component() = default;
};

