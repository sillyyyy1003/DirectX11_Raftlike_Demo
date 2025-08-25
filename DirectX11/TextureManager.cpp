#include "TextureManager.h"

#include <nlohmann/json.hpp>


void TextureManager::LoadTextures(const char* jsonFilePath)
{
    std::ifstream ifs(jsonFilePath);
    if (!ifs.is_open())
    {
        DebugLog::LogError("[ModelManager] Failed to open JSON file: {}", jsonFilePath);
        return;
    }

    nlohmann::json j;
    try
    {
        ifs >> j;
    }
    catch (const std::exception& e)
    {
        DebugLog::LogError("[ModelManager] Failed to parse JSON: {}", e.what());
        return;
    }

	DebugLog::Log("[TextureManager] Loading textures from JSON: {}", jsonFilePath);

    for (const auto& textureEntry : j["textures"])
    {
	    if(!textureEntry.contains("name") || !textureEntry.contains("file"))
	    {
			DebugLog::LogWarning("[TextureManager] Skipping texture entry with missing fields.");
            continue;
	    }

		std::string name = textureEntry["name"];
		std::string filePath = textureEntry["file"];
        RegisterTexture(name.c_str(), filePath.c_str());
    }

}

void TextureManager::Register(const char* name, const char* filePath)
{
	std::shared_ptr<Texture> tex = std::make_shared<Texture>();
	if (FAILED(tex->Create(filePath)))
	{
		DebugLog::LogError("[TextureManager] Failed to create texture from file: {}", filePath);
		return;
	}
	std::string texName = std::string(name);
	m_textures[texName] = tex; // 指定されたIDでテクスチャを登録

#ifdef _DEBUG
	DebugLog::Log("[TextureManager] Registered texture: {}", name);
#endif

}

void TextureManager::UnInit()
{
    for(auto& texture : m_textures)
    {
        if (texture.second)
        {
            texture.second.reset();
        }
	}
	m_textures.clear(); // Clear the map
}
