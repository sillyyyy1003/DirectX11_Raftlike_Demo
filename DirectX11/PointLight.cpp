#include "PointLight.h"
#include "Assets/Lib/imgui/Include/imgui.h"

PointLight::PointLight():m_range(10.0f),m_attenuation(1,0,0)
{
}

void PointLight::Update(float dt)
{
#ifdef _DEBUG
	if (ImGui::Begin("Point Light Option"))
	{
		ImGui::Checkbox("isEnable", &isEnable);

		float ambient[4] = {
			GetAmbient().x,GetAmbient().y,GetAmbient().z,GetAmbient().w
		};
		ImGui::ColorEdit4("Ambient", ambient);
		SetAmbient(ambient);

		float diffuse[4] = {
		GetDiffuse().x,GetDiffuse().y,GetDiffuse().z,GetDiffuse().w
		};
		ImGui::ColorEdit4("Diffuse", diffuse);
		SetDiffuse(diffuse);

		//位置
		float pos[3] = { m_pos.x,m_pos.y,m_pos.z };
		ImGui::InputFloat3("Position", pos);
		SetPosition(pos);

		//減衰
		float attenuation[3] = { m_attenuation.x,m_attenuation.y,m_attenuation.z };
		ImGui::InputFloat3("Attenuation", attenuation);
		SetAttenuation(attenuation);

		//範囲設定
		ImGui::SliderFloat("Range", &m_range, 0, 40.f);
	}

	ImGui::End();
#endif
}

void PointLight::SetRange(float _range)
{
	m_range = _range;
}

void PointLight::SetAttenuation(const DirectX::XMFLOAT3& _attenuation)
{
	m_attenuation = _attenuation;
}

void PointLight::SetAttenuation(float x, float y, float z)
{
	m_attenuation = DirectX::XMFLOAT3(x, y, z);
}

void PointLight::SetAttenuation(const float* attenuation)
{
	m_attenuation = DirectX::XMFLOAT3(attenuation[0], attenuation[1], attenuation[2]);
}

Light::PointLight PointLight::GetPointLight()
{
	Light::PointLight light;
	light = {
		this->GetAmbient(),
		this->GetDiffuse(),
		this->GetPosition(),
		this->GetRange(),
		this->GetAttenuation(),
		(float)GetEnable(),
	};
	return light;
}
