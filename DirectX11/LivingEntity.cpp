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
	if (m_isDead) return;
	m_isDead = true;
	NotifyDeathListeners(m_isDead);
}

void PlayerEntity::Revive()
{
	if (!m_isDead)return;
	m_isDead = false;
	NotifyDeathListeners(m_isDead);	//Notify every thing to resume
}

void PlayerEntity::Update(float dt)
{
	if(m_isTickDamaged)
	{
		OnDamage(dt * TickDamage);
	}

	if (m_life <= 0.0f && !m_isDead)
	{
		Dead();
	}
}

void PlayerEntity::AddDeathListener(Callback cb)
{
	m_deathListeners.push_back(cb);
}

void PlayerEntity::NotifyDeathListeners(bool state)
{
	for (auto& cb : m_deathListeners) cb(state);
}
