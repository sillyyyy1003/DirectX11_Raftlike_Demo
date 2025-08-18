#include "HungerComponent.h"


HungerComponent::HungerComponent(float initialValue) :
	m_initialHungerValue(initialValue),
	m_currentHungerValue(initialValue),
	m_starveSpeed(0.f),
	m_starveThreshold(90)
{
}

HungerComponent::~HungerComponent()
{
	m_hungryListeners.clear();
	m_starveListeners.clear();
}


void HungerComponent::Init(float starveSpeed, float starveThreshold)
{
	SetStarveSpeed(starveSpeed);
	SetStarveThreshold(starveThreshold);

}

void HungerComponent::UnInit()
{

}

void HungerComponent::Update(float dt)
{
	if (m_isStarve)return;

	// Update hunger value
	m_currentHungerValue -= m_starveSpeed * dt;

	// 空腹度限界にこえたら
	bool hungerState = (m_currentHungerValue <= m_starveThreshold);
	if (hungerState != m_isHungry)
	{
		m_isHungry = hungerState;
		// Notify listeners about the change in hunger state
		NotifyHungryListeners(m_isHungry);
	}

	bool starveState = (m_currentHungerValue <= 0.f);
	if (starveState != m_isStarve)
	{
		m_isStarve = starveState;
		// Notify listeners about the change in starve state
		NotifyStarveListener(m_isStarve);
	}


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


float HungerComponent::GetCurrentHungerPercentage() const
{
	return m_currentHungerValue / m_initialHungerValue;
}
