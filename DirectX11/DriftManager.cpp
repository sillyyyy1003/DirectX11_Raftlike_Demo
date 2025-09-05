#include "DriftManager.h"
#include <memory>
#include "ItemDataBase.h"
#include "KInput.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "PhysicsComponent.h"
#include "Player.h"
#include "RandomManager.h"
#include "RenderComponent.h"

namespace
{
    static constexpr int NearItemNum = 10;
    static constexpr int FarItemNum = 500;

    // ==== 距離判定用パラメーター ====
    static constexpr float NearToFarDistance = 25.f; // Near → Far 回收
    static constexpr float FarToNearDistance = 15.f; // Far → Near 回收
    static constexpr float MaxDistance = -70.f; // FarItem 最大存活距离

    static constexpr float SpawnInterval = 2.f;    // FarItem 自动生成间隔
    static constexpr DirectX::XMFLOAT3 FloatVector = { 0,0,-2 };

    static constexpr uint InitialSpawnNum = 20;
    static constexpr DirectX::XMFLOAT3 WoodSize = { 0.01f,0.05f,0.01f };
    static constexpr DirectX::XMFLOAT3 WireSize = { 0.05f,0.05f,0.05f };
    static constexpr DirectX::XMFLOAT3 PlasticBarrelSize = { 0.02f,0.02f,0.02f };

    static constexpr float SpawnRangeX = 15.f;
    static constexpr DirectX::XMFLOAT2 SpawnRangeZ = { 70.f, 80.f };

    static constexpr DirectX::XMFLOAT3 NearItemObjectDefaultPos = { 0,100,0 };
    enum ObjectState:int
    {
        Active = 0,         // Spawned and waiting to be picked up
        InActive = 1,       // Can be spawned
        WaitToRecycle = 2,  // Marked for recycling
    
	};

    struct SpawnParam
    {
        DirectX::XMFLOAT3 pos;
        DriftManager::DriftObjectType type;
    };
}

DriftManager::DriftManager() :
    m_pPlayer(nullptr),
    m_spawnTimer(0),
    m_spawnRangeX(-SpawnRangeX, SpawnRangeX),
    m_spawnRangeZ(SpawnRangeZ)
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
    Material* material1 = MaterialManager::Instance().GetMaterial("BaseObjectMaterial1");
    //===============Near Items
    std::vector<std::shared_ptr<ItemInstance>> woodItems(NearItemNum);
    for (auto& item : woodItems)
    {
        item = ItemDataBase::Instance().CreateItemInstance("Wood", 1, -1, Layers::DRIFT);
        item->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(effect);
        item->DeActivate();
        item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
        item->SetState(InActive);
        item->SetPosition(NearItemObjectDefaultPos);
    }
    m_nearItemInstances.emplace(DriftObjectType::Wood, std::move(woodItems));

    std::vector<std::shared_ptr<ItemInstance>> wireItems(NearItemNum);
    for (auto& item : wireItems)
    {
        item = ItemDataBase::Instance().CreateItemInstance("Wire", 1, -1, Layers::DRIFT);
        item->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(effect);
        item->DeActivate();
        item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
        item->SetState(InActive);
        item->SetPosition(NearItemObjectDefaultPos);

    }
    m_nearItemInstances.emplace(DriftObjectType::Wire, std::move(wireItems));

    std::vector<std::shared_ptr<ItemInstance>> plasticItems(NearItemNum);
    for (auto& item : plasticItems)
    {
        item = ItemDataBase::Instance().CreateItemInstance("Plastic", 1, -1, Layers::DRIFT);
        item->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(effect);
        item->DeActivate();
        item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
        item->SetState(InActive);
        item->SetPosition(NearItemObjectDefaultPos);

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
        item->GetTransform().SetScale(WoodSize);
        item->DeActivate();
    }
    m_farGameObjects.emplace(DriftObjectType::Wood, std::move(woodFarItems));

    std::vector<std::unique_ptr<GameObject>> wireFarItems(FarItemNum);
    for (auto& item : wireFarItems)
    {
        item = std::make_unique<GameObject>(GameObject::GameObjectType::Item);
        std::shared_ptr<RenderComponent> render = std::make_shared<RenderComponent>();
        render->Init(material1, effect, wireModel);
        item->AddComponent<RenderComponent>(MyComponent::ComponentType::Render, render);
        item->GetTransform().SetScale(WireSize);
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
        item->GetTransform().SetScale(PlasticBarrelSize);
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
    m_nearItemInstances.clear();
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

    //===========Update Far Items

    std::vector<SpawnParam> spawnNearPos(0);
    for (auto& [type, items] : m_farGameObjects)
    {
        for (auto& farItem : items)
        {
            if (!farItem->GetActive()) continue;

            // if far item is further than max distance, recycle it
            if (ShouldRecycleFarItem(farItem->GetTransform().GetPosition()))
            {
                farItem->DeActivate();
                continue;
            }

            // if far item is near enough to player, spawn a near item and recycle far item
            if (ShouldSpawnNearItem(farItem->GetTransform().GetPosition()))
            {
                farItem->DeActivate();
                DirectX::XMFLOAT3 pos = farItem->GetTransform().GetPosition();
				spawnNearPos.push_back({ pos,type});
                DebugLog::Log("[DriftManager] : Recycle near item at {}, {}, {}", pos.x, pos.y, pos.z);

            }
        }
    }



    //===========Update Near Items states
    std::vector<SpawnParam> spawnFarPos(0);
    for (auto& [type, items] : m_nearItemInstances)
    {
        for (auto& item : items)
        {
            // if item wait to recycle, inactive it&skip loop
            if (item->GetState() != Active)
            {
                item->DeActivate();
                item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->DeActivePhysics();
                item->SetPosition(NearItemObjectDefaultPos);
            }

            if (!item->GetActive()) continue;

            // Check active range
            if (CheckNearItemRecycle(item.get()))
            {
                item->SetState(WaitToRecycle); // Set Inactive mark
                DirectX::XMFLOAT3 pos = item->GetTransform().GetPosition();
                spawnFarPos.push_back({ pos,type });
                DebugLog::Log("[DriftManager] : Recycle far item at {},{},{}", pos.x, pos.y, pos.z);
            }
        }
    }


    // Spawn new items
    for(auto& it : spawnNearPos)
    {
        CreateNearItem(it.type, it.pos);
	}

    for(auto& it : spawnFarPos)
    {
        CreateFarItem(it.type, it.pos);
	}


    // far object update
    for (auto& [type, items] : m_farGameObjects)
    {
        for (auto& farItem : items)
        {
            if (!farItem->GetActive()) continue;

            // Set Move vector
            farItem->GetTransform().Translate(FloatVector, tick);
            farItem->Update(tick);
        }
    }

    // near object update
    for (auto& [type, items] : m_nearItemInstances)
    {
        for (auto& item : items)
        {
            if (!item->GetActive()) continue;

            // Set velocity
            if (auto* physics = item->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics))
            {
                physics->SetVelocity(FloatVector);
            }
            item->Update(tick);
        }
    }


}

void DriftManager::Draw()
{

    //================ spawn far
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
    float dist = GetDistanceFromPlayer(item->GetTransform().GetPosition());
    if (dist > NearToFarDistance)
    {
        return true;
    }
    return false;
}

bool DriftManager::ShouldSpawnNearItem(DirectX::XMFLOAT3 pos)
{
    float dist = GetDistanceFromPlayer(pos);
    return dist < FarToNearDistance;
}

bool DriftManager::ShouldRecycleFarItem(const DirectX::XMFLOAT3& pos)
{
    //float dist = GetDistanceFromPlayer(pos);
    //return dist > MaxDistance;
    return pos.z < MaxDistance;
}



DirectX::XMFLOAT3 DriftManager::GetRandomFarPosition()
{
    float x = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(m_spawnRangeX.x, m_spawnRangeX.y);
    float y = 0.0f;   // 海面高度
    float z = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(m_spawnRangeZ.x, m_spawnRangeZ.y);

    return DirectX::XMFLOAT3(x, y, z);
}

void DriftManager::CreateFarItem(DriftObjectType type, const DirectX::XMFLOAT3& pos)
{
    auto it = m_farGameObjects.find(type);
    if (it == m_farGameObjects.end()) return;
    for (auto& farItem : it->second)
    {
        if (!farItem->GetActive())
        {
            farItem->Activate();
            farItem->SetPosition(pos);
            break;
        }
    }
}

void DriftManager::CreateNearItem(DriftObjectType type, const DirectX::XMFLOAT3& pos)
{
    auto it = m_nearItemInstances.find(type);
    if (it == m_nearItemInstances.end()) return;

    auto& items = it->second;

    if (auto found = std::ranges::find_if(items, [](const auto& item) { return !item->GetActive(); });
        found != items.end())
    {
        auto& nearItem = *found;
        nearItem->Activate();
        nearItem->SetState(Active);
        nearItem->SetPosition(pos);
        nearItem->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->ActivatePhysics();

    }

}

void DriftManager::SpawnInitialItem(DriftObjectType type)
{
    if (!m_pPlayer)return;
    float minX = m_spawnRangeX.x, maxX = m_spawnRangeX.y;
    float minZ = 20.f, maxZ = 80.f;
    auto playerPos = m_pPlayer->GetTransform().GetPosition();

    float x = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(minX, maxX);
    float z = RandomManager::Instance().GetGenerator("Drift").GetRandomFloat(minZ, maxZ);

    DirectX::XMFLOAT3 pos = {
         playerPos.x + x,
         0.f,
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

void DriftManager::UpdateFarObjectStatus()
{
}
