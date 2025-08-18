#include "TextureManager.h"


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
