#pragma once
#include "IEffect.h"

class SkyboxEffect:
	public IEffect
{
public:
	SkyboxEffect();
	~SkyboxEffect() override = default;


	void Apply() override;
	void ApplyRenderState();

	void InitPixelShader(PixelShader* _ps) { m_ps = _ps; };
	void InitVertexShader(VertexShader* _vs) { m_vs = _vs; };
	void InitTexture(Texture* tex) { m_skyboxTex = tex; }
	void InitCamera(CameraBase* _camera) { m_pCamera = _camera; };

	void SetTextureCB();
	void SetCameraCB(CameraBase* camera = nullptr) override;
	void SetDirLightCB(const DirectX::XMFLOAT3& light);

private:
	Texture* m_skyboxTex;
	PixelShader* m_ps;
	VertexShader* m_vs;
	CameraBase* m_pCamera;
};

