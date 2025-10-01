#include "UICraftCategoryPanel.h"
#include "d3dUtil.h"
#include "Inventory.h"
#include "TextureManager.h"

namespace
{
	static constexpr uint8_t MaxSlot = 3;	// slot number
	static constexpr float SlotSpacing = 6.f;	// slotŠÔ‚Ì‹——£
	static constexpr float PanelPadding = 5.f;	// slot‚Æpanel‚Ìpadding
	static constexpr float IconScale = 0.8f;
	static constexpr DirectX::XMFLOAT4 SelectedColor = { 1,1,1,1 };
	static constexpr DirectX::XMFLOAT4 UnselectedColor = { 0.8f,0.8f,0.8f,1 };
}

UICraftCategoryPanel::UICraftCategoryPanel():
	m_pBackground(nullptr),
	m_slots(MaxSlot),
	m_detailPanel(nullptr)
//	m_currentCategoryName("")
{
}

UICraftCategoryPanel::~UICraftCategoryPanel()
{
}

void UICraftCategoryPanel::Init(Material* panelBackgroundMaterial, Material* backgroundMaterial, Material* iconMaterial, IEffect* effect, Primitive* model)
{
	m_pBackground = std::make_unique<UIRender>();
	m_pBackground->Init(panelBackgroundMaterial, effect, model);

	for (int i = 0; i < m_slots.size(); i++)
	{
		// init render
		m_slots[i] = std::make_unique<UICraftSlot>();
		m_slots[i]->Init(effect, backgroundMaterial, iconMaterial, model);

		// init click event
		m_slots[i]->SetOnClick([this, i]()
			{
				// Set current index;
				m_currentIndex = i;
				// set name
				//m_currentCategoryName = CraftSystem::Instance().GetCategories()[m_currentIndex];

				// Set category detail panel active
				m_categoryPanel->SetActive(true);
				m_categoryPanel->UpdatePanel(m_currentIndex);
			});
	}
}


void UICraftCategoryPanel::InitPosAndSize(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& size)
{
	// set background size & pos
	float totalWidth = size.x + PanelPadding * 2; // size + left& right padding
	float totalHeight = m_slots.size() * size.y + (m_slots.size() - 1) * SlotSpacing + PanelPadding * 2;

	m_pBackground->GetTransform().SetScale({ totalWidth,totalHeight,1.f });
	m_pBackground->GetTransform().SetPosition(
		{ pos.x,
			pos.y - totalHeight / 2.f,
			pos.z
		});

	float startPosY = pos.y - totalWidth / 2.f;
	for (size_t i = 0; i < m_slots.size(); i++)
	{
		// set scale
		m_slots[i]->GetBackground()->GetTransform().SetScale({ size.x,size.y,1.f });
		m_slots[i]->GetItemIcon()->GetTransform().SetScale(
			{
				size.x * IconScale,
				size.y * IconScale,
				1.f
			});

		// set position
		DirectX::XMFLOAT3 slotPos = {
			pos.x,
			startPosY - i * (size.y + SlotSpacing),
			pos.z + 0.1f
		};
		m_slots[i]->SetSlotPosition(slotPos);

		m_slots[i]->SetHitRect();
	}

}

void UICraftCategoryPanel::LoadSizeAndPos(nlohmann::json& j, const char* uiName)
{
	if (!j.contains(uiName))
	{
		DebugLog::Log("UIInventory data is not found");
		return;
	}

	auto& ui = j[uiName];

	DirectX::XMFLOAT3 pos = JsonToXMFLOAT3(ui["position"]);	// panel position
	DirectX::XMFLOAT3 slotSize = JsonToXMFLOAT3(ui["slotSize"]);	// slot size
	float iconScale = ui["slotScale"].get<float>();
	float panelSpacing = ui["slotSpacing"].get<float>();
	float panelPadding = ui["panelPadding"].get<float>();

	// set background size & pos
	float totalWidth = slotSize.x + panelPadding * 2; // slotSize + left& right padding
	float totalHeight = m_slots.size() * slotSize.y + (m_slots.size() - 1) * panelSpacing + panelPadding * 2;

	m_pBackground->GetTransform().SetScale({ totalWidth,totalHeight,1.f });
	m_pBackground->GetTransform().SetPosition(
		{ pos.x,
			pos.y - totalHeight / 2.f,
			pos.z
		});

	float startPosY = pos.y - totalWidth / 2.f;
	for (size_t i = 0; i < m_slots.size(); i++)
	{
		// set scale
		m_slots[i]->GetBackground()->GetTransform().SetScale({ slotSize.x,slotSize.y,1.f });
		m_slots[i]->GetItemIcon()->GetTransform().SetScale(
			{
				slotSize.x * iconScale,
				slotSize.y * iconScale,
				slotSize.z
			});

		// set position
		DirectX::XMFLOAT3 slotPos = {
			pos.x,
			startPosY - i * (slotSize.y + panelSpacing),
			pos.z + 0.1f
		};
		m_slots[i]->SetSlotPosition(slotPos);

		m_slots[i]->SetHitRect();
	}
}

void UICraftCategoryPanel::Draw()
{
	m_pBackground->Draw();
	for (int i = 0; i < m_slots.size(); i++)
	{
		std::string categoryName = CraftSystem::Instance().GetCategories()[i];
		std::string texName = CraftSystem::Instance().GetIconName(categoryName);
		Texture* iconTex = TextureManager::Instance().GetTexture(texName);
		Material* iconMaterial = m_slots[i]->GetItemIcon()->GetRenderComponent()->GetMaterial();
		// Get icon texture
	
		iconMaterial->SetTexture(Material::Albedo, iconTex);

		// if any slot is clicked make slot brighter
		if (m_currentIndex != -1)
		{
			if (m_currentIndex == i)iconMaterial->SetDiffuse(SelectedColor);
			else iconMaterial->SetDiffuse(UnselectedColor);
		}

		m_slots[i]->Draw();
	}
}

UIComponent* UICraftCategoryPanel::HitTest(float x, float y)
{
	for (auto& slot : m_slots)
	{
		if (slot->IsActive())
		{
			if (UIComponent* hit = slot->HitTest(x, y))
				return hit;
		}
	}
	return nullptr; 
}

void UICraftCategoryPanel::SetPanels(UICraftCategoryDetailPanel* categoryDetailPanel,
	UICraftDetailPanel* recipeDetailPanel)
{
	m_categoryPanel = categoryDetailPanel;
	m_detailPanel = recipeDetailPanel;
}
