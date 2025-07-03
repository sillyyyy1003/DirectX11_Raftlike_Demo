#include "HungerComponent.h"


HungerComponent::HungerComponent(float initialValue) :
	m_initialHungerValue(initialValue),
	m_currentHungerValue(initialValue),
	m_starveSpeed(1.f),
	m_pUiComponent(nullptr)
{
}

void HungerComponent::Update(float dt)
{
	if (m_isStarve)return;

	// Update hunger value
	m_currentHungerValue -= m_starveSpeed * dt;

	// 空腹度ゼロ以下の場合
	if (m_currentHungerValue <= 0)m_isStarve = true;

	//Uiある場合、更新する
	assert(m_pUiComponent != nullptr);
	m_pUiComponent->UpdateUI(m_currentHungerValue / m_initialHungerValue);

}

void HungerComponent::SetStarveSpeed(float speed)
{
	m_starveSpeed = speed;
}

void HungerComponent::RestoreHunger(float foodValue)
{
	// recover hunger
	m_currentHungerValue += foodValue;

	//Check if over the limit
	if (m_currentHungerValue >= m_initialHungerValue)
		m_currentHungerValue = m_initialHungerValue;

	// Set status to not starve
	m_isStarve = false;
}

void HungerComponent::SetUIComponent(UIBar* pUi)
{
	m_pUiComponent = pUi;
}

void HungerComponent::Draw()
{
	assert(m_pUiComponent != nullptr);
	m_pUiComponent->Draw();
}
