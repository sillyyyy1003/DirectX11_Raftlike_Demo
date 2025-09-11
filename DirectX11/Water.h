#pragma once
#include <nlohmann/json.hpp>
#include "GameObject.h"

class Water :
    public GameObject
{
public:
	Water();
	~Water() override = default;

	/// @brief Init Water parameters
	/// @param deepColor furthest color
	/// @param shallowColor nearest color
	/// @param maxDistance distance
	/// @param waveSpeed wave transfer speed
	/// @param waveAmplitude wave height
	/// @param waveLength wave length;
	void Init(const DirectX::XMFLOAT4& deepColor, const DirectX::XMFLOAT4& shallowColor, float maxDistance,
	          float waveSpeed, float waveAmplitude, float waveLength);

	void LoadFromConfig(nlohmann::json& j, const char* waterName);

	void Update(float dt) override;
	void Draw() override;

private:
	float m_time = 0;

	DirectX::XMFLOAT4 m_deepColor;
	DirectX::XMFLOAT4 m_shallowColor;
	float m_maxDistance;
	float m_waveSpeed;
	float m_waveAmplitude;
	float m_waveLength;

};

