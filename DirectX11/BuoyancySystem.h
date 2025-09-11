#pragma once
#include <DirectXCollision.h>
#include <Jolt.h>
#include <Core/Mutex.h>
#include <Physics/Body/BodyManager.h>
#include <Physics/Collision/ContactListener.h>
#include "RenderComponent.h"

using namespace JPH;

class BuoyancySystem:
	public ContactListener
{
public:
	BuoyancySystem();
	~BuoyancySystem() override = default;

	/// @brief Init buoyancy physics
	/// @param waterWidth Width of the water box
	/// @param waterHeight height of the water box
	void Init(float waterWidth, float waterHeight);

	/// @brief Init buoyancy physics& render data
	/// @param waterWidth Width of the water box
	/// @param waterHeight height of the water box
	/// @param mat Water Material
	/// @param effect Water effect using transparent shader
	void Init(float waterWidth, float waterHeight, Material* mat, IEffect* effect);


	virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
	virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;

	void PreUpdate(float dt);
	void Draw();

	/// @brief 時間に沿って、水現在の高さを取得する
	/// @param pos 頂点・オブジェクトの位置
	/// @return 波による水面の位置
	DirectX::XMFLOAT3 GetWaterSurfacePosition(const DirectX::XMFLOAT3& pos) const;
	RVec3 GetWaterSurfacePosition(RVec3Arg inXZPosition) const;

private:
	//==== physics data
	Mutex m_bodiesInWaterMutex;
	BodyIDVector m_bodiesInWater;
	//std::vector<BodyID> m_bodiesInWater;
	BodyID m_waterSensor;

	//==== render data
	std::unique_ptr<PlaneMesh> m_pMesh;
	std::unique_ptr<RenderComponent> m_pRenderComponent;
	Transform m_transform;

	float m_time = 0.f;
};

