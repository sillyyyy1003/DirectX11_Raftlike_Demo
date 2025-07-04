﻿#include "Material.h"

Material::Material():
	m_ambient(1.f,1.f,1.f,1.f),
	m_diffuse(1,1,1,1),
	m_specular(0,0,0,1),
	m_emission(0,0,0,0)
{
}

void Material::SetTexture(TextureType type, Texture* tex)
{
	m_textureList[type] = tex;
}

Texture* Material::GetTexture(TextureType type)
{
	auto it = m_textureList.find(type);
	if (it == m_textureList.end())return nullptr;
	return it->second;
}

bool Material::GetTextureEnable()
{
	if (m_textureList.size() == 0)return false;
	else return true;
}

void Material::SetAmbient(const DirectX::XMFLOAT4& ambient) 
{
	m_ambient = ambient;
}

void Material::SetDiffuse(const DirectX::XMFLOAT4& diffuse)
{
	m_diffuse = diffuse;
}

void Material::SetSpecular(const DirectX::XMFLOAT4& specular)
{
	m_specular = specular;
}

void Material::SetEmission(const DirectX::XMFLOAT4& emission)
{
	m_emission = emission;
}

void Material::SetDefaultMaterial()
{
	SetAmbient({1,1,1,1});
	SetDiffuse({ 1,1,1,1 });
	SetSpecular({ 0,0,0,0 });
	SetEmission({ 0,0,0,0 });
}
