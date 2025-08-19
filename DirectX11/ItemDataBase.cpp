#include "ItemDataBase.h"

#include <memory>
#include "DebugLog.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "PhysicsManager.h"
#include "RenderComponent.h"
#include <nlohmann/json.hpp>

namespace
{
	static constexpr float DefaultObjectScale = 0.05f;
}

ItemDataBase& ItemDataBase::Instance()
{
	static ItemDataBase instance;
	return instance;
}

void ItemDataBase::RegisterItem(const char* name, std::shared_ptr<Item> item)
{
	//Check if item already exits
	auto it = m_items.find(std::string(name));
	if (it != m_items.end())
	{
		DebugLog::LogWarning("[ItemDataBase] {} already exits", name);
		return;
	}

	m_items[std::string(name)] = item;				// Mapに登録
	m_items[std::string(name)]->SetName(name);		// 名前を設定する
	DebugLog::Log("[ItemDataBase] {} is registered!", name);
}

std::shared_ptr<const Item> ItemDataBase::GetItem(const char* name)
{
	auto it = m_items.find(std::string(name));
	if (it != m_items.end())
		return it->second;

	DebugLog::LogWarning("[ItemDataBase] {} is not exit", name);
	return nullptr;
}

void ItemDataBase::RegisterItem(const char* name, std::shared_ptr<Item> item, uint32_t itemId, uint32_t modelId,
	uint32_t materialId)
{
	item->SetName(name);
	item->SetItemId(itemId);
	item->SetModelId(modelId);
	item->SetMaterialId(materialId);
	RegisterItem(name, item);
}

void ItemDataBase::UnInit()
{
	m_items.clear();			// Clear the map
}

std::shared_ptr<ItemInstance> ItemDataBase::CreateItemInstance(const char* itemName, int count, float durability)
{
	std::shared_ptr<ItemInstance> itemInstance = std::make_shared<ItemInstance>();
	auto it = m_items.find(std::string(itemName));
	if (it == m_items.end())
	{
		DebugLog::LogError("[ItemDataBase] {} is not found in database!", itemName);
		return nullptr;
	}
	uint32_t id = it->second->GetItemId();
	uint32_t modelId = it->second->GetModelId();
	uint32_t materialId = it->second->GetMaterialId();
	itemInstance->InitItem(GetItem(itemName), count,durability);
	// Add Render component
	std::shared_ptr<RenderComponent> renderComponent = std::make_shared<RenderComponent>();
	renderComponent->Init(MaterialManager::Instance().GetMaterial(materialId), nullptr, ModelManager::Instance().GetModel(modelId));
	itemInstance->AddComponent(MyComponent::ComponentType::Render, renderComponent);

	// Add Physics component
	DirectX::XMFLOAT3 modelSize = ModelManager::Instance().GetModel(modelId)->GetModelSize();
	BodyCreationSettings boxSettings(new BoxShape(RVec3(modelSize.x * 0.5f * DefaultObjectScale, modelSize.y * 0.5f * DefaultObjectScale, modelSize.z * 0.5f * DefaultObjectScale)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM);
	PhysicsManager::Instance().SetBodyCreationMass(1.f, boxSettings);	// Set the mass properties for the apple box
	std::shared_ptr<PhysicsComponent> physicsComponent = make_shared<PhysicsComponent>();
	physicsComponent->Init(boxSettings, EActivation::Activate);
	itemInstance->AddComponent(MyComponent::ComponentType::Physics, physicsComponent);

	// Set Default scale
	itemInstance->GetTransform().SetScale(DefaultObjectScale, DefaultObjectScale, DefaultObjectScale);

	return itemInstance;
}

void ItemDataBase::LoadItemDataFromJsonFile(const char* jsonFilePath)
{
	std::ifstream ifs(jsonFilePath);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[ItemDataBase] Failed to open JSON file: {}", jsonFilePath);
		return;
	}

	nlohmann::json j;
	try
	{
		ifs >> j;
	}
	catch (const std::exception& e)
	{
		DebugLog::LogError("[ItemDataBase] Failed to parse JSON: {}", e.what());
		return;
	}

	// Load food item data
	if (j.contains("foods"))
	{
		for(const auto& food:j["foods"])
		{
			std::string name = food["name"];
			float nutrition = food["nutrition"];
			std::string modelName = food["model"];
			std::string materialName = food["material"];

			auto foodPtr = std::make_shared<Food>(nutrition);
			m_nextID++;	//todo:make id format :1001 (itemCode+xxx);
			RegisterItem(
				name.c_str(),
				foodPtr, 
				m_nextID,
				ModelManager::Instance().GetModelId(modelName),
				MaterialManager::Instance().GetMaterialId(materialName)
			);
		}
	}

	//todo: ほかのアイテムを追加
}

ItemDataBase::~ItemDataBase()
{

}
