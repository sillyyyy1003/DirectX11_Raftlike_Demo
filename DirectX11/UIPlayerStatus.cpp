#include "UIPlayerStatus.h"
#include "HungerComponent.h"
#include "Player.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <fstream>

#include "d3dUtil.h"
#include "DebugLog.h"
using json = nlohmann::json;

UIPlayerStatus::UIPlayerStatus():
	m_pPlayer(nullptr)
{
}

void UIPlayerStatus::Init(Material* iconMaterial, Material* barBgMaterial, Material* barMaterial, IEffect* effect, Primitive* model)
{
	m_pHealthBar = std::make_unique<UIBar>();
	m_pHungerBar = std::make_unique<UIBar>();
	m_pHpIcon = std::make_unique<UIRender>();
	m_pHungerIcon = std::make_unique<UIRender>();

	m_pHpIcon->Init(iconMaterial, effect, model);
	m_pHungerIcon->Init(iconMaterial, effect, model);

	m_pHealthBar->Init(barBgMaterial, barMaterial, effect, effect);
	m_pHungerBar->Init(barBgMaterial, barMaterial, effect, effect);
}

void UIPlayerStatus::Init(Material* iconMaterial, Material* barBgMaterial, Material* barMaterial, IEffect* effect,
	Primitive* model, Texture* hpIconTex, Texture* hungerIconTex)
{
	m_pHealthBar = std::make_unique<UIBar>();
	m_pHungerBar = std::make_unique<UIBar>();
	m_pHpIcon = std::make_unique<UIRender>();
	m_pHungerIcon = std::make_unique<UIRender>();

	m_pHpIcon->Init(iconMaterial, effect, model);
	m_pHungerIcon->Init(iconMaterial, effect, model);

	m_pHealthBar->Init(barBgMaterial, barMaterial, effect, effect);
	m_pHungerBar->Init(barBgMaterial, barMaterial, effect, effect);
}

void UIPlayerStatus::Init(const MaterialList& hpMaterials, const MaterialList& hungerMaterials, IEffect* effect, Primitive* model)
{
	m_pHealthBar = std::make_unique<UIBar>();
	m_pHungerBar = std::make_unique<UIBar>();
	m_pHpIcon = std::make_unique<UIRender>();
	m_pHungerIcon = std::make_unique<UIRender>();

	
	m_pHpIcon->Init(hpMaterials[MaterialType::Icon], effect, model);
	m_pHealthBar->Init(hpMaterials[MaterialType::Background], hpMaterials[MaterialType::Bar], effect, effect);

	m_pHungerIcon->Init(hungerMaterials[MaterialType::Icon], effect, model);
	m_pHungerBar->Init(hungerMaterials[MaterialType::Background], hungerMaterials[MaterialType::Bar], effect, effect);
}

void UIPlayerStatus::Update(float dt)
{
	if (!m_isActive)return;
	float hungerValue = m_pPlayer->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->GetCurrentHungerPercentage();
	m_pHungerBar->UpdateUI(hungerValue);

	float healthValue = m_pPlayer->GetComponent<PlayerEntity>(MyComponent::ComponentType::LivingEntity)->GetCurrentHealthPercentage();
	m_pHealthBar->UpdateUI(healthValue);
}


void UIPlayerStatus::Draw()
{
	if (!m_isActive)return;
	m_pHealthBar->Draw();
	m_pHungerBar->Draw();

	m_pHpIcon->Draw();
	m_pHungerIcon->Draw();

}


void UIPlayerStatus::SetSize(const DirectX::XMFLOAT3& iconSize, const DirectX::XMFLOAT3& barSize)
{
	m_pHpIcon->GetTransform().SetScale(iconSize);
	m_pHungerIcon->GetTransform().SetScale(iconSize);

	m_pHealthBar->SetScale({barSize.x, barSize.y});
	m_pHungerBar->SetScale({ barSize.x, barSize.y });
	
}

void UIPlayerStatus::InitPositionAndSize(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& iconSize, const DirectX::XMFLOAT3& barSize, float distanceBetweenBars)
{
	//Set Size
	SetSize(iconSize, barSize);

	//Set Icon Pos;
	DirectX::XMFLOAT3 hpIconPos = { pos.x + iconSize.x / 2.f, pos.y - iconSize.y / 2.f, pos.z }; //左揃えで+half iconSize
	m_pHpIcon->GetTransform().SetPosition(hpIconPos);
	DirectX::XMFLOAT3 hpBarPos = { hpIconPos.x + iconSize.x / 2.f, hpIconPos.y, pos.z };	  // bar center aligned with icon
	m_pHealthBar->SetPosition(hpBarPos);

	DirectX::XMFLOAT3 hungerIconPos = { pos.x + iconSize.x / 2.f, pos.y - distanceBetweenBars - iconSize.y - iconSize.y / 2.f, pos.z };
	m_pHungerIcon->GetTransform().SetPosition(hungerIconPos);
	DirectX::XMFLOAT3 hungerBarPos = { hungerIconPos.x + iconSize.x / 2.f, hungerIconPos.y, pos.z };
	m_pHungerBar->SetPosition(hungerBarPos);

}

void UIPlayerStatus::LoadPositionAndSize(const char* fileName)
{
	struct UIPlayerStatusConfig
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 iconSize;
		DirectX::XMFLOAT3 barSize;
		float distanceBetweenBars;
	};

	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[UIPlayerStatus] Failed to open JSON file: {}", fileName);
		return;
	}

	json j;
	ifs >> j;

	UIPlayerStatusConfig config{};
	auto& ui = j["UIPlayerStatus"];

	config.position = JsonToXMFLOAT3(ui["position"]);
	config.iconSize = JsonToXMFLOAT3(ui["iconSize"]);
	config.barSize = JsonToXMFLOAT3(ui["barSize"]);
	config.distanceBetweenBars = ui["distanceBetweenBars"].get<float>();

	//Set position/size
	InitPositionAndSize(config.position, config.iconSize, config.barSize, config.distanceBetweenBars);
}

