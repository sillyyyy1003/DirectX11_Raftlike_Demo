#pragma once
#include <DirectXMath.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

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

	void Move(Vec3Arg& moveDir);

	DirectX::XMFLOAT3 GetPosition();
private:

	Ref<CharacterVirtual> m_pCharacter;
	bool m_ControlMovementDuringJump = true;	///< false>>空中では移動できない
	float m_jumpSpeed;
	float m_moveSpeed;							//移動速度
	Vec3 m_moveDirection;						//移動方向
	Vec3 m_desiredVelocity = Vec3::sZero();		// Smoothed value of the player input

	// The different stances for the character
	RefConst<Shape>			m_standingShape;
	RefConst<Shape>			m_crouchingShape;
	RefConst<Shape>			m_innerCrouchingShape;
	RefConst<Shape>			m_innerStandingShape;

	// List of active characters in the scene so they can collide
	CharacterVsCharacterCollisionSimple m_characterVsCharacterCollision;

};

