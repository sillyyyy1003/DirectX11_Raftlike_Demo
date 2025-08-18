#pragma once
#include "Component.hpp"
#include "UIElement.h"

/// @brief 
class LivingEntity:
	public Component
{
protected:
	float m_startLife;
	float m_life;
	bool  m_isDead = false;
public:

	LivingEntity(float _startLife);
	~LivingEntity() override = default;

	virtual void OnDamage(float _damage);
	virtual void Dead();

};

class PlayerEntity: public LivingEntity
{

public:
	PlayerEntity(float _startLife = 100.f);
	~PlayerEntity() override = default;

	void OnDamage(float _damage) override;
	void Dead() override;

	float GetCurrentHealthPercentage() const { return m_life / m_startLife; }

	void Update(float dt);

	/// @brief Set PlayerEntity is damaged by tick
	/// @param isDamaged 
	void SetTickDamaged(bool isDamaged) { m_isTickDamaged = isDamaged; }

	/// @brief Callback method for starving state change
	/// @param isStarve player's current hunger value is below 0
	void OnStateStarveChanged(bool isStarve)
	{
		if (isStarve)SetTickDamaged(true);
		else SetTickDamaged(false);
	}

protected:
	bool m_isTickDamaged = false;
};