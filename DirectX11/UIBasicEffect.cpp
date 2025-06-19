#include "UIBasicEffect.h"
#include "GameApp.h"
#include "RenderState.h"

UIBasicEffect::UIBasicEffect():
m_ps(nullptr),
m_vs(nullptr),
m_viewSize({ WIN_WIDTH,WIN_HEIGHT})
{
}

void UIBasicEffect::Apply()
{
	assert(m_ps != nullptr);
	assert(m_vs != nullptr);

	//Bind 
	m_vs->Bind();
	m_ps->Bind();

	ApplyRenderState();
}

void UIBasicEffect::SetWVPMatrix(const Transform& t, CameraBase* camera)
{
	// WVP 計算
	DirectX::XMMATRIX WVP[3];
	WVP[0] = t.GetLocalToWorldMatrixXM();
	WVP[1] = DirectX::XMMatrixIdentity();
	WVP[2]= DirectX::XMMatrixOrthographicLH(m_viewSize.x, m_viewSize.y, 0.1f, 3.0f);

	m_vs->WriteBuffer(0, WVP);

}

void UIBasicEffect::SetMaterial(Material* mat)
{
	struct Material
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		float isTexEnable;
		float pad[3];
	};

	bool isTexEnable = mat->GetTextureEnable();

	Material material = {
		mat->GetAmbient(),
		mat->GetDiffuse(),
		(float)isTexEnable,
		0,0,0
	};
	// Set Material to Pixel Shader
	m_ps->WriteBuffer(0, &material);

	// Textureあればシェーダーに渡す
	if (isTexEnable)m_ps->SetTexture(0, mat->GetTexture(::Material::Albedo));
}

void UIBasicEffect::ApplyRenderState()
{
	GameApp::SetBlendState(RenderStates::BSTransparent);		//AlphaBlend
	GameApp::SetSamplerState(RenderStates::SSLinearWrap);		//Default Sampling
	GameApp::SetCullingMode(nullptr);	//表だけ
}
