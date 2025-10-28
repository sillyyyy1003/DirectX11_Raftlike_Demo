#include "UIChargeEffect.h"

#include "GameApp.h"
#include "RenderState.h"

UIChargeEffect::UIChargeEffect():
	m_ps(nullptr),
	m_vs(nullptr),
	m_viewSize({ WIN_WIDTH,WIN_HEIGHT })
{
}

void UIChargeEffect::Apply()
{
	assert(m_ps != nullptr);
	assert(m_vs != nullptr);

	//Bind 
	m_vs->Bind();
	m_ps->Bind();

	ApplyRenderState();
}

void UIChargeEffect::SetWVPMatrixCB(const Transform& t, CameraBase* camera)
{
	// WVP 計算
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = t.GetLocalToWorldMatrix();
	DirectX::XMMATRIX view = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&mat[1], view);
	DirectX::XMMATRIX proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(m_viewSize.x, m_viewSize.y, 0.1f, 3.0f));
	XMStoreFloat4x4(&mat[2], proj);

	m_vs->WriteBuffer(0, mat);
}

void UIChargeEffect::SetConstantBuffer(Transform& transform, Material* material)
{
	SetWVPMatrixCB(transform);
	SetMaterial(material);
}

void UIChargeEffect::SetMaterial(Material* mat)
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
		mat->GetEmission().w,0,0
	};
	// Set Material to Pixel Shader
	m_ps->WriteBuffer(0, &material);

	// Textureあればシェーダーに渡す
	if (isTexEnable)m_ps->SetTexture(0, mat->GetTexture(::Material::Albedo));
}

void UIChargeEffect::ApplyRenderState()
{
	GameApp::SetBlendState(RenderStates::BSTransparent);		//AlphaBlend
	GameApp::SetSamplerState(RenderStates::SSLinearWrap);		//Default Sampling
	GameApp::SetCullingMode(nullptr);	//表だけ
}
