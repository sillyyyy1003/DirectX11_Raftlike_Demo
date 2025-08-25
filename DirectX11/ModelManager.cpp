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
    auto it = m_modelList.find(modelName);
    if (it == m_modelList.end())
    {
        DebugLog::LogWarning("[ModelManager] {} doesn't exist!", modelName);
        return nullptr;
    }
    return GetModel(it->second);
}

Primitive* ModelManager::GetModel(ModelID id)
{
    auto it = m_models.find(id);
    if (it == m_models.end())
    {
        DebugLog::LogWarning("[ModelManager] Model ID {} doesn't exist!", id);
        return nullptr;
    }
    return it->second.get();
}

ModelManager::ModelID ModelManager::GetModelId(const std::string& name) const
{
    auto it = m_modelList.find(name);
    return (it != m_modelList.end()) ? it->second : InvalidId;
}

ModelManager::ModelID ModelManager::GetOrCreateModelId(const std::string& name)
{
    auto it = m_modelList.find(name);
    if (it != m_modelList.end())
    {
        return it->second;
    }

    ModelID newId = m_nextId++;
    m_modelList[name] = newId;
    m_models[newId] = nullptr;
    return newId;
}

void ModelManager::LoadModel(const char* modelName, const char* filePath)
{
    ModelID id = GetOrCreateModelId(modelName);
    auto& data = m_models[id];

    if (data) // already loaded
    {
        DebugLog::LogWarning("[ModelManager] {} already exists!", modelName);
        return;
    }

    auto model = std::make_shared<Model>();
    model->Init(filePath);
    data = model;

    DebugLog::Log("[ModelManager] Loaded model: {} (ID={})", modelName, id);
}

void ModelManager::LoadModel(const char* modelName, const std::shared_ptr<Primitive>& model)
{
    ModelID id = GetOrCreateModelId(modelName);
    auto& data = m_models[id];

    if (data)
    {
        DebugLog::LogWarning("[ModelManager] {} already exists!", modelName);
        return;
    }

    data = model;
    DebugLog::Log("[ModelManager] Loaded model: {} (ID={})", modelName, id);
}

bool ModelManager::LoadModels(const char* jsonFilePath)
{
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

    DebugLog::Log("[ModelManager] Loading models from JSON: {}", jsonFilePath);

    for (const auto& modelEntry : j["models"])
    {
        if (!modelEntry.contains("name") || !modelEntry.contains("file"))
        {
            DebugLog::LogWarning("[ModelManager] Skipping model entry with missing fields.");
            continue;
        }

        std::string name = modelEntry["name"];
        std::string filePath = modelEntry["file"];
        LoadModel(name.c_str(), filePath.c_str());
    }

    DebugLog::Log("[ModelManager] Finished loading models.");
    return true;
}

void ModelManager::UnInit()
{
    for (auto& [id, data] : m_models)
    {
        data.reset();
    }
    m_models.clear();
    m_modelList.clear();
    m_nextId = 0;
}

ModelManager::~ModelManager()
{
}
