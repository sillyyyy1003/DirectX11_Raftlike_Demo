#pragma once
#include "Cube.h"

class SkyboxEffect;

class Skybox
{
public:
	Skybox();
	~Skybox() = default;

	SkyboxEffect* GetSkyboxEffect() const { return m_pSkyboxEffect; }
	void Init(SkyboxEffect* effect);
	void Update(float dt);
	void Draw();

private:
	std::unique_ptr<SkyBoxCube> m_pSkyBoxCube;
	SkyboxEffect* m_pSkyboxEffect;
	

};

