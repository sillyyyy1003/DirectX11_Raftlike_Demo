#pragma once
#include "Cube.h"

class SkyboxEffect;

class Skybox
{
public:
	Skybox();
	~Skybox() = default;

	void Init(SkyboxEffect* effect);
	void Draw();
private:
	std::unique_ptr<SkyBoxCube> m_pSkyBoxCube;
	SkyboxEffect* m_pSkyboxEffect;

};

