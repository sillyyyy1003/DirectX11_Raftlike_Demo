#pragma once
#include <DirectXCollision.h>
#include <Jolt.h>
#include <Physics/EActivation.h>
#include <Physics/Body/BodyCreationSettings.h>
#include <Physics/Body/BodyID.h>
#include "Component.hpp"
#include "GameObject.h"
#include "Layers.h"
#include "Transform.h"

/// @brief 物理コンポーネント
///	Now Only box collider
///	todo: SphereCollider, CapsuleCollider, MeshCollider, etc.
class PhysicsComponent :public Component
{
public:
	enum class Type :uint8_t
	{
		Box,			///< Box Collider
		//Sphere,		///< Sphere Collider
		//Capsule,		///< Capsule Collider
		//Mesh,			///< Mesh Collider
	};
private:

	JPH::BodyID m_bodyID;
	GameObject* m_pGameObject;	// 所属しているGameObjectを設定
	DirectX::XMFLOAT3 m_bodySize;	// Body Size

public:
	static constexpr MyComponent::ComponentType TYPE = MyComponent::ComponentType::Physics;

	PhysicsComponent();
	~PhysicsComponent() override;

	void Init(const BodyCreationSettings& settings,EActivation activation);
	void Init(BodyID id);
	void Init(const BodyCreationSettings& settings, EActivation activation, GameObject* obj);
	void Init(const BodyCreationSettings& settings, EActivation activation, GameObject* obj, const DirectX::XMFLOAT3& bodySize);

	BodyID GetBodyID()const { return m_bodyID; }

	/// @brief 位置設定
	/// @param pos 位置
	/// @param eActivation 状態変更 Default:状態変更しない
	void SetPosition(const DirectX::XMFLOAT3& pos, JPH::EActivation eActivation = JPH::EActivation::DontActivate);

	void SetPosition(float x, float y, float z, JPH::EActivation eActivation = JPH::EActivation::DontActivate);
	void SetPosition(float* pos, JPH::EActivation eActivation = JPH::EActivation::DontActivate);

	/// @brief 回転設定
	/// @param rot EulerAngle(Radian)
	/// @param eActivation 
	void SetRotation(const DirectX::XMFLOAT3& rot, JPH::EActivation eActivation = JPH::EActivation::DontActivate);
	void SetRotation(float x, float y, float z, JPH::EActivation eActivation = JPH::EActivation::DontActivate);
	void SetRotation(float* rot, JPH::EActivation eActivation = JPH::EActivation::DontActivate);

	/// @brief 更新した位置と回転をTransformに反映
	void SyncPhysicsToTransform(Transform& transform);
	/// @brief 更新したTransformを物理に反映
	void SyncTransformToPhysics(const Transform& transform);
	/// @brief プレイヤーのTransformを物理に反映
	///	位置＆Y軸回転のみ反映
	void PlayerSyncTransformToPhysics(const Transform& transform);

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;

	/// @brief Only Apply on Dynamic Body to add force to the body.
	void AddForce(const DirectX::XMFLOAT3& vec3);
	void AddForce(float x, float y, float z);
	void AddForce(float* vec3);
	void AddForce(const RVec3& vec3);

	/// @brief Only Apply on Kinematic body to set velocity of body.
	void SetVelocity(const DirectX::XMFLOAT3& vec3);
	void SetVelocity(float x, float y, float z);
	void SetVelocity(float* vec3);

	void SetGameObject(GameObject* obj) { m_pGameObject = obj; }
	GameObject* GetGameObject() const { return m_pGameObject; }

	EMotionType GetEmotionType();

	void ActivatePhysics();
	void DeActivePhysics();

	ObjectLayer GetLayer() const;

	DirectX::XMFLOAT3 GetBodySize() const { return m_bodySize; }
};

