#pragma once
#include <string>
#include <unordered_map>
#include "Item.h"

/// @brief Item Instance Manager
class ItemDataBase
{
public:

	static ItemDataBase& Instance();
	void RegisterItem(const char* name, std::shared_ptr<Item> item);
	std::shared_ptr<const Item> GetItem(const char* name);


private:
	ItemDataBase() = default;
	~ItemDataBase() = default;
	
private:
	typedef std::unordered_map<std::string, std::shared_ptr<Item>> Items;
	Items m_items;


};

