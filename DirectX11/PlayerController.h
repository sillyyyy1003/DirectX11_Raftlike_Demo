#pragma once
#include <cstdint>
class Player;

class PlayerController
{
private:
	enum class PlayerState:uint16_t
	{
		Default = 0,	//初期状態
		Walk,			//歩行
		Jump,			//ジャンプ
		Attack,			//攻撃
	};


	Player* m_pPlayer;
	float m_moveSpeed;	//移動速度

	PlayerState m_state;	//プレイヤー状態
public:

	PlayerController(Player* player);

	///	@brief Handle player input and update player position
	void Update(float deltaTime);


private:

};

