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

Primitive* ModelManager::GetModel(const char* modelName)
{
	auto it = m_models.find(std::string(modelName));
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
		DebugLog::LogWarning("[ModelManager] {} already exists!", modelName);
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
	DebugLog::Log("[ModelManager] Loaded model: {}", modelName);
}

bool ModelManager::LoadModels(const char* jsonFilePath)
{
	// json file check
	std::ifstream ifs(jsonFilePath);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[ModelManager] Failed to open JSON file: {}", jsonFilePath);
		return false;
	}

	json j;
	try
	{
		ifs >> j; 
	}
	catch (const std::exception& e)
	{
		DebugLog::LogError("[ModelManager] Failed to parse JSON: {}", e.what());
		return false;
	}

	DebugLog::Log("[ModelManager] Loading models from JSON file: {}", jsonFilePath);

    for (const auto& modelEntry : j["models"])
    {
		// Check if the model entry contains the required fields
        if (!modelEntry.contains("name") || !modelEntry.contains("file"))
        {
			DebugLog::LogWarning("[ModelManager] Skipping model entry with missing 'name' or 'file'.");
            continue;
        }


        std::string name = modelEntry["name"];
        std::string filePath = modelEntry["file"];
        std::shared_ptr<Model> model = std::make_shared<Model>();
		model->Init(filePath.c_str());
		// Check if the model was loaded successfully
        if (!model)
        {
			DebugLog::LogError("[ModelManager] Failed to load model from {}",filePath);
        	continue;
        }

		// emplace data into the map
        ModelData data;
        data.model = model;
        data.name = name;
        data.filePath = filePath;
		m_models.emplace(name, model);
		DebugLog::Log("[ModelManager] Loaded model: {}", name);
    }
	DebugLog::Log("[ModelManager] Finish Loading model!");
    return true;
}

void ModelManager::LoadModel(const char* modelName, const std::shared_ptr<Primitive>& model)
{
	// Check if the model already exists in the map
	auto it = m_models.find(modelName);
	if (it != m_models.end())
	{
		DebugLog::LogWarning("[ModelManager] {} already exists!", modelName);
		return;
	}

	//move ptr and reset former ptr
	std::shared_ptr<Primitive> modelPtr = std::move(model);

	// Add the model to the map
	m_models[modelName] = {
		modelPtr,
		std::string(modelName),
		std::string("") // No file path provided
	};
	DebugLog::Log("[ModelManager] Loaded model: {}", modelName);
}

void ModelManager::UnInit()
{
	for(auto& modelPair : m_models)
	{
		modelPair.second.model.reset();
	}
	m_models.clear();

}
