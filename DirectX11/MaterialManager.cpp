#include "MaterialManager.h"
#include <nlohmann/json.hpp>

#include "d3dUtil.h"
#include "DebugLog.h"
#include "TextureManager.h"

Material* MaterialManager::GetMaterial(const char* materialName)
{
	auto it = m_materialList.find(materialName);
	if (it == m_materialList.end())
	{
		return nullptr; // マテリアルが見つからない場合はnullptrを返す
	}
	return GetMaterial(it->second);
}

Material* MaterialManager::GetMaterial(MaterialID id)
{
	auto it = m_materials.find(id);
	if (it == m_materials.end())
	{
		return nullptr; // マテリアルが見つからない場合はnullptrを返す
	}
	return it->second.get();
}

MaterialManager::MaterialID MaterialManager::GetMaterialId(const std::string& name) const
{
	auto it = m_materialList.find(name);
	return (it != m_materialList.end()) ? it->second : InvalidId;
}

MaterialManager::MaterialID MaterialManager::GetOrCreateMaterialId(const std::string& name)
{
	auto it = m_materialList.find(name);
	if (it != m_materialList.end())
	{
		return it->second; // 既に存在する場合はそのIDを返す
	}
	MaterialID newId = m_nextId++;
	m_materialList[name] = newId;
	m_materials[newId] = nullptr; // 新しいIDのマテリアルはまだロードされていない
	return newId;
}

void MaterialManager::RegisterMaterial(const char* materialName, const std::shared_ptr<Material>& material)
{
	auto id = GetOrCreateMaterialId(materialName);
	auto& data = m_materials[id];
	if (data)
	{
		DebugLog::LogWarning("[MaterialManager] {} already exists!", materialName);
		return;
	}
	data = material; // マテリアルを登録
	DebugLog::Log("[MaterialManager] Loaded material: {} (ID={})", materialName, id);
}

void MaterialManager::RegisterMaterials(const char* jsonFilePath)
{
	std::ifstream ifs(jsonFilePath);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[MaterialManager] Failed to open JSON file: {}", jsonFilePath);
		return;
	}

	nlohmann::json j;
	try
	{
		ifs >> j;
	}
	catch (const std::exception& e)
	{
		DebugLog::LogError("[MaterialManager] Failed to parse JSON: {}", e.what());
		return;
	}

	DebugLog::Log("[MaterialManager] Loading models from JSON: {}", jsonFilePath);

	for (const auto& materialEntry : j["materials"])
	{
		if (!materialEntry.contains("name"))
		{
			DebugLog::LogWarning("[ModelManager] Skipping model entry with missing fields.");
			continue;
		}

		std::string name = materialEntry["name"];
		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetAmbient(JsonToXMFLOAT4(materialEntry["ambient"]));
		material->SetDiffuse(JsonToXMFLOAT4(materialEntry["diffuse"]));
		material->SetSpecular(JsonToXMFLOAT4(materialEntry["specular"]));
		material->SetEmission(JsonToXMFLOAT4(materialEntry["emission"]));

		// Set albedo tex
		std::string albedo = materialEntry["albedoTex"];
		if (!albedo.empty())
		{
			Texture* albedoTex = TextureManager::Instance().GetTexture(albedo);
			if (albedoTex)
			{
				material->SetTexture(Material::Albedo, albedoTex);
			}
			else
			{
				DebugLog::LogWarning("[MaterialManager] Albedo texture {} not found for material {}", albedo, name);
			}
		}

		// Set normal tex
		std::string normal = materialEntry["normalTex"];
		if (!normal.empty())
		{
			Texture* normalTex = TextureManager::Instance().GetTexture(normal);
			if (normalTex)
			{
				material->SetTexture(Material::Normal, normalTex);
			}
			else
			{
				DebugLog::LogWarning("[MaterialManager] Normal texture {} not found for material {}", normal, name);
			}
		}

		// Set metallic texture
		std::string metallic = materialEntry["metallicTex"];
		if (!metallic.empty())
		{
			Texture* metallicTex = TextureManager::Instance().GetTexture(metallic);
			if (metallicTex)
			{
				material->SetTexture(Material::Metallic_Smooth, metallicTex);
			}
			else
			{
				DebugLog::LogWarning("[MaterialManager] Metallic texture {} not found for material {}", metallic, name);
			}
		}
		DebugLog::Log("[MaterialManager] Load Material {}", name);
		MaterialID id = GetOrCreateMaterialId(name);	// マテリアルIDを取得または作成
		m_materials[id] = material; // マテリアルを登録
	}

	DebugLog::Log("[MaterialManager] Materials loaded from JSON: {}", jsonFilePath);
}

void MaterialManager::UnInit()
{
	for(auto& material : m_materials)
	{
		material.second.reset(); // マテリアルをリセット
	}
	m_materials.clear(); // マテリアルのマップをクリア
	m_materialList.clear(); // マテリアルIDのマップをクリア

}
