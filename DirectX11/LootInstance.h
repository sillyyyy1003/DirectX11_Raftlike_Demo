#pragma once
#include "Item.h"
class LootInstance :
    public ItemInstance
{
private:

    typedef std::string ItemName;   // アイテム名
    typedef int ItemCount;          // アイテムの数
    std::unordered_map<ItemName, ItemCount> m_lootContent;  // 実際生成した中身

public:
    void InitItem(std::shared_ptr<const Item> _proto, int _count, float _durability) override;
    const std::unordered_map<ItemName, ItemCount>& GetLootContent() const { return m_lootContent; }
};

