#include "DirLight.h"
#include <imgui.h>
void DirLight::Update(float dt)
{
#if defined(_DEBUG) || defined(DEBUG)

	if (ImGui::Begin("Light Option"))
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


		float pos[3] = {
		m_pos.x,m_pos.y,m_pos.z
		};
		ImGui::InputFloat3("Position", pos);
		SetPosition(pos);

		float dir[3] = {
		m_dir.x,m_dir.y,m_dir.z };
		ImGui::InputFloat3("Direction", dir);
		SetDirection(dir);

		float intensity = m_intensity;
		ImGui::InputFloat("Intensity", &intensity);
		SetIntensity(intensity);
	}

	ImGui::End();
#endif

}
