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
		Builder = 1,	// 建築ツール
		Food = 2,		// 食品
		Water = 3,		// 飲み物
		BaseMaterial = 4// 建築材料	
	};
	
	virtual ~Item() = default;

	Item(ItemType _type, bool stackable = true, int maxStack = 20, float maxDurability = 0);

	ItemType GetItemType() const { return m_itemType; }
	bool GetIsStackable() const { return m_isStackable; }
	int GetMaxStack() const { return m_maxStack; }
	float GetMaxDurability() const { return m_maxDurability; }
	bool HasDurability() const { return m_maxDurability > 0; }

	//virtual void OnUse(Player* player) = 0;
	//virtual void OnHold(Player* player, float deltaTime) {}
private:
	ItemType m_itemType;
	bool m_isStackable;			// スタック可能か？
	int m_maxStack;				// 最大スタック数
	float m_maxDurability;		// 最大耐久値
};


/// @brief 実際にゲームワールドで生成するインスタンス
class ItemInstance :
	public GameObject
{
public:
	//ItemInstance(std::shared_ptr<const Item> _proto, int _count = 1, float _durability = -1);
	ItemInstance() {}

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
private:
	std::shared_ptr<const Item> m_protoPtr;
	int m_count = 1;				// 実際のアイテム数
	float m_durability = -1;		// 現在の耐久値 >>-1 耐久無し
};


//========BuildTool===========
class Builder:
	public Item
{
public:
	Builder();
	~Builder() override = default;
	//void OnUse(Player* player) override;
	//void OnHold(Player* player, float deltaTime) override;

};


class Food:
	public Item
{
public:
	Food(float _foodValue);
	~Food() override = default;

private:
	float m_foodValue;	// 回復値
};

