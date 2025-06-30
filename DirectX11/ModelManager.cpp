#include "ModelManager.h"
#include <iostream>
#include "DebugLog.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ModelManager& ModelManager::Instance()
{
	static ModelManager instance; // Guaranteed to be destroyed and instantiated on first use.
	return instance;
}

Model* ModelManager::GetModel(const char* modelName)
{
	auto it = m_models.find(modelName);
	if (it != m_models.end())
	{
		return it->second.model.get();
	}

	DebugLog::LogWarning("[ModelManager]{} doesn't exist!", modelName);
	return nullptr;
}

void ModelManager::LoadModel(const char* modelName, const char* filePath)
{
	// Check if the model already exists in the map
	auto it = m_models.find(modelName);
	if (it != m_models.end())
	{
		DebugLog::LogWarning("[ModelManager]{} already exists!", modelName);
		return;
	}

	// Create a new Model instance and add it to the map
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->Init(filePath);
	m_models[modelName] = {
		model,
		std::string(modelName),
		std::string(filePath)
	};
	DebugLog::Log("[ModelManager]Loaded model: {}", modelName);
}

bool ModelManager::LoadModels(const char* jsonFilePath)
{
	std::ifstream ifs(jsonFilePath);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[ModelManager]Failed to open JSON file: {}", jsonFilePath);
		return false;
	}
		
	json j;
 
    for (const auto& modelEntry : j["models"])
    {
        if (!modelEntry.contains("name") || !modelEntry.contains("file"))
        {
            std::cerr << "[Warning] Skipping model entry with missing 'name' or 'file'.\n";
            continue;
        }

        std::string name = modelEntry["name"];
        std::string filePath = modelEntry["file"];

        std::shared_ptr<Model> model = std::make_shared<Model>();
		model->Init(filePath.c_str());
        if (!model)
        {
            std::cerr << "[Warning] Failed to load model from: " << filePath << std::endl;
			DebugLog::LogError("");
        	continue;
        }

        ModelData data;
        data.model = model;
        data.name = name;
        data.filePath = filePath;

        m_models[name] = std::move(data);
        std::cout << "[Info] Loaded model '" << name << "' from '" << filePath << "'\n";
    }

    return true;
}
