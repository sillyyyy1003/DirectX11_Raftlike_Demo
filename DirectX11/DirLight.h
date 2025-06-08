#pragma once
#include "LightBase.h"
class DirLight :
    public LightBase
{
public:
	DirLight() = default;
	~DirLight() override = default;

	void Update(float dt) override;

};

