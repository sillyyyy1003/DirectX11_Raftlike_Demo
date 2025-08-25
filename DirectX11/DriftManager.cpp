#include "DriftManager.h"
#include <memory>
#include "ItemDataBase.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "PhysicsComponent.h"
#include "Player.h"
#include "RandomManager.h"
#include "RenderComponent.h"

namespace
{
    static constexpr int NearItemNum = 20;
    static constexpr int FarItemNum = 500;

    // ==== 距離判定用パラメーター ====
    static constexpr float NearToFarDistance = 20.f; // Near → Far 回収
    static constexpr float FarToNearDistance = 20.f; // Far → Near 回収
    static constexpr float MaxDistance = 100.f; // FarItem 最大存活距离

    static constexpr float SpawnInterval = 2.f;    // FarItem 自动生成间隔
    static constexpr DirectX::XMFLOAT3 FloatVector = { 0,0,-1 };

    static constexpr uint InitialSpawnNum = 20;
	static constexpr DirectX::XMFLOAT3 DefaultObjectScale = { 0.01f,0.01f,0.01f };
}

DriftManager::DriftManager() :
    m_pPlayer(nullptr),
    m_spawnTimer(0)
{
}

DriftManager::~DriftManager()
{
}

void DriftManager::Init(IEffect* effect)
{
	Primitive* woodModel = ModelManager::Instance().GetModel("BaseMaterial_Wood");
	Primitive* wireModel = ModelManager::Instance().GetModel("BaseMaterial_Wire");
	Primitive* plasticModel = ModelManager::Instance().GetModel("BaseMaterial_PlasticBottle");

    Material* material = MaterialManager::Instance().GetMaterial("BaseObjectMaterial");

    //===============Near Items
    std::vector<std::shared_ptr<ItemInstance>> woodItems(NearItemNum);
    for (auto& item : woodItems)
    {
        item = ItemDataBase::Instance().CreateItemInstance("Wood",1,-1,Layers::DRIFT);
        item->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(effect);
        item->DeActivate();
        item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
    }
    m_nearItemInstances.emplace(DriftObjectType::Wood, std::move(woodItems));

    std::vector<std::shared_ptr<ItemInstance>> wireItems(NearItemNum);
    for (auto& item : wireItems)
    {
        item = ItemDataBase::Instance().CreateItemInstance("Wire",1, -1, Layers::DRIFT);
        item->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(effect);
        item->DeActivate();
        item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
    }
    m_nearItemInstances.emplace(DriftObjectType::Wire, std::move(wireItems));

    std::vector<std::shared_ptr<ItemInstance>> plasticItems(NearItemNum);
    for (auto& item : plasticItems)
    {
        item = ItemDataBase::Instance().CreateItemInstance("Plastic", 1, -1, Layers::DRIFT);
        item->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(effect);
        item->DeActivate();
        item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
    }
    m_nearItemInstances.emplace(DriftObjectType::Plastic, std::move(plasticItems));


	//================Far Items
	std::vector<std::unique_ptr<GameObject>> woodFarItems(FarItemNum);
    for (auto& item : woodFarItems)
    {
        item = std::make_unique<GameObject>(GameObject::GameObjectType::Item);
        std::shared_ptr<RenderComponent> render = std::make_shared<RenderComponent>();
        render->Init(material, effect, woodModel);
        item->AddComponent<RenderComponent>(MyComponent::ComponentType::Render, render);
        item->GetTransform().SetScale(DefaultObjectScale);
        item->DeActivate();
	}
	m_farGameObjects.emplace(DriftObjectType::Wood, std::move(woodFarItems));

	std::vector<std::unique_ptr<GameObject>> wireFarItems(FarItemNum);
    for (auto& item : wireFarItems)
    {
        item = std::make_unique<GameObject>(GameObject::GameObjectType::Item);
        std::shared_ptr<RenderComponent> render = std::make_shared<RenderComponent>();
        render->Init(material, effect, wireModel);
        item->AddComponent<RenderComponent>(MyComponent::ComponentType::Render, render);
        item->GetTransform().SetScale(DefaultObjectScale);
        item->DeActivate();
    }
	m_farGameObjects.emplace(DriftObjectType::Wire, std::move(wireFarItems));

	std::vector<std::unique_ptr<GameObject>> plasticFarItems(FarItemNum);
    for (auto& item : plasticFarItems)
    {
        item = std::make_unique<GameObject>(GameObject::GameObjectType::Item);
        std::shared_ptr<RenderComponent> render = std::make_shared<RenderComponent>();
        render->Init(material, effect, plasticModel);
        item->AddComponent<RenderComponent>(MyComponent::ComponentType::Render, render);
        item->GetTransform().SetScale(DefaultObjectScale);
        item->DeActivate();
	}
	m_farGameObjects.emplace(DriftObjectType::Plastic, std::move(plasticFarItems));

    for (int i = 0; i < InitialSpawnNum; ++i) // 初始数量可调
    {
		int type = RandomManager::Instance().GetGenerator("Drift").GetRandomInt(0, 2);
        SpawnInitialItem(static_cast<DriftObjectType>(type));
    }
}

void DriftManager::Init(IEffect* effect, Player* player)
{
    SetPlayer(player);
	Init(effect);
}

void DriftManager::UnInit()
{
    for (auto& [type, items] : m_nearItemInstances)
    {
        for (auto& item : items) item.reset();
        items.clear();
    }
	m_nearItemInstances.clear();

	for (auto& [type, items] : m_farGameObjects)
    {
        for (auto& item : items) item.reset();
        items.clear();
    }
	m_farGameObjects.clear();
}

void DriftManager::Update(float tick)
{
  
    m_spawnTimer += tick;
    if (m_spawnTimer > SpawnInterval)
    {
        m_spawnTimer = 0.f;
        int type = RandomManager::Instance().GetGenerator("Drift").GetRandomInt(0, 2);
        CreateFarItem(static_cast<DriftObjectType>(type), GetRandomFarPosition());
    }

    //===========Update Near Items
    for(auto& [type, items] : m_nearItemInstances)
    {
        for (auto& item : items)
        {
            if (!item->GetActive()) continue;
			// Set velocity
            if (auto* physics = item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics))
            {
                physics->SetVelocity(FloatVector);
            }

            // Check active range
            if (CheckNearItemRecycle(item.get()))
            {
                item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();

                if (item->GetState() == ItemInstance::InActive)
                {
                    // Create far item
                    CreateFarItem(type, item->GetTransform().GetPosition());
                }
            }
            item->Update(tick);
        }
	}

	//===========Update Far Items
    for(auto& [type, items] : m_farGameObjects)
    {
        for (auto& farItem : items)
        {
            if (!farItem->GetActive()) continue;

			// Set Move vector
        	farItem->GetTransform().Translate(FloatVector, 1.f * tick);
            farItem->Update(tick);

			// if far item is further than max distance, recycle it
            if (ShouldRecycleFarItem(farItem->GetTransform().GetPosition()))
            {
                farItem->DeActivate();
                continue;
            }
       
			// if far item is near enough to player, spawn a near item and recycle far item
            if (ShouldSpawnNearItem(farItem->GetTransform().GetPosition()))
            {
                CreateNearItem(type, farItem->GetTransform().GetPosition());
                farItem->DeActivate();
            }
        }
	}


}

void DriftManager::Draw()
{
    //for (const auto& item : m_nearItems) item->Draw();
    //for (const auto& item : m_farItems)  item->Draw();

    for (const auto& [type, items] : m_nearItemInstances)
    {
        for (const auto& item : items) item->Draw();
    }

    for (const auto& [type, items] : m_farGameObjects)
    {
        for (const auto& item : items) item->Draw();
	}

}

float DriftManager::GetDistanceFromPlayer(const DirectX::XMFLOAT3& pos) const
{
    Vec3 itemPos = { pos.x, pos.y, pos.z };
    Vec3 playerPos = {
        m_pPlayer->GetTransform().GetPosition().x,
        m_pPlayer->GetTransform().GetPosition().y,
        m_pPlayer->GetTransform().GetPosition().z
    };
    return (playerPos - itemPos).Length();
}

bool DriftManager::CheckNearItemRecycle(ItemInstance* item)
{
    if (item->GetState() == ItemInstance::ItemState::Collected)
        return true;

    float dist = GetDistanceFromPlayer(item->GetTransform().GetPosition());
    if (dist > NearToFarDistance) 
    {
        item->SetState(ItemInstance::InActive); // Set Inactive mark
        return true;
    }
    return false;
}

bool DriftManager::ShouldSpawnNearItem(DirectX::XMFLOAT3 pos)
{
    float dist = GetDistanceFromPlayer(pos);
    return dist <= FarToNearDistance; 
}

bool DriftManager::ShouldRecycleFarItem(const DirectX::XMFLOAT3& pos)
{
    float dist = GetDistanceFromPlayer(pos);
    return dist > MaxDistance;
}



DirectX::XMFLOAT3 DriftManager::GetRandomFarPosition()
{
    float x = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(-10.f, 10.f);
    float y = 0.0f;   // 海面高度
	float z = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(90.f, 100.f);

    return DirectX::XMFLOAT3(x, y, z);
}

void DriftManager::CreateFarItem(DriftObjectType type, const DirectX::XMFLOAT3& pos)
{
	auto it = m_farGameObjects.find(type);
	if (it == m_farGameObjects.end()) return;
    for (auto& farItem : it->second)
    {
        if(!farItem->GetActive())
        {
            farItem->Activate();
            farItem->SetPosition(pos);
#ifdef _DEBUG
            DebugLog::Log("[DriftManager] Spawn far item at ({:.2f},{:.2f},{:.2f})",
                farItem->GetTransform().GetPosition().x,
                farItem->GetTransform().GetPosition().y,
                farItem->GetTransform().GetPosition().z);
#endif
            break;
        }
    }
}

void DriftManager::CreateNearItem(DriftObjectType type, const DirectX::XMFLOAT3& pos)
{
    auto it = m_nearItemInstances.find(type);
    if (it == m_nearItemInstances.end()) return ;

    auto& items = it->second;

    if (auto found = std::ranges::find_if(items, [](const auto& item) { return !item->GetActive(); });
        found != items.end())
    {
        auto& nearItem = *found;
        nearItem->Activate();
        nearItem->SetState(ItemInstance::Active);
        nearItem->SetPosition(pos);
        if (auto physics = nearItem->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics))
            physics->ActivatePhysics();
    }

}

void DriftManager::SpawnInitialItem(DriftObjectType type)
{
    if (!m_pPlayer)return;
    float minX = -10.f, maxX = 10.f;
    float minZ = 20.f, maxZ = 40.f;
    auto playerPos = m_pPlayer->GetTransform().GetPosition();

    float x = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(minX, maxX);
    float z = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(minZ, maxZ);

    DirectX::XMFLOAT3 pos = {
         playerPos.x + x,
         playerPos.y,
         playerPos.z + z
    };

    if (ShouldSpawnNearItem(pos))
    {
        CreateNearItem(type, pos);
    }
    else
    {
        CreateFarItem(type, pos);
    }
}
