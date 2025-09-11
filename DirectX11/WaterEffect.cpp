#include "WaterEffect.h"
#include "d3dUtil.h"
#include "DebugLog.h"
#include "GameApp.h"
#include "RenderState.h"

WaterEffect::WaterEffect():
	m_pCamera(nullptr),
	m_vs(nullptr),
	m_ps(nullptr),
	m_tex(nullptr)
{
}

WaterEffect::~WaterEffect()
{
}

void WaterEffect::Apply()
{	//null check
	assert(m_ps != nullptr);
	assert(m_vs != nullptr);

	//Bind 
	m_vs->Bind();
	m_ps->Bind();

	ApplyRenderState();
}

void WaterEffect::ApplyRenderState()
{
	GameApp::SetBlendState(RenderStates::BSTransparent);		//AlphaBlend
	GameApp::SetSamplerState(RenderStates::SSLinearWrap);		//Default Sampling
	GameApp::SetCullingMode(RenderStates::RSNoCull);	//表だけ
}

void WaterEffect::SetWVPMatrixCB(const Transform& t, CameraBase* camera)
{
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = t.GetLocalToWorldMatrix();

#if defined(_DEBUG) || defined(DEBUG)
	if (m_pCamera == nullptr)
		DebugLog::LogError("Camera is null");
#endif

	assert(m_pCamera != nullptr);
	mat[1] = m_pCamera->GetViewXMF();
	mat[2] = m_pCamera->GetProjXMF();

	m_vs->WriteBuffer(0, mat);
}


void WaterEffect::SetDirLightCB()
{
	struct Light {
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 lightDir;
		float lightIntensity;			//光の強さ
	};
	DirectX::XMFLOAT3 lightDir = (m_pDirLight->GetPosition()) * -1;

	Light light = {
		m_pDirLight->GetAmbient(),
	   m_pDirLight->GetDiffuse(),
	   lightDir,
		m_pDirLight->GetIntensity()
	};

	//cbuffer DirLight:register(b1)
	m_ps->WriteBuffer(1, &light);
}

void WaterEffect::SetWaterConstantBuffer(const Transform& transform, WaveVtxShaderCB vtxShaderCb,WavePixShaderCB pixShaderCb)
{
	// Set WVP
	SetWVPMatrixCB(transform);	// slot0

	// Set wave cb to vertex shader;
	m_vs->WriteBuffer(1, &vtxShaderCb);	//slot1

	// set player pos to 
	DirectX::XMFLOAT4 cameraPos = {
		m_pCamera->GetPos().x,
		m_pCamera->GetPos().y,
		m_pCamera->GetPos().z,
		1.f
	};
	m_vs->WriteBuffer(2, &cameraPos);

	// Set texture to pixel Shader;
	m_ps->SetTexture(0, m_tex);

	// Set wave cb to pixel shader;
	m_ps->WriteBuffer(0, &pixShaderCb);		// slot0

	SetDirLightCB();	// slot1

}

void WaterEffect::Init(VertexShader* vs, PixelShader* ps, CameraBase* camera, Texture* normalTex, LightBase* pLight)
{
	m_vs = vs;
	m_ps = ps;
	m_pCamera = camera;
	m_tex = normalTex;
	m_pDirLight = pLight;
}
