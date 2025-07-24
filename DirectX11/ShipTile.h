#pragma once
#include "GameObject.h"
#include "Player.h"



namespace Ship
{
	//Tile座標
	struct TileCoord
	{
		int x, y;
		bool operator==(const TileCoord& other) const
		{
			return x == other.x && y == other.y;
		}
	};
}

/// @brief 船のタイル単体クラス
class Tile :
	public GameObject
{
public:
	Tile(float durability = 100.f);
	~Tile() override = default;

private:
	float m_durability;		//船の耐久値

};

/// @brief 全ての船タイルを管理するクラス
class TileManager:
	public Component
{
public:
	TileManager();
	~TileManager() override = default;

	void Update(float dt);

	void Draw();
private:
	//typedef std::unordered_map<Ship::TileCoord, Tile> Tiles;
	//Tiles m_tiles;			// 全てのタイルを管理する
	//std::unordered_set<Ship::TileCoord> m_edgeTilesCoord;	// 外側のタイル座標を管理する

	Player* m_player;



};