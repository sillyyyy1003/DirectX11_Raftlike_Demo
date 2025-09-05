#pragma once
#include <string>
#include <unordered_map>
#include "Item.h"
#include "Layers.h"

/// @brief Item Instance Manager
class ItemDataBase
{
public:

	static ItemDataBase& Instance();
	/// @brief アイテムを登録する
	void RegisterItem(const char* name, std::shared_ptr<Item> item);

	/// @brief アイテムを登録する（IDとモデルID、マテリアルIDも設定）
	void RegisterItem(const char* name, std::shared_ptr<Item> item, uint32_t itemId, uint32_t modelId, uint32_t materialId);

	std::shared_ptr<const Item> GetItem(const char* name);
	void UnInit();

	std::shared_ptr<ItemInstance> CreateItemInstance(const char* itemName, int count = 1, float durability = -1);

	std::shared_ptr<ItemInstance> CreateItemInstance(const char* itemName, int count, float durability, ObjectLayer layer);


	void LoadItemDataFromJsonFile(const char* jsonFilePath);
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

