#include "LightBase.h"
LightBase::LightBase():
m_pos(0, 0, 0),
m_dir(1,-1,0),
m_ambient(1,1,1,1),
m_diffuse(1,1,1,1),
isEnable(true)
{
}

void LightBase::SetPosition(const DirectX::XMFLOAT3& _pos)
{
	m_pos = _pos;
}

void LightBase::SetPosition(float* _pos)
{
	m_pos = DirectX::XMFLOAT3(_pos[0], _pos[1], _pos[2]);
}

void LightBase::SetDirection(const DirectX::XMFLOAT3& _dir)
{
	m_dir = _dir;
}

void LightBase::SetDirection(float* _dir)
{
	m_dir = DirectX::XMFLOAT3(_dir[0], _dir[1], _dir[2]);
}

void LightBase::SetAmbient(const DirectX::XMFLOAT4& _ambient)
{
	m_ambient = _ambient;
}

void LightBase::SetAmbient(float* ambient)
{
	m_ambient = DirectX::XMFLOAT4(ambient[0], ambient[1], ambient[2], ambient[3]);
}

void LightBase::SetDiffuse(const DirectX::XMFLOAT4& _diffuse)
{
	m_diffuse = _diffuse;
}

void LightBase::SetDiffuse(float* diffuse)
{
	m_diffuse = DirectX::XMFLOAT4(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
}
