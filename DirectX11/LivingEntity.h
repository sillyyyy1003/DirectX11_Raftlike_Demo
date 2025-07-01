#pragma once
#include "Component.hpp"
#include "UIElement.h"

/// @brief 
class LivingEntity:
	public Component
{
private:
	float m_startLife;
	float m_life;
	bool m_isDead = false;
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

private:

	UIElement* m_uiElement;		// HP表示用

};