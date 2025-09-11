#include "Water.h"

#include "d3dUtil.h"
#include "RenderComponent.h"
#include "WaterEffect.h"


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
	if (GetComponent<RenderComponent>(MyComponent::ComponentType::Render) != nullptr)
		render = GetComponent<RenderComponent>(MyComponent::ComponentType::Render);
	else
		return;

	IEffect* effect = GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->GetEffect();

	WaterEffect::WaveVtxShaderCB vtxShaderCb;
	WaterEffect::WavePixShaderCB pixShaderCb;
	//vtxShaderCb = {
	//	m_time,
	//	WaveSpeed,		// wave speed
	//	WaveAmplitude,	// wave amplitude,
	//	WaveLength,	// wave length
	//};

	vtxShaderCb = {
		m_time,
		m_waveSpeed,		// wave speed
		m_waveAmplitude,	// wave amplitude,
		m_waveLength,	// wave length
	};


	//pixShaderCb = {
	//	DeepColor,		// deep color
	//	ShallowColor,	// shallow color
	//	MaxDistance,	// max distance
	//	m_time,
	//	0.f,0.0
	//};

	pixShaderCb = {
		m_deepColor,		// deep color
		m_shallowColor,	// shallow color
		m_maxDistance,	// max distance
		m_time,
		0.f,0.0
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
