#pragma once
#include <map>
#include "Item.h"

class Player;
class IEffect;
/// @brief 全ての漂流物を扱う
class DriftManager
{
public:

	enum class DriftObjectType :uint8_t
	{
		Wood = 0, 	// Wood
		Wire = 1,	// Wire
		Plastic = 2	// Plastic
	};

	DriftManager();
	~DriftManager();

	void Init(IEffect* effect);
	void Init(IEffect* effect, Player* player);
	void UnInit();
	void Update(float tick);
	void Draw();
	float GetDistanceFromPlayer(const DirectX::XMFLOAT3& pos) const;

	/// @brief 位置判断に必要なプレイヤーのデータをセット
	void SetPlayer(Player* player) { m_pPlayer = player; }


private:

	/// @brief ひろわれたか・アイテムが遠くにあるかをチェック
	/// @param item 
	/// @return true>> アイテムをリサイクルする必要がある
	bool CheckNearItemRecycle(ItemInstance* item);

	/// @brief プレイヤーに近い位置に生成する必要があるか
	bool ShouldSpawnNearItem(DirectX::XMFLOAT3 pos);

	/// @brief プレイヤーから遠い位置にあるか
	///	true>> Recycle the far item
	bool ShouldRecycleFarItem(const DirectX::XMFLOAT3& pos);

	/// @brief 一定範囲内のランダムな位置を取得
	DirectX::XMFLOAT3 GetRandomFarPosition();

	/// @brief 一般GameObjectを生成
	/// @param type アイテムの種類
	/// @param pos 生成位置
	void CreateFarItem(DriftObjectType type, const DirectX::XMFLOAT3& pos);

	/// @brief PhysicsComponentを持つItemInstanceを生成
	/// @param type 生成するアイテムの種類
	/// @param pos 生成位置
	void CreateNearItem(DriftObjectType type, const DirectX::XMFLOAT3& pos);

	/// @brief 初期アイテム生成
	///	@type 生成するアイテムの種類
	void SpawnInitialItem(DriftObjectType type);

	void UpdateFarObjectStatus();

private:

	Player* m_pPlayer;											// プレイヤーの位置
	std::map<DriftObjectType, std::vector<std::shared_ptr<ItemInstance>>> m_nearItemInstances;
	std::map<DriftObjectType, std::vector<std::unique_ptr<GameObject>>> m_farGameObjects;

	float m_spawnTimer;
	DirectX::XMFLOAT2 m_spawnRangeX;
	DirectX::XMFLOAT2 m_spawnRangeZ;


};

