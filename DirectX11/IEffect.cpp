#include "IEffect.h"
#include "DebugLog.h"
#include "GameApp.h"
#include "RenderState.h"


IEffect::IEffect()
{
}

IEffect::~IEffect()
{
}

void BasicEffect::ApplyRenderState()
{
	GameApp::SetBlendState(RenderStates::BSTransparent);		//AlphaBlend
	GameApp::SetSamplerState(RenderStates::SSLinearWrap);		//Default Sampling
	GameApp::SetCullingMode(RenderStates::RSCullClockWise);	//表だけ
}

void BasicEffect::Apply()
{

	//null check
	assert(m_ps!=nullptr);
	assert(m_vs!=nullptr);

	//Bind 
	m_vs->Bind();
	m_ps->Bind();

	ApplyRenderState();

}

void BasicEffect::SetMaterial(Material* mat)
{
	struct Material
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
	};

	Material material = {
		mat->GetAmbient(),
		mat->GetDiffuse(),
		mat->GetSpecular()
	};

	//Set Material
	m_ps->WriteBuffer(	1, &material);

	//Set Texture
	m_ps->SetTexture(0, mat->GetTexture(::Material::Albedo));
}

void BasicEffect::SetDirLight(DirLight* light)
{
	struct Light {
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 lightPos;
		DirectX::XMFLOAT4 lightDir;
	};
	Light l = {

		light->GetAmbient(),
		light->GetDiffuse(),
		{light->GetPosition().x,light->GetPosition().y,light->GetPosition().z,1.f},
		{light->GetDirection().x,light->GetDirection().y,light->GetDirection().z,1.f
	   }
	};

	m_ps->WriteBuffer(0, &l);
}

void BasicEffect::SetDirLight()
{
	struct Light {
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 lightPos;
		DirectX::XMFLOAT4 lightDir;
	};

	Light light = {

	   m_pDirLight->GetAmbient(),
	   m_pDirLight->GetDiffuse(),
	   {m_pDirLight->GetPosition().x,m_pDirLight->GetPosition().y,m_pDirLight->GetPosition().z,1.f},
	   {m_pDirLight->GetDirection().x,m_pDirLight->GetDirection().y,m_pDirLight->GetDirection().z,1.f
	  }
	};
	m_ps->WriteBuffer(0, &light);
}

void BasicEffect::SetPointLight(std::vector<PointLight*> pointLights)
{

}

void BasicEffect::SetCameraMatrix(CameraBase* camera)
{
	if (camera != nullptr)
	{
		DirectX::XMFLOAT4 camPos = {
			camera->GetPos().x,
			camera->GetPos().y,
			camera->GetPos().z,
			1.f
		};

		m_ps->WriteBuffer(2, &camPos);
	
	}else
	{
#ifdef _DEBUG
		if (m_pCamera == nullptr)
			DebugLog::LogError("Camera is null");
#endif

		DirectX::XMFLOAT4 camPos = {
			m_pCamera->GetPos().x,
			m_pCamera->GetPos().y,
			m_pCamera->GetPos().z,
		1.f
		};

		m_ps->WriteBuffer(2, &camPos);
	}
}

/*
void BasicEffect::SetCameraMatrix()
{
	if(m_pCamera)
	{
		DirectX::XMFLOAT4 camPos = {
			m_pCamera->GetPos().x,
			m_pCamera->GetPos().y,
			m_pCamera->GetPos().z,
		1.f
		};

		m_ps->WriteBuffer(2, &camPos);
	}
}
*/

void BasicEffect::SetWVPMatrix(const Transform& t, CameraBase* camera)
{
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = t.GetLocalToWorldMatrix();
	mat[1]=camera->GetViewXMF();
	mat[2]=camera->GetProjXMF();

	m_vs->WriteBuffer(0, mat);
}

void BasicEffect::SetWVPMatrix(const Transform& t)
{
	DirectX::XMFLOAT4X4 mat[3];
	mat[0] = t.GetLocalToWorldMatrix();
	mat[1] = m_pCamera->GetViewXMF();
	mat[2] = m_pCamera->GetProjXMF();

	m_vs->WriteBuffer(0, mat);
}

