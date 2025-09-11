#pragma once
#include "IEffect.h"

/// <summary>
/// Water Effect
/// </summary>
class WaterEffect :
    public IEffect
{
public:
    struct WaveVtxShaderCB
    {
        float time;
        float waveSpeed;
		float waveHeight;
        float waveLength;
    };


    struct WavePixShaderCB
    {
        DirectX::XMFLOAT4 deepColor;
		DirectX::XMFLOAT4 shallowColor;

        float maxDistance;
        float time;
        float padding[2];
    };

    WaterEffect();
    ~WaterEffect() override;

    /// @brief Bind vs/ps & set render state
    void Apply() override;

    /// @brief RenderState·Cull State·SamplerStateを設定する
    void ApplyRenderState();

    void SetWVPMatrixCB(const Transform& t, CameraBase* camera = nullptr) override;
    void SetConstantBuffer(Transform& transform, Material* material) override {};

    /// @brief シェーダーに渡すライトデータ
    void SetDirLightCB()override;

    void SetWaterConstantBuffer(const Transform& transform,WaveVtxShaderCB vtxShaderCb, WavePixShaderCB pixShaderCb);

    void Init(VertexShader* vs,PixelShader* ps,CameraBase* camera,Texture* normalTex, LightBase* pLight);

private:
    CameraBase* m_pCamera;
    VertexShader* m_vs;
	PixelShader* m_ps;

	Texture* m_tex;             // normal map texture
    LightBase* m_pDirLight;     // Light

};

