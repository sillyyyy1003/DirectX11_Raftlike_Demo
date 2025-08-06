#pragma once
#include <DirectXMath.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "Transform.h"

using namespace JPH;

/// @brief this class represents a player character in the game
///	using jolt physics
class PlayerCharacter
{
public:

	PlayerCharacter();
	~PlayerCharacter();

	void Init();
	void Update(float deltaTime);
	void SetRotation(const DirectX::XMFLOAT3& rot);

	void SetMoveDir(Vec3Arg& moveDir);

	/// @brief 今のコライダーの世界座標を取得
	DirectX::XMFLOAT3 GetPosition();
	/// @brief 今の回転を取得
	DirectX::XMFLOAT3 GetEulerRotation();

	void SyncPlayerWorldPosition(Transform& t);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void Jump();

	CharacterVirtual* GetCharacterVirtual() { return m_pCharacter; }
private:

	Ref<CharacterVirtual> m_pCharacter;
	bool m_ControlMovementDuringJump = true;	///< false>>空中では移動できない
	float m_jumpSpeed;
	float m_moveSpeed;							// 移動速度
	Vec3 m_moveDirection;						// 移動方向
	Vec3 m_desiredVelocity;						// Smoothed value of the player input
	bool m_enableCharacterInertia;				///< true>>キャラクターの慣性を有効にする
	bool m_allowSliding = false;				///< true>when the player is pressing movement controls
	bool m_wantToJump = false;					///< true>>when player trigger jump	

	// The different stances for the character
	RefConst<Shape>			m_standingShape;	// 立ち状態
	RefConst<Shape>			m_crouchingShape;	// 蹲る状態
	RefConst<Shape>			m_innerCrouchingShape;	// Inner crouching shape for the character
	RefConst<Shape>			m_innerStandingShape;	// Inner standing shape for the character

	// List of active characters in the scene so they can collide
	CharacterVsCharacterCollisionSimple m_characterVsCharacterCollision;



#ifdef _DEBUG
	float m_debugDrawRadius = 0.f;
	float m_debugDrawHeight = 0.f;
public:
	/// @brief デバッグ用の描画半径を取得
	float GetDebugDrawRadius() const { return m_debugDrawRadius; }
	/// @brief デバッグ用の描画高さを取得
	float GetDebugDrawHeight() const { return m_debugDrawHeight; }

	
#endif
};

