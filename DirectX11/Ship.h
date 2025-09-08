#pragma once
#include "Item.h"
#include "PhysicsComponent.h"


/// <summary>
/// This is ship unit for ship build
/// </summary>
class ShipTile :
	public Item
{
	// Ship tile 本身并不持有碰撞体。仅用于数据层和视觉层：即视觉表现是依赖ShipTile，当前Tile的耐久值也是依赖ShipTile
public:
	enum ShipTileType :uint8_t
	{
		SquareTile,
		TriangleTile,
	};

	ShipTile(ShipTileType type);
	~ShipTile() override;

	void OnDamage(float damage);
	void Destroy();


	
private:
	float m_currentDurability;
	bool m_isDestroyed = false;
	ShipTileType m_tileType;
};




class Ship
{
public:
	Ship();
	~Ship();

	typedef std::vector<std::shared_ptr<ItemInstance>> Tiles;
	Tiles m_tiles;	// tile visual/durability data

	typedef std::vector<std::shared_ptr<PhysicsComponent>> PhysicsComponent;
	PhysicsComponent m_pPhysicsComponents;	// physics data

	/// @brief Tileの数が変更する時、もう一回整理して、ChunkかSingleBoxかまとめる
	void OnTilesChange();

	void Draw();


};

