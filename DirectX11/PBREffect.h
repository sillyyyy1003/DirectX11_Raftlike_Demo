#pragma once
#include "IEffect.h"

/// @brief PBR Effect
/// 方向光　点光源
/// MaterialにAlbedo, Normal,Metallic, AOを持つ
/// 透視投影
/// Default RenderState
class PBREffect :
    public IEffect
{
protected:
	// Shader
	PixelShader* m_ps;
	VertexShader* m_vs;

	// Camera
	CameraBase* m_pCamera;

	// Light
	LightBase* m_pDirLight;
	//PointLight

public:
	PBREffect();
	~PBREffect() override = default;
	void InitPixelShader(PixelShader* _ps) { m_ps = _ps; };
	void InitVertexShader(VertexShader* _vs) { m_vs = _vs; };
	void InitCamera(CameraBase* _camera) { m_pCamera = _camera; };
	void InitEffectDirLight(LightBase* _light) { m_pDirLight = _light; };

	/// @brief RenderState·Cull State·SamplerStateを設定する
	void ApplyRenderState();

	/// @brief Shader Bind
	void Apply()override;

	/// @brief シェーダーに渡すマテリアルデータ
	/// @param mat 
	void SetMaterial(Material* mat) override;

	/// @brief シェーダーに渡すライトデータ
	void SetDirLight()override;

	/// @brief カメラ
	/// @param camera 
	void SetCameraCB(CameraBase* camera = nullptr)override;

	/// @brief シェーダーにWVPデータを渡す
	void SetWVPMatrix(const Transform& t, CameraBase* camera = nullptr)override;
};



