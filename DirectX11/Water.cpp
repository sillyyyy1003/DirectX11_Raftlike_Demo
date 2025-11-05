#include "Water.h"

#include "d3dUtil.h"
#include "RenderComponent.h"
#include "WaterEffect.h"

namespace 
{
	static constexpr  float WaterTransparency=0.75f;
}

Water::Water()
{
}

void Water::Init(const DirectX::XMFLOAT4& deepColor, const DirectX::XMFLOAT4& shallowColor, float maxDistance,
	float waveSpeed, float waveAmplitude, float waveLength)
{
	m_deepColor = deepColor;
	m_shallowColor = shallowColor;
	m_maxDistance = maxDistance;
	m_waveSpeed = waveSpeed;
	m_waveAmplitude = waveAmplitude;
	m_waveLength = waveLength;
}

void Water::Update(float dt)
{
	if (!m_isActive)return;

	m_time += dt;
}

void Water::Draw()
{
	if (!m_isActive)return;

	RenderComponent* render;
	if (GetComponent<RenderComponent>() != nullptr)
		render = GetComponent<RenderComponent>();
	else
		return;

	IEffect* effect = GetComponent<RenderComponent>()->GetEffect();

	WaterEffect::WaveVtxShaderCB vtxShaderCb;
	WaterEffect::WavePixShaderCB pixShaderCb;
	
	vtxShaderCb = {
		m_time,
		m_waveSpeed,		// wave speed
		m_waveAmplitude,	// wave amplitude,
		m_waveLength,	// wave length
	};

	pixShaderCb = {
		m_deepColor,		// deep color
		m_shallowColor,	// shallow color
		m_maxDistance,	// max distance
		m_time,
		WaterTransparency,
		0.0
	};

	// Set constant buffer
	dynamic_cast<WaterEffect*>(effect)->SetWaterConstantBuffer(m_transform, vtxShaderCb, pixShaderCb);

	// Draw
	render->Render(m_transform);
}

void Water::LoadFromConfig(nlohmann::json& j, const char* waterName)
{
	nlohmann::json waterData = j[waterName];
	m_deepColor = JsonToXMFLOAT4(waterData["deepColor"]);
	m_shallowColor = JsonToXMFLOAT4(waterData["shallowColor"]);
	m_maxDistance = waterData["maxDistance"];
	m_waveSpeed = waterData["waveSpeed"];
	m_waveAmplitude = waterData["waveAmplitude"];
	m_waveLength = waterData["waveLength"];
}
