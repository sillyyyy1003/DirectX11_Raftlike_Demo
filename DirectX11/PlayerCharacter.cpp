#include "PlayerCharacter.h"

#include <imgui.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include "Layers.h"
#include "PhysicsManager.h"

namespace 
{
	static constexpr float MoveSpeed = 4.f;
	static constexpr float JumpSpeed = 4.f;
	constexpr float RotateLimit = DirectX::XM_PI * 7 / 18;	// 70度, 限制玩家上下视角旋转范围

	static constexpr float CharacterHeightStanding = 1.35f;
	static constexpr float CharacterRadiusStanding = 0.5f;
	static constexpr float CharacterHeightCrouching = 0.8f;
	static constexpr float CharacterRadiusCrouching = 0.5f;
	static constexpr float InnerShapeFraction = 0.8f;		//  内部形状的比例 0.8

	static constexpr EBackFaceMode sBackFaceMode = EBackFaceMode::CollideWithBackFaces;
	static constexpr float MaxSlopeAngle = DegreesToRadians(45.f);
	static constexpr float MaxStrength = 100.0f;
	static constexpr float CharacterPadding = 0.02f;
	static constexpr float PenetrationRecoverySpeed = 1.0f;
	static constexpr float PredictiveContactDistance = 0.1f;
	static constexpr bool EnableWalkStairs = true;
	static constexpr bool EnableStickToFloor = true;
	static constexpr bool EnhancedInternalEdgeRemoval = false;
	static constexpr bool CreateInnerBody = false;
	static constexpr bool PlayerCanPushOtherCharacters = true;
	static constexpr bool OtherCharactersCanPushPlayer = true;
	
	static constexpr bool EnableCharacterInertia = true;
}

PlayerCharacter::PlayerCharacter() :
	m_jumpSpeed(JumpSpeed),
	m_moveSpeed(MoveSpeed),
	m_moveDirection(0,0,0),
	m_desiredVelocity(0,0,0),
	m_enableCharacterInertia(EnableCharacterInertia)
{
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::Init()
{
	//Init Shape
	m_standingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * CharacterHeightStanding + CharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * CharacterHeightStanding, CharacterRadiusStanding)).Create().Get();
	m_crouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * CharacterHeightCrouching + CharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * CharacterHeightCrouching, CharacterRadiusCrouching)).Create().Get();
	m_innerStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * CharacterHeightStanding + CharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * InnerShapeFraction * CharacterHeightStanding, InnerShapeFraction * CharacterRadiusStanding)).Create().Get();
	m_innerCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * CharacterHeightCrouching + CharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * InnerShapeFraction * CharacterHeightCrouching, InnerShapeFraction * CharacterRadiusCrouching)).Create().Get();

	
#ifdef _DEBUG
	//CapsuleShape  	Create a capsule centered around the origin with one sphere cap at (0, -inHalfHeightOfCylinder, 0) and the other at (0, inHalfHeightOfCylinder, 0)
	//元の位置は0.5f * CharacterHeightStanding + CharacterRadiusStandingなので、描画する際にまだRadius一個分ずれる必要ある
	m_debugDrawHeight = CharacterHeightStanding + 2.f * CharacterRadiusStanding;	//Capsule Height = Cylinder Height + 2 * Radius
	m_debugDrawRadius = CharacterRadiusStanding;
#endif

	Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();

	// Create 'player' character
	settings->mMaxSlopeAngle = MaxSlopeAngle;
	settings->mMaxStrength = MaxStrength;
	settings->mShape = m_standingShape;	//初期は立つ状態
	settings->mBackFaceMode = sBackFaceMode;
	settings->mCharacterPadding = CharacterPadding;
	settings->mPenetrationRecoverySpeed = PenetrationRecoverySpeed;
	settings->mPredictiveContactDistance = PredictiveContactDistance;
	settings->mSupportingVolume = Plane(Vec3::sAxisY(), -CharacterRadiusStanding); // Accept contacts that touch the lower sphere of the capsule
	settings->mEnhancedInternalEdgeRemoval = EnhancedInternalEdgeRemoval;
	settings->mInnerBodyShape = CreateInnerBody ? m_innerStandingShape : nullptr;
	settings->mInnerBodyLayer = Layers::PLAYER;
	

	//Init character
	m_pCharacter = new CharacterVirtual(settings, RVec3::sZero(), Quat::sIdentity(), 0, PhysicsManager::Instance().GetPhysicsSystem());
	m_pCharacter->SetCharacterVsCharacterCollision(&m_characterVsCharacterCollision);//プレイヤー間の衝突を有効化
	m_characterVsCharacterCollision.Add(m_pCharacter);

	//プレイヤーのCollisionListenerを設定
	for (CharacterVirtual* character : m_characterVsCharacterCollision.mCharacters)
		character->SetListener(PhysicsManager::Instance().GetPlayerContactListener());
	
}

void PlayerCharacter::Update(float deltaTime)
{
	PhysicsSystem* system = PhysicsManager::Instance().GetPhysicsSystem();
	TempAllocator* tempAllocator = PhysicsManager::Instance().GetTempAllocator();
	CharacterVirtual::ExtendedUpdateSettings update_settings;
	//bool isInWater = dynamic_cast<MyPlayerContactListener*>(m_pCharacter->GetListener())->GetInWater();

	// Basic physics when on the ground
	{
	bool playerControlsHorizontalVelocity = m_ControlMovementDuringJump || m_pCharacter->IsSupported();
	if (playerControlsHorizontalVelocity)
	{
		// Smooth the player input
		m_desiredVelocity = m_enableCharacterInertia ? 0.25f * m_moveDirection * m_moveSpeed + 0.75f * m_desiredVelocity : m_moveDirection * m_moveSpeed;

		// True if the player intended to move
		m_allowSliding = !m_moveDirection.IsNearZero();
	}
	else
	{
		// While in air we allow sliding
		m_allowSliding = true;
	}

	// Update the character rotation and its up vector to match the up vector set by the user settings
	Quat character_up_rotation = Quat::sEulerAngles(Vec3(0, 0, 0));
	m_pCharacter->SetUp(character_up_rotation.RotateAxisY());
	m_pCharacter->SetRotation(character_up_rotation);

	// A cheaper way to update the character's ground velocity,
	// the platforms that the character is standing on may have changed velocity
	m_pCharacter->UpdateGroundVelocity();

	// Determine new basic velocity
	Vec3 current_vertical_velocity = m_pCharacter->GetLinearVelocity().Dot(m_pCharacter->GetUp()) * m_pCharacter->GetUp();
	Vec3 ground_velocity = m_pCharacter->GetGroundVelocity();
	Vec3 new_velocity;
	bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;

	if (m_pCharacter->GetGroundState() == CharacterVirtual::EGroundState::OnGround	// If on ground
		&& (m_enableCharacterInertia ?
			moving_towards_ground													// Inertia enabled: And not moving away from ground
			: !m_pCharacter->IsSlopeTooSteep(m_pCharacter->GetGroundNormal())))			// Inertia disabled: And not on a slope that is too steep
	{
		// Assume velocity of ground when on ground
		new_velocity = ground_velocity;

		// Jump
		if (m_wantToJump && moving_towards_ground)
		{
			new_velocity += m_jumpSpeed * m_pCharacter->GetUp();
			m_wantToJump = false;	// Reset jump request
		}

	}
	else
		new_velocity = current_vertical_velocity;

	// Gravity
	new_velocity += (character_up_rotation * system->GetGravity()) * deltaTime;

	if (playerControlsHorizontalVelocity)
	{
		// Player input
		new_velocity += character_up_rotation * m_desiredVelocity;
	}
	else
	{
		// Preserve horizontal velocity
		Vec3 current_horizontal_velocity = m_pCharacter->GetLinearVelocity() - current_vertical_velocity;
		new_velocity += current_horizontal_velocity;
	}

	// Update character velocity
	m_pCharacter->SetLinearVelocity(new_velocity);

	//todo:if switch stance

	// Settings for our update function
	if (!EnableStickToFloor)
		update_settings.mStickToFloorStepDown = Vec3::sZero();
	else
		update_settings.mStickToFloorStepDown = -m_pCharacter->GetUp() * update_settings.mStickToFloorStepDown.Length();
	if (!EnableWalkStairs)
		update_settings.mWalkStairsStepUp = Vec3::sZero();
	else
		update_settings.mWalkStairsStepUp = m_pCharacter->GetUp() * update_settings.mWalkStairsStepUp.Length();
	}

	m_pCharacter->ExtendedUpdate(deltaTime,
		-m_pCharacter->GetUp() * system->GetGravity().Length(),
		update_settings,
		system->GetDefaultBroadPhaseLayerFilter(Layers::PLAYER),
		system->GetDefaultLayerFilter(Layers::PLAYER),
		{ },
		{ },
		*tempAllocator);

	
}

void PlayerCharacter::SetRotation(const DirectX::XMFLOAT3& rot)
{
	Quat q = Quat::sEulerAngles(Vec3(rot.x, rot.y, rot.z));	//Euler Angle→Quaternion
	m_pCharacter->SetRotation(q);
}

void PlayerCharacter::SetMoveDir(Vec3Arg& moveDir)
{
	Quat rotation = m_pCharacter->GetRotation();	//Local Rotationを獲得
	m_moveDirection = rotation * moveDir;			//進行方向を計算
}

DirectX::XMFLOAT3 PlayerCharacter::GetPosition()
{
	return DirectX::XMFLOAT3{
		m_pCharacter->GetPosition().GetX(),
		m_pCharacter->GetPosition().GetY(),
		m_pCharacter->GetPosition().GetZ()
	};
}

DirectX::XMFLOAT3 PlayerCharacter::GetEulerRotation()
{
	return DirectX::XMFLOAT3{
		m_pCharacter->GetRotation().GetEulerAngles().GetX(),
		m_pCharacter->GetRotation().GetEulerAngles().GetY(),
		m_pCharacter->GetRotation().GetEulerAngles().GetZ()
	};
}

void PlayerCharacter::SyncPlayerWorldPosition(Transform& t)
{
	t.SetPosition(GetPosition());
}

void PlayerCharacter::SetPosition(const DirectX::XMFLOAT3& pos)
{
	Vec3 position = Vec3(pos.x, pos.y, pos.z);
	m_pCharacter->SetPosition(position);
}

void PlayerCharacter::Jump()
{
	// Check player is in water or on Ground;
	bool isInWater = dynamic_cast<MyPlayerContactListener*>(m_pCharacter->GetListener())->GetInWater();
	if(!isInWater)
	{
		m_wantToJump = true;
		return;
	}

	/*
	//Check player dive in water or not
	float waterLevel = 0.f;	// todo : Get water level from buoyancy system
	if (this->GetPosition().y + PlayerParam::PlayerEyeHeight.y > waterLevel)
	{
		//Head is out of water -> Give Player a push to get back to the boat
	}else
	{
		//Give Player a constant push to float up
	}
	*/
}


