#pragma once
#include "GameObject.h"

class Player;

/// @brief ゲーム内で実際使われているアイテムの基底クラス
class Item
{
public:
	enum class ItemType :uint8_t
	{
		Weapon = 0,		// 武器
		Utility = 1,	// 建築ツール
		Food = 2,		// 食品
		Water = 3,		// 飲み物
		BaseMaterial = 4// 建築材料	
	};
	
	virtual ~Item() = default;

	Item(ItemType _type, bool stackable = true, int maxStack = 20, float maxDurability = 0);
	/// @brief アイテム名を設定する
	void SetName(const char* name) { m_itemName = std::string(name); }

	ItemType GetItemType() const { return m_itemType; }
	bool GetIsStackable() const { return m_isStackable; }
	int GetMaxStack() const { return m_maxStack; }
	float GetMaxDurability() const { return m_maxDurability; }
	bool HasDurability() const { return m_maxDurability > 0; }
	std::string GetName() const { return m_itemName; }

	virtual void OnUse(Player* player) const = 0;
	//virtual void OnHold(Player* player, float deltaTime) {}

	void SetItemId(uint32_t itemId) { m_itemId = itemId; }
	void SetModelId(uint32_t modelId) { m_modelId = modelId; }
	void SetMaterialId(uint32_t materialId) { m_materialID = materialId; }


	uint32_t GetItemId() const { return m_itemId; }
	uint32_t GetModelId() const { return m_modelId; }
	uint32_t GetMaterialId() const { return m_materialID; }
private:
	ItemType m_itemType;

	bool m_isStackable;			// スタック可能か？
	int m_maxStack;				// 最大スタック数
	float m_maxDurability;		// 最大耐久値

	std::string m_itemName;		// アイテム名
	uint32_t m_itemId = -1;		// アイテムID, -1は未設定
	uint32_t m_modelId = -1;
	uint32_t m_materialID = -1;
};


/// @brief 実際にゲームワールドで生成するインスタンス
class ItemInstance :
	public GameObject
{
public:
	enum ItemState
	{
		Active,		// Default
		InActive,	// Far away from player
		Collected	// プレイヤーにひろわれた
	};
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
	std::string GetName() const { return m_protoPtr->GetName(); };
	void DecreaseCount(int count);

	uint32_t GetItemId() const { return m_protoPtr ? m_protoPtr->GetItemId() : -1; }
	uint32_t GetModelId() const { return m_protoPtr ? m_protoPtr->GetModelId() : -1; }
	uint32_t GetMaterialId() const { return m_protoPtr ? m_protoPtr->GetMaterialId() : -1; }

	void SetState(ItemState state) { m_itemState = state; }
	ItemState GetState() const { return m_itemState; }

	void Update(float dt) override;
	
private:
	std::shared_ptr<const Item> m_protoPtr;
	int m_count = 1;				// 実際のアイテム数
	float m_durability = -1;		// 現在の耐久値 >>-1 耐久無し

	ItemState m_itemState = ItemState::Active;
};


//========BuildTool===========
class Utility:
	public Item
{
public:
	Utility();
	~Utility() override = default;
	void OnUse(Player* player) const override;
	//void OnHold(Player* player, float deltaTime) override;

};


class Food:
	public Item
{
public:
	Food(float _foodValue);
	~Food() override = default;
	void OnUse(Player* player) const  override;

private:
	float m_foodValue;	// 回復値
};


class BaseMaterial :
	public Item
{

public:
	BaseMaterial();
	~BaseMaterial() override = default;

	void OnUse(Player* player) const override;
};