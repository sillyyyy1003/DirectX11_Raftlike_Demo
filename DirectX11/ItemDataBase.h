#pragma once
#include <string>
#include <unordered_map>
#include "Item.h"
#include "Layers.h"

class SceneGame;
/// @brief Item Instance Manager
class ItemDataBase
{
public:

	static ItemDataBase& Instance();
	/// @brief アイテムを登録する
	void RegisterItem(const char* name, std::shared_ptr<Item> item);

	/// @brief アイテムを登録する（IDとモデルID、マテリアルIDも設定）
	void RegisterItem(const char* name, const char* iconName, const char* description, std::shared_ptr<Item> item, uint32_t itemId, uint32_t modelId, uint32_t materialId);

	std::shared_ptr<const Item> GetItem(const char* name);
	void UnInit();

	/// @brief 描画機能持ちのItemInstanceを生成する
	/// @param itemName item name
	/// @param count 数
	/// @param durability 耐久値 
	std::shared_ptr<ItemInstance> CreateItemInstanceToWorld(const char* itemName, int count = 1);

	/// @brief 描画機能・物理演算機能持ちのItemInstanceを生成する
	/// @param itemName item name
	/// @param count 数
	/// @param durability 耐久値 
	/// @param layer 物理レイヤー
	std::shared_ptr<ItemInstance> CreateItemInstanceToWorldWithPhysics(const char* itemName, int count, ObjectLayer layer);

	/// @brief データのみのItemInstanceを生成する
	/// @param itemName item name
	/// @param count 数
	/// @param durability 耐久値 
	std::shared_ptr<ItemInstance> CreateItemInstance(const char* itemName, int count = 1);

	/// @brief 描画機能・物理演算機能持ちのItemInstanceを生成する・Kinematic or Dynamic指定可能
	/// @param itemName item name
	/// @param count 数
	/// @param durability 耐久値 
	/// @param layer 物理レイヤー
	/// @param type 物理型
	/// @return 
	std::shared_ptr<ItemInstance> CreateItemInstanceToWorldWithPhysics(const char* itemName, int count, ObjectLayer layer,EMotionType type);

	
	void LoadItemDataFromJsonFile(const char* jsonFilePath);

	std::shared_ptr<ItemInstance> CreateItemInstance(Item::ItemType type);


	const DirectX::XMFLOAT3& GetItemSize(const std::string& itemName);
private:

	ItemDataBase() = default;
	~ItemDataBase();

	
	
private:
	typedef std::unordered_map<std::string, std::shared_ptr<Item>> Items;
	Items m_items;
	typedef std::unordered_map<std::string, DirectX::XMFLOAT3> ItemSizes;
	ItemSizes m_itemSizes;

	uint32_t m_nextID;
};

