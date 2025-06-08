#pragma once
#include "LightBase.h"
class PointLight :
    public LightBase
{
protected:
	float m_range = 0.0f;					//Lightの範囲
	DirectX::XMFLOAT3  m_attenuation = {};	//減衰


	PointLight();
	~PointLight() override = default;

	void Update(float dt) override;

	float GetRange() { return m_range; };
	DirectX::XMFLOAT3 GetAttenuation() const{ return m_attenuation; };


	void SetRange(float _range);
	void SetAttenuation(const DirectX::XMFLOAT3& _attenuation);
	void SetAttenuation(float x, float y, float z);
	void SetAttenuation(const float* attenuation);

	Light::PointLight GetPointLight();
};

