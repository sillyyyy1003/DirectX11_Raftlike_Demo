#pragma once
#include <cstdint>
#include <DirectXMath.h>

#include "CameraController.h"
class Player;

/// @brief Playerのインプット処理
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

	//========Input Param
	POINT m_centerPos;						//毎ループWindowの中心位置
	POINT m_oldPos;							//マウス位置
	DirectX::XMFLOAT2 m_windowSize;         //Window Size
	//========Input Param


public:
	bool m_isControllable = true;			//操作可能か

public:

	PlayerController(Player* player);

	///	@brief Handle player input and update player position
	void Update(float dt);

	void UpdateWindowSize(DirectX::XMFLOAT2 windowSize);
	void UpdateWindowCenter(POINT center);

};

