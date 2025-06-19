#pragma once
#include "CameraBase.h"
#include "DirLight.h"
#include "Material.h"
#include "PointLight.h"
#include "Shader.h"

/// <summary>
/// レンダリングのデータを扱う 基底クラス
/// </summary>
class IEffect
{
public:
	IEffect();
	virtual ~IEffect();
	virtual void Apply() = 0;

	
	virtual void SetMaterial(Material* mat){};
	virtual void SetWVPMatrix(const Transform& t, CameraBase* camera = nullptr){};
	virtual void SetDirLight() {}
	virtual void SetCameraCB(CameraBase* camera = nullptr) {};
	

};

/// <summary>
/// 基本レンダリング
///	方向光　点光源含め
///	Blinn-Phong適用
///	MaterialにAlbedoTexのみ
///	透視投影 (WVP)
///	RenderState:Basic
/// </summary>
class BasicEffect :public IEffect
{
protected:

	PixelShader* m_ps;
	VertexShader* m_vs;

	CameraBase* m_pCamera;
	LightBase* m_pDirLight;

public:
	BasicEffect() = default;
	~BasicEffect() override = default;

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
	void SetMaterial(Material* mat)override;

	/// @brief シェーダーに渡すライトデータ
	void SetDirLight()override;

	/// @brief カメラConstantBufferを設定する
	/// @param camera 
	void SetCameraCB(CameraBase* camera = nullptr) override;

	/// @brief シェーダーにWVPデータを渡す
	void SetWVPMatrix(const Transform& t, CameraBase* camera = nullptr)override;

	/// @brief シェーダーに渡すライトデータ
	/// @param light  
	void SetDirLight(DirLight* light);
	//todo:Set to Pixel Shader
	void SetPointLight(std::vector<PointLight*> pointLights);
};
