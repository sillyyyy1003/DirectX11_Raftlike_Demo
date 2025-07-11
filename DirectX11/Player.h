#pragma once
#include "FirstPersonCamera.h"
#include "GameObject.h"
#include "HungerComponent.h"
#include "PlayerController.h"
#include "UIElement.h"

/// <summary>
/// Player
/// </summary>
class Player :
    public GameObject
{
public:
	Player();

	void Init();

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


	CameraController* GetCameraController() { return m_pCameraController.get(); }

private:
	// Playerの動きを扱う
	std::unique_ptr<PlayerController> m_pPlayerController;

	// Cameraの動き
	std::shared_ptr <CameraController> m_pCameraController;

	// Hunger manager
	std::shared_ptr<HungerComponent> m_pHungerComponent;

	// 物理挙動を扱う
	std::shared_ptr<PlayerCharacter> m_pPlayerCharacter;

	float m_speed;

};

