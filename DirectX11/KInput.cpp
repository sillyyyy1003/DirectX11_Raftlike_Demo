#include "KInput.h"


KInput::~KInput()
{
}

HRESULT KInput::InitInput()
{
	// 一番最初の入力
	GetKeyboardState(Get()->mKeyTable);
	return S_OK;
}


void KInput::UpdateInput()
{
	// 古い入力を更新
	memcpy_s(Get()->mOldTable, sizeof(mOldTable), Get()->mKeyTable, sizeof(mKeyTable));
	// 現在の入力を取得
	GetKeyboardState(Get()->mKeyTable);
}

bool KInput::IsKeyPress(BYTE key)
{
	return Get()->mKeyTable[key] & 0x80;
}

bool KInput::IsKeyTrigger(BYTE key)
{
	return(Get()->mKeyTable[key] ^ Get()->mOldTable[key]) & Get()->mKeyTable[key] & 0x80;
}

bool KInput::IsKeyRelease(BYTE key)
{
	return (Get()->mKeyTable[key] ^ Get()->mOldTable[key]) & Get()->mOldTable[key] & 0x80;
}

bool KInput::IsKeyRepeat(BYTE key)
{
	return false;
}
