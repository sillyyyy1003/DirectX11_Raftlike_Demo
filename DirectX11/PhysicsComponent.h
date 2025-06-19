#pragma once
#include <DirectXCollision.h>
#include <Jolt.h>
#include <Physics/EActivation.h>
#include <Physics/Body/BodyID.h>

#include "Component.hpp"
#include "Layers.h"

class PhysicsComponent:public Component
{
protected:

	JPH::BodyID m_bodyID;	

public:

	PhysicsComponent();
	~PhysicsComponent();

	void Init();

	/// @brief 位置設定
	/// @param pos 位置
	/// @param eActivation 状態変更 Default:状態変更しない
	void SetPosition(const DirectX::XMFLOAT3& pos, JPH::EActivation eActivation = JPH::EActivation::DontActivate);

	void SetPosition(float x, float y, float z, JPH::EActivation eActivation = JPH::EActivation::DontActivate)
	{
		SetPosition(DirectX::XMFLOAT3(x, y, z), eActivation);
	}

	void SetPosition(float* pos, JPH::EActivation eActivation = JPH::EActivation::DontActivate)
	{
		SetPosition(DirectX::XMFLOAT3(pos[0], pos[1], pos[2]), eActivation);
	}

	/// @brief 回転設定
	/// @param rot EulerAngle(Radian)
	/// @param eActivation 
	void SetRotation(const DirectX::XMFLOAT3& rot, JPH::EActivation eActivation = JPH::EActivation::DontActivate);
	void SetRotation(float x, float y, float z, JPH::EActivation eActivation = JPH::EActivation::DontActivate)
	{
		SetRotation(DirectX::XMFLOAT3(x, y, z), eActivation);
	}
	void SetRotation(float* rot, JPH::EActivation eActivation = JPH::EActivation::DontActivate)
	{
		SetRotation(DirectX::XMFLOAT3(rot[0], rot[1], rot[2]), eActivation);
	}


};

