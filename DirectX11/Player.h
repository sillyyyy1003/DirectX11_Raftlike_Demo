#pragma once
#include "FirstPersonCamera.h"
#include "GameObject.h"

/// <summary>
/// Player
/// </summary>
class Player :
    public GameObject
{
private:




public:
	Player();

	void Update(float dt)override;
	void Draw() override;


	/// @brief 平行移動
	/// @param d 距離
	void Strafe(float d);

	/// @brief 直行
	/// @param d 距離
	void Walk(float d);

	/// @brief 前に直行
	/// @param d 距離
	void MoveForward(float d);

	/// @brief 上下観察
	/// @param rad +:UP -:DOWN
	void Pitch(float rad);

	/// @brief 左右観察
	/// @param rad +:LEFT -:RIGHT
	void RotateY(float rad);

	

};

