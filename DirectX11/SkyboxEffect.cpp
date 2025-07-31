#include "SkyboxEffect.h"

#include "GameApp.h"
#include "RenderState.h"

SkyboxEffect::SkyboxEffect():
	m_skyboxTex(nullptr),
	m_ps(nullptr),
	m_vs(nullptr)
{
}

void SkyboxEffect::Apply()
{
	//null check
	assert(m_ps != nullptr);
	assert(m_vs != nullptr);

	//Bind 
	m_vs->Bind();
	m_ps->Bind();

	ApplyRenderState();
}

void SkyboxEffect::ApplyRenderState()
{
	GameApp::SetBlendState(nullptr);
	GameApp::SetCullingMode(RenderStates::RSNoCull);
	GameApp::SetDepthStencilState(RenderStates::DSSLessEqual);
}

void SkyboxEffect::SetTextureCB()
{
	assert(m_skyboxTex != nullptr);
	
	// Set texture cube to the pixel shader
	m_ps->SetTexture(0, m_skyboxTex);
}

void SkyboxEffect::SetCameraCB(CameraBase* camera)
{
	// Set camera data to vertex shader
	DirectX::XMFLOAT4X4 mat[2];
	if(camera!=nullptr)
	{
		mat[0] = camera->GetViewXMF();
		mat[1] = camera->GetProjXMF();
	}else
	{
		mat[0] = m_pCamera->GetViewXMF();
		mat[1] = m_pCamera->GetProjXMF();
	}

	m_vs->WriteBuffer(0, mat);

}
