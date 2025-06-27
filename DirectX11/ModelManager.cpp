#include "ModelManager.h"

#include "DebugLog.h"

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
