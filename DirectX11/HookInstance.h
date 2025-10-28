#pragma once
#include "Item.h"



class HookInstance :
	public ItemInstance
{
public:

	enum HookState :uint8_t
	{
		Idle,		// 待機
		Charging,	// 溜め中
		Casting,	// 投擲中
		Caught,		// 釣れた
		Retrieving, // 回収中
	};

	void OnUseStart(Player* player) override;
	void OnUseHold(Player* player, float deltaTime) override;
	void OnUseRelease(Player* player) override;

	void Update(float dt) override;
	void SetHookState(HookState state) { m_hookState = state; }
	HookState GetHookState() const { return m_hookState; }

	void AddItem(ItemInstance* item) { m_caughtItems.push_back(item); }
	bool HasCharge() override { return true; }
	float GetChargeProgress() override;
private:

	HookState m_hookState = Idle;
	float m_chargeTime = 0;
	float m_throwSpeed = 0;

	/// <summary>
	/// current velocity
	/// </summary>
	DirectX::XMFLOAT3 m_velocity = { 0,0,0 };

	std::vector<ItemInstance*> m_caughtItems = {};
};

