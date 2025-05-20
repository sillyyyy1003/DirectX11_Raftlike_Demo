#pragma once
#include <Windows.h>
#undef max
#undef min

class KInput
{

public:
	BYTE mKeyTable[256] = {};
	BYTE mOldTable[256] = {};

private:
	KInput() {};
	KInput(const KInput&) = delete;
	KInput& operator=(const KInput&) = delete;
	~KInput();

	/// @brief Get the Singleton Instance
	/// @return 
	static KInput* Get() { static KInput instance; return &instance; };

public:
	static HRESULT InitInput();

	/// @brief 
	static void UpdateInput();

	static bool IsKeyPress(BYTE key);
	static bool IsKeyTrigger(BYTE key);
	static bool IsKeyRelease(BYTE key);
	static bool IsKeyRepeat(BYTE key);

};



