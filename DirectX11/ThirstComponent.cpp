#include "ThirstComponent.h"

ThirstComponent::ThirstComponent(float initialValue):
	m_initialThirstValue(initialValue),
	m_currentThirstValue(initialValue),
	m_thirstSpeed(0.f),
	m_pUiComponent(nullptr)
{
}

void ThirstComponent::Update(float dt)
{
	if (m_isThirsty)return;

	// Update Thirst Value
	m_currentThirstValue -= m_thirstSpeed * dt;

	// Check if the thirst value is below 0
	if(m_currentThirstValue<=0)
	{
		m_currentThirstValue = 0;
		m_isThirsty = true;
	}

	// check if ui is set
	assert(m_pUiComponent != nullptr);
	// Ui更新(percentage)
	m_pUiComponent->UpdateUI(m_currentThirstValue / m_initialThirstValue);
}

void ThirstComponent::SetThirstSpeed(float speed)
{
	m_thirstSpeed = speed;
}

void ThirstComponent::RestoreThirst(float thirstValue)
{
	// recover hunger
	m_currentThirstValue += thirstValue;

	//Check if over the limit
	if (m_currentThirstValue >= m_initialThirstValue)
		m_currentThirstValue = m_initialThirstValue;

	// Set status to not starve
	m_isThirsty = false;
}

void ThirstComponent::SetUIComponent(UIBar* thirstBar)
{
	m_pUiComponent = thirstBar;
}

void ThirstComponent::Draw()
{
	assert(m_pUiComponent != nullptr); //check if UI component is set
	m_pUiComponent->Draw();
}
