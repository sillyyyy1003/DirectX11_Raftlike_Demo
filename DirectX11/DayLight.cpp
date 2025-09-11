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
    static constexpr float DayLength = 900.f;
    static constexpr float DayTime =  600.f;
    static constexpr float NightTime = 300.f;
    static constexpr float DayLightFadeTime = 60.f;
	static constexpr float UnitHour = 37.5f; // 1時間の秒数

}

DayLight::DayLight():
	m_time(112.5f)//assume sunrise at 6:00 AM(0) make startTime = 9AM so m_time init=37.5*3;
{
}

void DayLight::Update(float dt)
{
	m_time += dt;

	float timeOfDay = fmodf(m_time, DayLength);


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
