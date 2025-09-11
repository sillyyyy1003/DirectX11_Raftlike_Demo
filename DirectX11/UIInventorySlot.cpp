#include "UIInventorySlot.h"
#include "d3dUtil.h"
#include "Inventory.h"
#include "KInput.h"
#include "TextureManager.h"
#include "d3dUtil.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace
{
	static constexpr float IconScale = 0.75f; // アイコンのスケール
	static constexpr float SlotScale = .9f; // スロットのサイズ
	static constexpr float IconOffset = 6.f;
	static constexpr DirectX::XMFLOAT2 TextOffset = { -8.f, 4.f }; // アイテム数のテキストオフセット
	static constexpr float InventoryBackgroundSizeOffset = 5.f;
}
UIInventorySlot::UIInventorySlot()
{
}

void UIInventorySlot::Init(IEffect* effect, Material* bgMat, Material* iconMat, Primitive* model, UIFontSet* fontSet, const char* fontName, UIBrush* brush)
{
	// Init pointer
	m_pBackground = std::make_unique<UIRender>();
	m_pItemIcon = std::make_unique<UIRender>();
	m_pText = std::make_unique<UIText>();

	// Init text
	m_pText->Init(fontSet, fontName, brush);
	// text alignment should be bottom in y and right align in x
	m_pText->SetTextAlignment(UIText::TextAlign::Right);
	m_pText->SetParagraphAlignment(UIText::ParagraphAlign::Bottom);

	// Set Material effect & model to background
	m_pBackground->SetEffect(effect);
	m_pBackground->SetMaterial(bgMat);
	m_pBackground->SetModel(model);

	// Set Material effect & model to item icon
	m_pItemIcon->SetEffect(effect);
	m_pItemIcon->SetMaterial(iconMat);
	m_pItemIcon->SetModel(model);
}


void UIInventorySlot::Draw()
{
	m_pBackground->Draw();

	//もしアイテムがない場合はアイコンを非表示にする
	if (m_pItemIcon->GetRenderComponent()->GetMaterial()->GetTexture(Material::Albedo) == nullptr)return; // アイコンが設定されていない場合は描画しない
	
	m_pItemIcon->Draw();
	m_pText->Draw();
}


void UIInventorySlot::SetPosition(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& textOffset)
{
	m_pBackground->GetTransform().SetPosition(pos);

	DirectX::XMFLOAT3 iconPos = pos;
	iconPos.z -= 0.1f; // アイコンは背景の少し手前に表示
	m_pItemIcon->GetTransform().SetPosition(pos);

	m_pText->SetPosition({
		pos.x + textOffset.x,
		pos.y + textOffset.y,
		pos.z
		}); // アイテム数は少し上に表示

}

void UIInventorySlot::OnClick(float x, float y)
{
	if (m_onClick)
		m_onClick();
}

//=============Inventory UI
void UIInventory::Update(float dt)
{
	if (!m_isActive)return;
	// Inpu
	int move = 0;
	//===Wheel input
	if (gD3D->GetWheelMoveUnit() != 0)
	{
		move = gD3D->GetWheelMoveUnit(); // +1 or -1
		gD3D->SetWheelMoveUnit(0);           // リセット
		move*=-1; // ホイールの回転方向とインデックスの増減を合わせる
	}
	if (move != 0)
	{
		m_currentIndex += move;

		// wrap around
		if (m_currentIndex < 0)
			m_currentIndex = (int)(m_slots.size() - 1);
		else if (m_currentIndex >= (int)m_slots.size())
			m_currentIndex = 0;

		// フレーム位置更新
		m_pChosenSlotFrame->GetTransform().SetPosition(
			m_slots[m_currentIndex]->GetBackground()->GetTransform().GetPosition()
		);

		// Update player
		m_pInventory->UpdateItemOfPlayer(m_currentIndex, m_pPlayer);
	}


	//if (KInput::IsKeyTrigger(VK_LBUTTON))
	//{
	//	m_pInventory->ConsumeItem(m_currentIndex, m_pPlayer);
	//}

}

void UIInventory::Init(Inventory* inventory, IEffect* effect, Material* bgMat, Material* slotBgMat, Material* iconMat, Material* chosenSlotMat, Primitive* model, UIFontSet* fontSet, const char* fontName, UIBrush* brush)
{
	m_slots.resize(inventory->GetMaxSlots());

	for (size_t i = 0; i < m_slots.size(); i++)
	{
		m_slots[i] = std::make_unique<UIInventorySlot>();
		m_slots[i]->Init(effect, slotBgMat, iconMat, model, fontSet, fontName, brush);
		// Set Click event for each slot
		m_slots[i]->SetOnClick([this, i]() {
			m_currentIndex = (int)i; // Set current index to clicked slot
			// Set chosen slot frame position
			m_pChosenSlotFrame->GetTransform().SetPosition(m_slots[i]->GetBackground()->GetTransform().GetPosition());
			});
	}
	// Set text provider for each slot(for object count)
	SetInventory(inventory);

	// Init background
	m_pBackground = std::make_unique<UIRender>();
	m_pBackground->SetEffect(effect);
	m_pBackground->SetMaterial(bgMat);
	m_pBackground->SetModel(model);

	// Init chosen slot frame
	m_pChosenSlotFrame = std::make_unique<UIRender>();
	m_pChosenSlotFrame->SetEffect(effect);
	m_pChosenSlotFrame->SetMaterial(chosenSlotMat);
	m_pChosenSlotFrame->SetModel(model);
}

void UIInventory::InitSizeAndPos(const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT3& pos)
{
	// Set background size and position
	float totalWidth = m_slots.size() * size.x + (m_slots.size() - 1) * IconOffset + InventoryBackgroundSizeOffset * 2;
	float totalHeight = size.y + InventoryBackgroundSizeOffset * 2;
	m_pBackground->GetTransform().SetPosition(pos);
	m_pBackground->GetTransform().SetScale(totalWidth, totalHeight, 1.f);

	// Set Slot size & position
	float startPosX = pos.x - totalWidth / 2.f + size.x / 2.f + InventoryBackgroundSizeOffset;
	for (size_t i = 0; i < m_slots.size(); i++)
	{
		//======Set size for ui elements
		m_slots[i]->GetBackground()->GetTransform().SetScale(size);
		DirectX::XMFLOAT3 iconScale = {
			IconScale * size.x,
			IconScale * size.y,
			size.z
		};//Icon would be smaller than background
		m_slots[i]->GetItemIcon()->GetTransform().SetScale(iconScale);
		//Set text scale
		m_slots[i]->GetText()->SetScale(size);

		//======Set position for ui elements
		// UIがくっ付けないように、スロットの位置をずらす
		DirectX::XMFLOAT3 slotPos = {
			startPosX + i * (size.x + IconOffset),
			pos.y,
			pos.z
		};

		m_slots[i]->SetPosition(slotPos, TextOffset);
	}

	// Set Chosen Button
	m_pChosenSlotFrame->GetTransform().SetPosition({ startPosX,pos.y,pos.z });
	m_pChosenSlotFrame->GetTransform().SetScale({
		SlotScale * size.x,
		SlotScale * size.y,
		size.z
		});
}

void UIInventory::LoadSizeAndPos(const char* filePath)
{
	std::ifstream ifs(filePath);
	if (!ifs.is_open())
	{
		DebugLog::LogError("[UIInventory] Failed to open JSON file: {}", filePath);
		return;
	}

	nlohmann::json j;
	ifs >> j;

	auto& ui = j["UIInventory"];

	DirectX::XMFLOAT3 position = JsonToXMFLOAT3(ui["position"]);
	DirectX::XMFLOAT3 slotSize = JsonToXMFLOAT3(ui["slotSize"]);
	float slotScale = ui["slotScale"].get<float>();
	float iconScale = ui["iconScale"].get<float>();
	float iconOffset = ui["iconOffset"].get<float>();
	DirectX::XMFLOAT2 textOffset = JsonToXMFLOAT2(ui["textOffset"]);
	float backgroundSizeOffset = ui["backgroundSizeOffset"].get<float>();

	// 背景のサイズと位置
	float totalWidth = m_slots.size() * slotSize.x + (m_slots.size() - 1) * iconOffset + backgroundSizeOffset * 2;
	float totalHeight = slotSize.y + backgroundSizeOffset * 2;
	m_pBackground->GetTransform().SetPosition(position);
	m_pBackground->GetTransform().SetScale(totalWidth, totalHeight, 1.f);

	// スロットのサイズ & 位置
	float startPosX = position.x - totalWidth / 2.f + slotSize.x / 2.f + backgroundSizeOffset;
	for (size_t i = 0; i < m_slots.size(); i++)
	{
		// サイズ設定
		m_slots[i]->GetBackground()->GetTransform().SetScale(slotSize);
		DirectX::XMFLOAT3 iconSize = {
			iconScale * slotSize.x,
			iconScale * slotSize.y,
			slotSize.z
		};
		m_slots[i]->GetItemIcon()->GetTransform().SetScale(iconSize);
		m_slots[i]->GetText()->SetScale(slotSize);

		// 位置設定
		DirectX::XMFLOAT3 slotPos = {
			startPosX + i * (slotSize.x + iconOffset),
			position.y,
			position.z
		};
		m_slots[i]->SetPosition(slotPos, textOffset);
	}

	// 選択中スロットのフレーム設定
	m_pChosenSlotFrame->GetTransform().SetPosition({ startPosX, position.y, position.z });
	m_pChosenSlotFrame->GetTransform().SetScale({
		slotScale * slotSize.x,
		slotScale * slotSize.y,
		slotSize.z
		});
}

void UIInventory::SetInventory(Inventory* inventory)
{
	m_pInventory = inventory;
	for(size_t i= 0; i < m_slots.size(); i++)
	{
		// Set text contents
		m_slots[i]->GetText()->SetProvider(m_pInventory, UIFormat::FormatItemNumber, (int)i);
	}
}

void UIInventory::SetPlayer(Player* player)
{
	m_pPlayer = player;
	m_pInventory->UpdateItemOfPlayer(m_currentIndex, m_pPlayer);
}

void UIInventory::Draw()
{
	if (!m_isActive)return;
	// Draw background
	m_pBackground->Draw();
	// Draw Slot
	for(size_t i = 0; i < m_slots.size(); i++)
	{
		if (m_pInventory->GetSlots()[i].has_value())
		{
			// Texture name format: "ItemName_Icon" e.g. "Food_Apple_Icon"
			std::string texName = m_pInventory->GetSlots()[i].value()->GetName() + "_Icon";
			// Get Texture
			Texture* iconTex = TextureManager::Instance().GetTexture(texName);
			// Set icon texture to item icon material(passed to shader)
			m_slots[i]->GetItemIcon()->GetRenderComponent()->GetMaterial()->SetTexture(Material::Albedo, iconTex);
		}else
		{
			// Set texture nullptr which will skip drawing icon& text
			m_slots[i]->GetItemIcon()->GetRenderComponent()->GetMaterial()->SetTexture(Material::Albedo,nullptr);
		}

		m_slots[i]->Draw();
	}

	// Draw chosen frame if a slot is chosen
	m_pChosenSlotFrame->Draw();
	
}


