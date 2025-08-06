#pragma once
#include <DirectXMath.h>
#include "SceneBase.hpp"

class CameraBase;
class SceneTitle:
	public SceneBase
{
public:
	
	SceneTitle() = default;
	~SceneTitle() override = default;

	void Init() override;
	void UnInit() override;
	void Update(float tick) override;
	void Draw() override;

private:
	CameraBase* m_pCurrentCamera;

	DirectX::XMFLOAT3 m_cameraRotation = { 0,0,0 };
};

