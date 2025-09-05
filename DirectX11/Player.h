#pragma once
#include "GameObject.h"
#include "HungerComponent.h"
#include "Inventory.h"
#include "LivingEntity.h"
#include "PlayerController.h"

/// <summary>
/// Player
/// </summary>
class Player :
    public GameObject
{
public:
	

	Player();
	~Player() override;

	//void Init();

	/// @brief Using Json file to read player settings
	/// @param filePath 
	bool Init(const char* filePath = nullptr);

	void Update(float dt)override;
	void Draw() override;


	/// @brief 平行移動
	/// @param d 距離
	void Strafe(float dt);

	/// @brief 直行
	/// @param d 距離
	void Walk(float dt);

	/// @brief 前に直行
	/// @param d 距離
	void MoveForward(float dt);

	/// @brief 上下観察
	/// @param rad +:UP -:DOWN
	void Pitch(float dt);

	/// @brief 左右観察
	/// @param rad +:LEFT -:RIGHT
	void RotateY(float dt);


	CameraController* GetCameraController() const { return m_pCameraController.get(); }
	Inventory* GetInventory() const { return m_pInventory.get(); }

	void OnStarveStateChanged(bool isStarve);
	void OnHungryStateChanged(bool isHungry);

	/// @brief Register Event to death listener
	/// @param cb 
	void AddDeathListener(const PlayerEntity::Callback& cb);

	void Kill() { m_pPlayerEntity->Dead(); }
	void Revive() { m_pPlayerEntity->Revive(); }

private:

	//PlayerのHPを扱う
	std::shared_ptr<PlayerEntity> m_pPlayerEntity;

	// Playerの動きを扱う
	std::unique_ptr<PlayerController> m_pPlayerController;

	// Cameraの動き
	std::shared_ptr <CameraController> m_pCameraController;

	// Hunger manager
	std::shared_ptr<HungerComponent> m_pHungerComponent;

	// 物理挙動を扱う
	std::shared_ptr<PlayerCharacter> m_pPlayerCharacter;

	// Bag
	std::shared_ptr<Inventory> m_pInventory;


	bool m_isInNegativeState = false;	/// true>>if player is in negative state, like hunger, thirst, etc.


	float m_moveSpeed;				// Player default move speed;
	float m_jumpSpeed;
	float m_negativeStatusScale;	// マイナス状態の影響を受けるスケール（空腹、渇きなど）
};


