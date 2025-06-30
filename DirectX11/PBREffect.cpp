#include "PBREffect.h"

#include "DebugLog.h"
#include "GameApp.h"
#include "RenderState.h"

PBREffect::PBREffect():
m_ps(nullptr),
m_vs(nullptr),
m_pCamera(nullptr),
m_pDirLight(nullptr)
{
}

void PBREffect::ApplyRenderState()
{
	GameApp::SetBlendState(RenderStates::BSTransparent);		//AlphaBlend
	GameApp::SetSamplerState(RenderStates::SSLinearWrap);		//Default Sampling
	GameApp::SetCullingMode(RenderStates::RSNoCull);	//表だけ
}

void PBREffect::Apply()
{
	//null check
	assert(m_ps != nullptr);
	assert(m_vs != nullptr);

	//Bind 
	m_vs->Bind();
	m_ps->Bind();

	ApplyRenderState();
}

void PBREffect::SetMaterial(Material* mat)
{
	//Set Texture
	//Albedo
	m_ps->SetTexture(0, mat->GetTexture(::Material::Albedo));
	//NormalMap
	m_ps->SetTexture(1, mat->GetTexture(::Material::Normal));
	//MetallicRoughnessMap
	m_ps->SetTexture(2, mat->GetTexture(::Material::Metallic_Smooth));
}

void PBREffect::SetDirLight()
{
	struct Light {
		
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 lightPos;
		float lightIntensity;			//光の強さ

	};

	Light light = {
	   m_pDirLight->GetDiffuse(),
	   {m_pDirLight->GetPosition().x,m_pDirLight->GetPosition().y,m_pDirLight->GetPosition().z},
		m_pDirLight->GetIntensity()

	};

	//cbuffer DirLight:register(b1)
	m_ps->WriteBuffer(1, &light);
}

void PBREffect::SetCameraCB(CameraBase* camera)
{
	//cbuffer Camera : register(b0)
	if (camera != nullptr)
	{
		DirectX::XMFLOAT4 camPos = {
			camera->GetPos().x,
			camera->GetPos().y,
			camera->GetPos().z,
			1.f
		};

		m_ps->WriteBuffer(0, &camPos);
	}
	else
	{
#if defined(_DEBUG) || defined(DEBUG)
		if (m_pCamera == nullptr)
			DebugLog::LogError("Camera is null");
#endif

		DirectX::XMFLOAT4 camPos = {
			m_pCamera->GetPos().x,
			m_pCamera->GetPos().y,
			m_pCamera->GetPos().z,
		1.f
		};

		m_ps->WriteBuffer(0, &camPos);
	}
}

void PBREffect::SetWVPMatrix(const Transform& t, CameraBase* camera)
{
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = t.GetLocalToWorldMatrix();
	if (camera != nullptr)
	{
		mat[1] = camera->GetViewXMF();
		mat[2] = camera->GetProjXMF();
	}
	else
	{
#if defined(_DEBUG) || defined(DEBUG)
		if (m_pCamera == nullptr)
			DebugLog::LogError("Camera is null");
#endif
		assert(m_pCamera != nullptr);
		mat[1] = m_pCamera->GetViewXMF();
		mat[2] = m_pCamera->GetProjXMF();
		
	}

	m_vs->WriteBuffer(0, mat);
}


