#pragma once
#include "Item.h"

class FoodInstance :
	public ItemInstance
{
public:
	void InteractWith(BodyID& rigidBody, Player* player) override;
	void OnUse(Player* player) override;
};

class CupInstance:
	public ItemInstance, public IToolBehavior
{
public:
	CupInstance();
	enum CupState :uint8_t
	{
		SaltyWater,		// 塩水
		FreshWater,		// 淡水
		Empty			// 空

	};
	void SetCupState(CupState state) { m_cupState = state; }
	CupState GetCupState() const { return m_cupState; }

	void OnUse(Player* player) override;

	/// @brief Interact with the owner item of rigid body
	/// @param rigidBody 
	/// @param player 
	void InteractWith(BodyID& rigidBody, Player* player) override;
private:

	CupState m_cupState = CupState::Empty;

};

class PurifierInstance :
	public ItemInstance
{
public:
	enum WaterPurifierState :uint8_t
	{
		Idle,			// 待機
		Purifying,		// 浄水中
		Finished		// 浄水完了
	};


	~PurifierInstance() override = default;

	void InteractWithCup(CupInstance* cup);

	WaterPurifierState GetPurifierState() const { return m_waterPurifierState; }
	void Update(float dt) override;

private:
	float m_timer = 0;
	WaterPurifierState m_waterPurifierState = Idle;
};
