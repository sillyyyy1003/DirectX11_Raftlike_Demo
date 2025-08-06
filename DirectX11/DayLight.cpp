#include "DayLight.h"
#include "d3dUtil.h"

namespace
{
	static constexpr DirectX::XMFLOAT3 DefaultLightPositionLeft = { -10.f, 0, 0 }; // Default Light Position
    static constexpr DirectX::XMFLOAT3 DefaultLightPositionTop = { 0, 10, 0 }; // Default Light Position
    static constexpr DirectX::XMFLOAT3 DefaultLightPositionRight = { 10.f, 0, 0 }; // Default Light Position

    static constexpr float DayLightIntensityMax = 3.5f;
    static constexpr float StartLightIntensity = 1.5f;
	static constexpr float NightLightIntensityMin = 1.f;
    static constexpr float DayLength = 1200.f;
    static constexpr float DayTime = 600.f;
    static constexpr float NightTime = 600.f;
    static constexpr float DayLightFadeTime = 60.f; 
}

void DayLight::Update(float dt)
{
	m_tick += dt;

	float timeOfDay = fmodf(m_tick, DayLength);


	if (timeOfDay < DayTime)
	{
		// Daytime
		float t = timeOfDay;

		if (t < DayLightFadeTime) 
		{
			float f = t / DayLightFadeTime;
			float smooth = EaseOutCubic(f);
			m_intensity = StartLightIntensity + (DayLightIntensityMax - StartLightIntensity) * smooth;

			m_pos = Lerp(DefaultLightPositionLeft, DefaultLightPositionTop, smooth);
		}
		else if (t < DayTime - DayLightFadeTime) 
		{
			m_intensity = DayLightIntensityMax;
			m_pos = DefaultLightPositionTop;
		}
		else 
		{
			float f = (t - (DayTime - DayLightFadeTime)) / DayLightFadeTime;
			float smooth = EaseOutCubic(f);
			m_intensity = DayLightIntensityMax - (DayLightIntensityMax - StartLightIntensity) * smooth;

			m_pos = Lerp(DefaultLightPositionTop, DefaultLightPositionRight, smooth);
		}
	}
	else
	{
		// Night
		float nightTime = timeOfDay - DayTime;
		float t = nightTime;

		if (t < DayLightFadeTime) 
		{
			float f = t / DayLightFadeTime;
			float smooth = EaseOutCubic(f);
			m_intensity = StartLightIntensity - (StartLightIntensity - NightLightIntensityMin) * smooth;

			m_pos = Lerp(DefaultLightPositionRight, DefaultLightPositionTop, smooth);
		}
		else if (t < NightTime - DayLightFadeTime) 
		{
			m_intensity = NightLightIntensityMin;
			m_pos = DefaultLightPositionTop;
		}
		else 
		{
			float f = (t - (NightTime - DayLightFadeTime)) / DayLightFadeTime;
			float smooth = EaseOutCubic(f);
			m_intensity = NightLightIntensityMin + (StartLightIntensity - NightLightIntensityMin) * smooth;

			m_pos = Lerp(DefaultLightPositionTop, DefaultLightPositionLeft, smooth);
		}
	}
}
