#include "UICraftCategoryDetailPanel.h"
#include "CraftSystem.h"
#include "ItemDataBase.h"
#include "TextureManager.h"
#include "UICraftDetailPanel.h"

namespace
{
	static constexpr int MaxSlotSize = 20;
	static constexpr float PanelPadding = 5.f;
	static constexpr float PanelSpacing = 10.f;
}

UICraftCategoryDetailPanel::UICraftCategoryDetailPanel():
	m_pBackground(nullptr),
	m_pTitle(nullptr),
	m_currentCategoryName(""),
	m_iconNames(0),
	m_slots(MaxSlotSize)
{
}

void UICraftCategoryDetailPanel::Init(UIFontSet* fontSet, UIBrush* brush, const char* titleFont, const char* slotText,
	Material* PanelBackground, Material* slotMaterial, IEffect* effect, Primitive* model)
{
	m_pTitle = std::make_unique<UIText>();
	m_pTitle->Init(fontSet, titleFont, brush);
	m_pTitle->SetTextAlignment(UIText::TextAlign::Left);
	m_pTitle->SetParagraphAlignment(UIText::ParagraphAlign::Middle);

	m_pBackground = std::make_unique<UIRender>();
	m_pBackground->Init(PanelBackground, effect, model);


	for (size_t i = 0; i < m_slots.size(); i++)
	{
		m_slots[i] = std::make_unique<UIButton>();
		m_slots[i]->Init(effect, slotMaterial, model);

		// Set click event for each slot
		m_slots[i]->SetOnClick([this, i]()
			{
				std::string recipeName = CraftSystem::Instance().GetRecipesByCategory(this->m_currentCategoryName)[i]->GetResultItemName();
				// Update detail panel data
				m_pDetailPanel->UpdatePanelInfo(recipeName);
				// Activate detail panel
				m_pDetailPanel->SetActive(true);
			});
	}

}

UIComponent* UICraftCategoryDetailPanel::HitTest(float x, float y)
{
	for (auto& slot : m_slots)
	{
		if (slot->IsActive())//表示しているslotだけをHitTest
		{
			if (UIComponent* hit = slot->HitTest(x, y))
				return hit;
		}
	}
	return nullptr;
}

void UICraftCategoryDetailPanel::UpdatePanel(int currentIndex)
{
#ifdef _DEBUG
	// Safety check
	if (currentIndex >= CraftSystem::Instance().GetCategories().size())return;
#endif
	m_currentCategoryName = CraftSystem::Instance().GetCategories()[currentIndex];
	// Set title tex
	m_pTitle->SetStaticText(m_currentCategoryName);

	// Get recipes
	CraftSystem::CraftRecipes recipes = CraftSystem::Instance().GetRecipesByCategory(m_currentCategoryName);

	// clear icon names list
	m_iconNames.clear();
	m_iconNames.resize(recipes.size());
	// Set recipe icon
	for(size_t i =0;i<recipes.size();i++)
	{
		std::string itemName = recipes[i]->GetResultItemName();
		std::string iconTexName = ItemDataBase::Instance().GetItem(itemName.c_str())->GetIconTextureName();
		m_iconNames[i] = iconTexName;
		m_slots[i]->SetActive(true);	// 必要なSlotだけを表示
	}

}

void UICraftCategoryDetailPanel::SetActive(bool isActive)
{
	// パネルを閉ざす時、全てのスロットを非表示にする
	if (isActive==false)
	{
		for(auto& slot:m_slots)
		{
			slot->SetActive(isActive);
		}
		return;
	}

	this->m_isActive = isActive;
	
}

void UICraftCategoryDetailPanel::Draw()
{
	if (!IsActive())return;

	m_pBackground->Draw();
	m_pTitle->Draw();

	// Draw slot icons according to icon name list;
	for(size_t i=0;i< m_iconNames.size();i++)
	{
		Texture* iconTexPtr = TextureManager::Instance().GetTexture(m_iconNames[i]);
#ifdef _DEBUG
		assert(iconTexPtr);
#endif
		m_slots[i]->GetUiRenderComponent()->GetMaterial()->SetTexture(Material::Albedo, iconTexPtr);
		m_slots[i]->Draw();
	}

}

void UICraftCategoryDetailPanel::InitPosAndSize(const DirectX::XMFLOAT3& pos, float panelWidth,
	const DirectX::XMFLOAT2& titleSize, const DirectX::XMFLOAT2& slotSize)
{
	// Set background
	float totalHeight = PanelPadding * 2 + titleSize.y + PanelSpacing + slotSize.y * m_slots.size() + PanelSpacing * m_slots.size();
	m_pBackground->GetTransform().SetScale({ panelWidth,totalHeight,1.0f });

	m_pBackground->GetTransform().SetPosition({
		pos.x + panelWidth / 2.f,
		pos.y - totalHeight / 2.f,
		pos.z
		});


	// Set title
	m_pTitle->SetScale({titleSize.x,titleSize.y,1.f});
	m_pTitle->SetPosition({
		pos.x + PanelPadding + titleSize.x / 2.f,
		pos.y - PanelPadding - titleSize.y / 2.f,
		pos.z
	});

	// Set Slots
	for(size_t i=0; i< m_slots.size();i++)
	{
		m_slots[i]->SetScale({ slotSize.x,slotSize.y,1.f });
		m_slots[i]->SetPosition({
			
			});

		DirectX::XMFLOAT3 buttonPos = {
			pos.x + panelWidth / 2.f,
			pos.y - PanelPadding - titleSize.y - PanelSpacing - (slotSize.y / 2.f) - i * (slotSize.y + PanelSpacing),
			pos.z + 0.1f
		};
		
		m_slots[i]->SetButtonRect(buttonPos, slotSize.x, slotSize.y);
	}

}

