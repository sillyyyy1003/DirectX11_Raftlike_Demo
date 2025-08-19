#pragma once
#include "BuoyancySystem.h"

class MaterialManager
{
public:
	typedef uint32_t MaterialID; ///< マテリアルのID
	static constexpr MaterialID InvalidId = -1;
	static MaterialManager& Instance()
	{
		static MaterialManager instance;
		return instance;
	}

	Material* GetMaterial(const char* materialName);
	Material* GetMaterial(MaterialID id);

	MaterialID GetMaterialId(const std::string& name) const;
	MaterialID GetOrCreateMaterialId(const std::string& name);

	void RegisterMaterial(const char* materialName, const std::shared_ptr<Material>& material);
	void RegisterMaterials(const char* jsonFilePath);

private:
	MaterialManager() = default;
	~MaterialManager() = default;
	// Disable copy constructor and assignment operator
	MaterialManager(const MaterialManager&) = delete;
	MaterialManager& operator=(const MaterialManager&) = delete;


	typedef std::unordered_map<std::string, MaterialID> MaterialMap; ///< マテリアルのIDと名前のマップ
	typedef std::unordered_map<MaterialID, std::shared_ptr<Material>> Materials;

	MaterialMap m_materialList;	///< マテリアルの名前とIDのマップ
	Materials m_materials;		///< マテリアルのIDとマテリアルデータのマップ

	MaterialID m_nextId = 0; ///< 次に割り当てるマテリアルID
};

