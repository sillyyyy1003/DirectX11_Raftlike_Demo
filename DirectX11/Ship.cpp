#include "Ship.h"


ShipTile::ShipTile(ShipTileType type) :
	Item(ItemType::ShipTile, false, 0, 100)//Ship Tile cant be stacked the default durability is 100
{
	m_tileType = type;
	m_currentDurability = GetMaxDurability();
}

ShipTile::~ShipTile()
{
}

void ShipTile::OnDamage(float damage)
{
	if(m_currentDurability>0)m_currentDurability -= damage;
	if (m_currentDurability <= 0)Destroy();
}

void ShipTile::Destroy()
{
	if (m_isDestroyed) return;
	m_isDestroyed = true;

}

Ship::Ship()
{
}

Ship::~Ship()
{
}

void Ship::OnTilesChange()
{
	// Check grid coordination

	// chunk if full of tile, keep it

	// if chunk has lost tile

	// if remain tile could fulfill a new chunk make a new chunk

	// Debug Log output
#ifdef _DEBUG

#endif


}

void Ship::Draw()
{
	for(const auto& tile:m_tiles)
	{
		tile->Draw();
	}
}
