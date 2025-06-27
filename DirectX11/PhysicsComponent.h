#pragma once
#include <DirectXCollision.h>
#include <Jolt.h>
#include <Physics/EActivation.h>
#include <Physics/Body/BodyCreationSettings.h>
#include <Physics/Body/BodyID.h>

#include "Component.hpp"
#include "Layers.h"
#include "Transform.h"

/// @brief 物理コンポーネント
///	Now Only box collider
///	todo: SphereCollider, CapsuleCollider, MeshCollider, etc.
class PhysicsComponent:public Component
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
	float m_mass;		//Dynamic Objectの質量


public:

	PhysicsComponent();
	~PhysicsComponent();

	void Init(const BodyCreationSettings& settings,EActivation activation);
	void init(BodyID id);

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
	void SyncTransformToPhysics(const Transform& transform);

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

	float GetMass()const;

};

