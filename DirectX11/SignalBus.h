#pragma once
#include <cstdint>
#include "Signal.hpp"

namespace Event
{
	enum class ResolutionPreset:uint8_t
	{
		R_1080p,
		R_720p,
		R_540p,
	};

}

/// @brief ゲーム全体変更する
class GameSignalBus
{
public:
	/// @brief WindowSize変更
	Signal<Event::ResolutionPreset> OnResolutionChangeRequest;
};

