#include "SkyboxEffect.h"

#include "d3dUtil.h"
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
	GameApp::SetDepthStencilState(RenderStates::DSSNoDepthTest);
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
	DirectX::XMFLOAT4X4 mat[3];

	// world matrix はカメラの位置のみ
	DirectX::XMFLOAT3 position = camera ? camera->m_transform.GetPosition() : m_pCamera->m_transform.GetPosition();
	DirectX::XMVECTOR positionVec = XMLoadFloat3(&position);
	DirectX::XMMATRIX World = DirectX::XMMatrixTranslationFromVector(positionVec);
	World = DirectX::XMMatrixTranspose(World);
	DirectX::XMFLOAT4X4 res;
	XMStoreFloat4x4(&res, World);


	if(camera!=nullptr)
	{
		mat[0] = res;
		mat[1] = camera->GetViewXMF();
		mat[2] = camera->GetProjXMF();
	}else
	{

		mat[0] = res;
		mat[1] = m_pCamera->GetViewXMF();
		mat[2] = m_pCamera->GetProjXMF();
	}

	m_vs->WriteBuffer(0, mat);

}

void SkyboxEffect::SetDirLightCB(const DirectX::XMFLOAT3& light)
{
}
