#include "ThirstComponent.h"

ThirstComponent::ThirstComponent(float initialValue):
	m_initialThirstValue(initialValue),
	m_currentThirstValue(initialValue),
	m_thirstSpeed(0.f)
{
}

void ThirstComponent::Init(float thirstSpeed, float thirstThreshold)
{
	m_thirstSpeed = thirstSpeed;
	m_thirstThreshold = thirstThreshold;
}

void ThirstComponent::Update(float dt)
{
	if (m_isThirsty)return;

	// Update Thirst Value
	m_currentThirstValue -= m_thirstSpeed * dt;

	// check iif over the limit
	bool hungerState = (m_currentThirstValue <= m_thirstThreshold);
	if (hungerState != m_isThirsty)
	{
		m_isThirsty = hungerState;
		// Notify listeners about the change in thirsty state
		NotifyThirstyListeners(m_isThirsty);
	}

	bool starveState = (m_currentThirstValue <= 0.f);
	if (starveState != m_isThirstyToDeath)
	{
		m_isThirstyToDeath = starveState;
		// Notify listeners about the change in dying state
		NotifyThirstyToDeathListener(m_isThirstyToDeath);
	}

}

void ThirstComponent::SetThirstSpeed(float speed)
{
	m_thirstSpeed = speed;
}

void ThirstComponent::RestoreThirst(float thirstValue)
{
	// recover thirst value
	m_currentThirstValue += thirstValue;

	//Check if over the limit
	if (m_currentThirstValue >= m_initialThirstValue)
		m_currentThirstValue = m_initialThirstValue;

	// Set status to not starve
	m_isThirsty = false;
}

float ThirstComponent::GetCurrentThirstPercentage() const
{
	return m_currentThirstValue / m_initialThirstValue;
}

