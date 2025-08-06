#include "ItemDataBase.h"
#include "DebugLog.h"

ItemDataBase& ItemDataBase::Instance()
{
	static ItemDataBase instance;
	return instance;
}

void ItemDataBase::RegisterItem(const char* name, std::shared_ptr<Item> item)
{
	//Check if item already exits
	auto it = m_items.find(std::string(name));
	if (it != m_items.end())
	{
		DebugLog::LogWarning("[ItemDataBase] {} already exits", name);
		return;
	}

	m_items[std::string(name)] = item;
	DebugLog::Log("[ItemDataBase] {} is registered!", name);


}

std::shared_ptr<const Item> ItemDataBase::GetItem(const char* name)
{
	auto it = m_items.find(std::string(name));
	if (it != m_items.end())
		return it->second;

	DebugLog::LogWarning("[ItemDataBase] {} is not exit", name);
	return nullptr;
}
