#include "LivingEntity.h"

LivingEntity::LivingEntity(float _startLife):
m_startLife(_startLife),
m_life(_startLife)
{
}

void LivingEntity::OnDamage(float _damage)
{
	m_life -= _damage;

	// check dead or not
	if (m_life <= 0)
	{
		m_life = 0;
		Dead();
	}
}

void LivingEntity::Dead()
{
	m_isDead = true;
}

//====================== Player Entity 
PlayerEntity::PlayerEntity(float _startLife) :
	LivingEntity(_startLife),
	m_uiElement(nullptr)
{
}

void PlayerEntity::OnDamage(float _damage)
{
	
}

void PlayerEntity::Dead()
{

}
