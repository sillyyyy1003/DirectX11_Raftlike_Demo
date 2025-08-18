#include "LivingEntity.h"

namespace
{
	static constexpr float TickDamage = 1.f;
}

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
	LivingEntity(_startLife)
{
}

void PlayerEntity::OnDamage(float _damage)
{
	LivingEntity::OnDamage(_damage);
}

void PlayerEntity::Dead()
{

}

void PlayerEntity::Update(float dt)
{
	if(m_isTickDamaged)
	{
		OnDamage(dt * TickDamage);
	}
}
