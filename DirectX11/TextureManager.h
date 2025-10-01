#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "DebugLog.h"
#include "Texture.h"

class TextureManager
{
private:
	TextureManager() = default; // シングルトンパターンのため、コンストラクタはprivate
	~TextureManager() = default;

public:
	/// @brief Static Instance 
	static TextureManager& Instance()
	{
		static TextureManager instance; // ローカル静的変数でインスタンスを生成
		return instance;
	}

	void LoadTextures(const char* jsonFilePath);

	/// @brief 指定のテクスチャを登録する
	void Register(const char* name, const char* filePath);

	Texture* RegisterTexture(const char* name,const char* filePath)
	{
		Register(name,filePath);
		return GetTexture(std::string(name));
	}

	Texture* GetTexture(const std::string& name)
	{
		auto it = m_textures.find(name);
		if (it != m_textures.end())
		{
			return it->second.get();
		}
#ifdef _DEBUG
		//Error check
		DebugLog::LogError("[TextureManager] {} is nullptr", name.c_str());
#endif
		return nullptr;
	}

	void UnInit();
private:
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures; // テクスチャのマップ
};

