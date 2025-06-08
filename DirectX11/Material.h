#pragma once
#include <DirectXMath.h>
#include <unordered_map>
#include "Texture.h"

class Material
{
public:
	enum TextureType:uint8_t
	{
		Albedo,
		Normal,		
		Metallic_Smooth,
	};
protected:
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_diffuse;
	DirectX::XMFLOAT4 m_specular;
	DirectX::XMFLOAT4 m_emission;

	using TextureList = std::unordered_map<TextureType, Texture*>;
	TextureList m_textureList;//テクスチャ


public:

	Material();
	~Material() = default;
	/// <summary>
	/// 読み込んたテクスチャをマテリアルに設定する
	/// </summary>
	/// <param name="type">テクスチャ種類</param>
	/// <param name="tex"></param>
	void SetTexture(TextureType type, Texture* tex);

	/// <summary>
	/// テクスチャを返す
	/// </summary>
	/// <param name="type">テクスチャ種類</param>
	/// <returns></returns>
	Texture* GetTexture(TextureType type);

	DirectX::XMFLOAT4 GetAmbient()const { return m_ambient; };
	DirectX::XMFLOAT4 GetDiffuse()const { return m_diffuse; };
	DirectX::XMFLOAT4 GetSpecular()const { return m_specular; };
	DirectX::XMFLOAT4 GetEmission()const { return m_emission; };

	void SetAmbient(const DirectX::XMFLOAT4& ambient);
	void SetDiffuse(const DirectX::XMFLOAT4& diffuse);
	void SetSpecular(const DirectX::XMFLOAT4& specular);
	void SetEmission(const DirectX::XMFLOAT4& emission);

};



