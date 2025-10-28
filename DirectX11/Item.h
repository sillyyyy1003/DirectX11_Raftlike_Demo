#pragma once
#include "Behaviour.hpp"
#include "GameObject.h"

class Player;

/// @brief ゲーム内で実際使われているアイテムの基底クラス
class Item
{
public:
	enum class ItemType :uint16_t
	{
		Weapon = 0,			// 武器
		Utility = 1,		// 建築ツール
		Food = 2,			// 食品
		Water = 3,			// 飲み物
		BaseMaterial = 4,	// 建築材料
		ShipTile = 5,	
		Cup = 6,
		WaterPurifier=7,
		Hook = 8,
		Box=9,				// Box filled with material/ food
	};
	
	virtual ~Item() = default;

	Item(ItemType _type, bool stackable = true, int maxStack = 20, float maxDurability = 0);
	/// @brief アイテム名を設定する
	void SetName(const char* name) { m_itemName = std::string(name); }

	/// @brief アイテムタイプを返す
	ItemType GetItemType() const { return m_itemType; }
	/// @brief スタック可能かを返す
	bool GetIsStackable() const { return m_isStackable; }
	/// @brief 最大スタック数を返す
	int GetMaxStack() const { return m_maxStack; }
	/// @brief 最大耐久値を返す
	float GetMaxDurability() const { return m_maxDurability; }
	/// @brief 耐久あるかどうかを返す
	bool HasDurability() const { return m_maxDurability > 0; }
	/// @brief アイテム名を返す
	std::string GetName() const { return m_itemName; }
	/// @brief アイテムのアイコン名を返す
	std::string GetIconTextureName() const { return m_iconName; }

	std::string GetItemDescription() const { return m_description; }

	void SetItemId(uint32_t itemId) { m_itemId = itemId; }
	void SetModelId(uint32_t modelId) { m_modelId = modelId; }
	void SetMaterialId(uint32_t materialId) { m_materialID = materialId; }
	void SetIconName(const char* name) { m_iconName = name; };
	void SetDescription(const char* description) { m_description = description; }

	uint32_t GetItemId() const { return m_itemId; }
	uint32_t GetModelId() const { return m_modelId; }
	uint32_t GetMaterialId() const { return m_materialID; }
private:
	ItemType m_itemType;

	bool m_isStackable;			// スタック可能か？
	int m_maxStack;				// 最大スタック数
	float m_maxDurability;		// 最大耐久値

	std::string m_itemName;		// アイテム名
	std::string m_iconName;		// IconTexture
	std::string m_description;	// description of item

	uint32_t m_itemId = -1;		// アイテムID, -1は未設定
	uint32_t m_modelId = -1;
	uint32_t m_materialID = -1;
};


/// @brief 実際にゲームワールドで生成するインスタンス
class ItemInstance :
	public GameObject, public Interactable
{
public:


	ItemInstance();

	/// @brief Item初期化
	/// @param _proto 参照ポインター
	/// @param _count 数
	/// @param _durability 耐久値 
	void InitItem(std::shared_ptr<const Item> _proto, int _count = 1, float _durability = -1);

	~ItemInstance() override = default;
	bool IsStackable() const { return m_protoPtr && m_protoPtr->GetIsStackable(); }
	bool HasDurability() const { return m_protoPtr && m_protoPtr->HasDurability(); }

	/// @brief Getter of item ptr
	std::shared_ptr<const Item> GetProto() const { return m_protoPtr; }
	/// @brief Getter of item count
	int GetCount() const { return m_count; }
	/// @brief Getter of item durability
	float GetDurability() const { return m_durability; }

	/// @brief Add count to item count
	void AddCount(int count) { m_count += count; }

	/// @brief Get Item(Proto ptr) Name
	std::string GetName() const { return m_protoPtr->GetName(); };
	void DecreaseCount(int count);

	uint32_t GetItemId() const { return m_protoPtr ? m_protoPtr->GetItemId() : -1; }
	uint32_t GetModelId() const { return m_protoPtr ? m_protoPtr->GetModelId() : -1; }
	uint32_t GetMaterialId() const { return m_protoPtr ? m_protoPtr->GetMaterialId() : -1; }

	void SetState(int state);
	int GetState() const { return m_objectState; }

	//void SetItemInstanceState(ItemInstanceState state);
	//ItemInstanceState GetItemInstanceState() const { return m_itemInstanceState; }

	virtual void Update(float dt) override;
	virtual void InteractWith(BodyID& rigidBody, Player* player) override {};

	virtual void OnUse(Player* player) override {}	// 状態分け無しの使用
	virtual void OnUseStart(Player* player) override {}	// 使用開始
	virtual void OnUseHold(Player* player,float deltaTime) override {}	// 使用中
	virtual void OnUseRelease(Player* player) override {}	// 使用終了

protected:
	std::shared_ptr<const Item> m_protoPtr;
	int m_count = 1;				// 実際のアイテム数
	float m_durability = -1;		// 現在の耐久値 >>-1 耐久無し

	int m_objectState = 0;			// physics state for item instance used for driftmanager
	/*ItemInstanceState m_itemInstanceState;*/
};


//========BuildTool===========
class Utility:
	public Item
{
public:
	Utility();
	~Utility() override = default;
};


class Food:
	public Item
{
public:
	Food(float _foodValue);
	~Food() override = default;
	float GetFoodValue() const { return m_foodValue; }
	
private:
	float m_foodValue;	// 回復値
};

class BaseMaterial :
	public Item
{

public:
	BaseMaterial();
	~BaseMaterial() override = default;
};

class Cup :
	public Item
{
public:

	Cup(float value);
	~Cup() override = default;
	float GetRecoverValue()const { return m_recoverValue; }

private:
	float m_recoverValue;// 回復値

};


class WaterPurifier:
	public Item
{
public:
	WaterPurifier(float timeThreshold);
	~WaterPurifier() override = default;

	float GetPurifyThreshold()const { return m_timeThreshold; }

protected:
	float m_timeThreshold;	// 浄水にかかる時間
};

class Spear :
	public Item
{
public:
	Spear(float damage,float durability);
	~Spear() override = default;

	float GetDamage() const { return m_damage; }
private:
	float m_damage;	// Attack damage;
};

class Hook:
public Item
{
public:
	Hook(float maxSpeed, float minSpeed, float chargeTime,float chargeSpeed, float durability);
	~Hook() override = default;

	float GetMaxSpeed() const { return m_maxSpeed; }
	float GetMinSpeed() const { return m_minSpeed; }
	float GetChargeLimit() const { return m_chargeTimeLimit; }
	float GetChargeSpeed() const { return m_chargeSpeed; }
private:
	float m_maxSpeed;			// 最大速度
	float m_minSpeed;			// 最小速度
	float m_chargeTimeLimit;	// 最大チャージ需要時間
	float m_chargeSpeed;
};