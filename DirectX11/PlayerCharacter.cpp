#include "PlayerCharacter.h"

#include <imgui.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include "Layers.h"
#include "PhysicsManager.h"

namespace 
{
	static constexpr float MoveSpeed = 6.f;
	static constexpr float JumpSpeed = 4.f;
	constexpr float RotateLimit = DirectX::XM_PI * 7 / 18;	// 70度, 限制玩家上下视角旋转范围

	static constexpr float CharacterHeightStanding = 1.35f;
	static constexpr float CharacterRadiusStanding = 0.3f;
	static constexpr float CharacterHeightCrouching = 0.8f;
	static constexpr float CharacterRadiusCrouching = 0.3f;
	static constexpr float InnerShapeFraction = 0.9f;

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
	
	
}

PlayerCharacter::PlayerCharacter() :
	//m_pCharacter(nullptr),
	m_jumpSpeed(JumpSpeed),
	m_moveSpeed(MoveSpeed),
	m_moveDirection(0,0,0)
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
	m_pCharacter->SetCharacterVsCharacterCollision(&m_characterVsCharacterCollision);
	m_characterVsCharacterCollision.Add(m_pCharacter);

	for (CharacterVirtual* character : m_characterVsCharacterCollision.mCharacters)
		character->SetListener(PhysicsManager::Instance().GetPlayerContactListener());

}

void PlayerCharacter::Update(float deltaTime)
{
	bool playerControlsHorizontalVelocity = m_ControlMovementDuringJump || m_pCharacter->IsSupported();

	// A cheaper way to update the character's ground velocity,
	// the platforms that the character is standing on may have changed velocity
	m_pCharacter->UpdateGroundVelocity();

	// Settings for our update function
	CharacterVirtual::ExtendedUpdateSettings update_settings;
	if (!EnableStickToFloor)
		update_settings.mStickToFloorStepDown = Vec3::sZero();
	else
		update_settings.mStickToFloorStepDown = -m_pCharacter->GetUp() * update_settings.mStickToFloorStepDown.Length();
	if (!EnableWalkStairs)
		update_settings.mWalkStairsStepUp = Vec3::sZero();
	else
		update_settings.mWalkStairsStepUp = m_pCharacter->GetUp() * update_settings.mWalkStairsStepUp.Length();

	PhysicsSystem* system = PhysicsManager::Instance().GetPhysicsSystem();
	TempAllocator* tempAllocator = PhysicsManager::Instance().GetTempAllocator();
	m_pCharacter->ExtendedUpdate(deltaTime,
		-m_pCharacter->GetUp() * system->GetGravity().Length(),
		update_settings,
		system->GetDefaultBroadPhaseLayerFilter(Layers::PLAYER),
		system->GetDefaultLayerFilter(Layers::PLAYER),
		{ },
		{ },
		*tempAllocator);

#ifdef _DEBUG
	if(ImGui::Begin("CharacterVirtual"))
	{
		ImGui::Text("Pos:%f,%f,%f", m_pCharacter->GetPosition().GetX(), m_pCharacter->GetPosition().GetY(), m_pCharacter->GetPosition().GetZ());
		ImGui::Text("Velocity:%f,%f,%f", m_pCharacter->GetLinearVelocity().GetX(), m_pCharacter->GetLinearVelocity().GetY(), m_pCharacter->GetLinearVelocity().GetZ());
		ImGui::Text("Rotation:%f,%f,%f", RadiansToDegrees(m_pCharacter->GetRotation().GetEulerAngles().GetX()), RadiansToDegrees(m_pCharacter->GetRotation().GetEulerAngles().GetY()), RadiansToDegrees(m_pCharacter->GetRotation().GetEulerAngles().GetZ()));
	}
	ImGui::End();

#endif

}

void PlayerCharacter::SetRotation(const DirectX::XMFLOAT3& rot)
{
	Quat q = Quat::sEulerAngles(Vec3(rot.x, rot.y, rot.z));	//Euler Angle→Quaternion
	m_pCharacter->SetRotation(q);
}

void PlayerCharacter::Move(Vec3Arg& moveDir)
{
	Quat rotation = m_pCharacter->GetRotation();	//Local Rotationを獲得
	m_moveDirection = rotation * moveDir;			//進行方向を計算
	Vec3 moveStep = m_moveSpeed * m_moveDirection;	//移動量計算
	moveStep.SetY(0);								//Y軸を０にする

	m_pCharacter->SetLinearVelocity(moveStep);

}

DirectX::XMFLOAT3 PlayerCharacter::GetPosition()
{
	return DirectX::XMFLOAT3{
		m_pCharacter->GetPosition().GetX(),
		m_pCharacter->GetPosition().GetY(),
		m_pCharacter->GetPosition().GetZ()
	};
}


