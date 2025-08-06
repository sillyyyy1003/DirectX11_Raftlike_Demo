#include "DayLight.h"
#include "d3dUtil.h"

namespace
{
	static constexpr DirectX::XMFLOAT3 DefaultLightPositionLeft = { -10.f, 0, 0 }; // Default Light Position
    static constexpr DirectX::XMFLOAT3 DefaultLightPositionTop = { 0, 10, 0 }; // Default Light Position
    static constexpr DirectX::XMFLOAT3 DefaultLightPositionRight = { 10.f, 0, 0 }; // Default Light Position

    static constexpr float DayLightIntensityMax = 2.5f;
	static constexpr float NightLightIntensityMax = 1.f;

}

void DayLight::Update(float dt)
{
	m_tick += dt;
	const float dayLength = 60.0f;
	const float daytime = 40.0f;

	float timeOfDay = fmodf(m_tick, dayLength);

    if (timeOfDay < daytime)
    {
		//Daytime
        float t = timeOfDay / daytime;
        float factor = (t <= 0.5f) ? (t * 2.0f) : (2.0f - t * 2.0f); // 0→1→0
        float smooth = EaseInOut(factor);

        if (t <= 0.5f)
        {
            m_pos = Lerp(DefaultLightPositionLeft, DefaultLightPositionTop, smooth);
        }
        else
        {
            m_pos = Lerp(DefaultLightPositionTop, DefaultLightPositionRight, 1.0f - smooth);
        }

        m_intensity = NightLightIntensityMax + (DayLightIntensityMax - NightLightIntensityMax) * factor;
    
    }
    else
    {
		//Nighttime
        float nightTime = timeOfDay - daytime;
        float nightDuration = dayLength - daytime;
        float t = nightTime / nightDuration;
        float factor = (t <= 0.5f) ? (t * 2.0f) : (2.0f - t * 2.0f); // 0→1→0
        float smooth = EaseInOut(factor);

        if (t <= 0.5f)
        {
            m_pos = Lerp(DefaultLightPositionRight, DefaultLightPositionTop, smooth);
        }
        else
        {
            m_pos = Lerp(DefaultLightPositionTop, DefaultLightPositionLeft, 1.0f - smooth);
        }
        //m_intensity = 2.f + (2.5f - 2.f) * factor;
        //m_diffuse = { 0.3f,0.2f,0.6f,1.f };
    }


}
