#pragma once
#include "Signal.hpp"
#include "DirectXMath.h"

namespace Event
{
	enum class ResolutionPreset:uint8_t
	{
		R_1080p,
		R_720p,
		R_540p,
	};

	// Resolution
	const DirectX::XMINT2 R_1080 = { 1920,1080 };		//1080p
	const DirectX::XMINT2 R_720 = { 1280,720 };		//720p
	const DirectX::XMINT2 R_540 = { 960,540 };		//540p

}

/// @brief ゲーム全体変更する
class GameSignalBus
{
public:
	/// @brief WindowSize変更
	Signal<Event::ResolutionPreset> OnResolutionChangeRequest;
};

