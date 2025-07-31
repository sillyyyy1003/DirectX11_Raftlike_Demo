#include "Skybox.h"

#include "SkyboxEffect.h"

Skybox::Skybox():
	m_pSkyBoxCube(nullptr),
	m_pSkyboxEffect(nullptr)
{
}

void Skybox::Init(SkyboxEffect* effect)
{
	m_pSkyboxEffect = effect;
	m_pSkyBoxCube = std::make_unique<SkyBoxCube>();
	m_pSkyBoxCube->Init();
}

void Skybox::Draw()
{
	assert(m_pSkyboxEffect != nullptr);
	assert(m_pSkyBoxCube != nullptr);

	m_pSkyboxEffect->SetCameraCB();
	m_pSkyboxEffect->SetTextureCB();

	m_pSkyboxEffect->Apply();
	m_pSkyBoxCube->Draw();
}
