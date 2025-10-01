#include "UIInventory.h"

#include "d3dUtil.h"
#include "Material.h"
#include "Inventory.h"
#include "TextureManager.h"

namespace
{
	static constexpr float IconScaler = 0.75f; // アイコンのスケール
	static constexpr float SlotScaler = .9f; // スロットのサイズ
	static constexpr float IconOffset = 6.f;
	static constexpr DirectX::XMFLOAT2 TextOffset = { 0.f, 4.f }; // アイテム数のテキストオフセット
	static constexpr float InventoryBackgroundSizeOffset = 5.f;
}

void UIInventory::Update(float dt)
{
	if (!m_isActive)return;

	// Input
	int move = 0;
	//===Wheel input
	if (gD3D->GetWheelMoveUnit() != 0)
	{
		move = gD3D->GetWheelMoveUnit(); // +1 or -1
		gD3D->SetWheelMoveUnit(0);           // リセット
		move *= -1; // ホイールの回転方向とインデックスの増減を合わせる
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

	}

	// Update player todo:this is temperary
	m_pInventory->UpdateItemOfPlayer(m_currentIndex, m_pPlayer);
}

void UIInventory::Init(Inventory* inventory, IEffect* effect, Material* bgMat, Material* slotBgMat, Material* iconMat, Material* chosenSlotMat, Primitive* model, UIFontSet* fontSet, const char* fontName, UIBrush* brush)
{
	m_slots.resize(inventory->GetMaxSlots());

	for (size_t i = 0; i < m_slots.size(); i++)
	{
		m_slots[i] = std::make_unique<UIInventorySlot>();
		m_slots[i]->Init(effect, slotBgMat, iconMat, model, fontSet, fontName, brush);
		// Set Click event for each slot
		m_slots[i]->SetOnClick([this, i]()
			{
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
			IconScaler * size.x,
			IconScaler * size.y,
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

		//=======Set hit rect
		m_slots[i]->SetHitRect();
	}

	// Set Chosen Button
	m_pChosenSlotFrame->GetTransform().SetPosition({ startPosX,pos.y,pos.z });
	m_pChosenSlotFrame->GetTransform().SetScale({
		SlotScaler * size.x,
		SlotScaler * size.y,
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
		m_slots[i]->GetBackground()->GetTransform().SetScale(slotSize);	// Set background size
		DirectX::XMFLOAT3 iconSize = {
			iconScale * slotSize.x,
			iconScale * slotSize.y,
			slotSize.z
		};
		m_slots[i]->GetItemIcon()->GetTransform().SetScale(iconSize);	// Set Icon size
		m_slots[i]->GetText()->SetScale(slotSize);	// Set text size

		// 位置設定
		DirectX::XMFLOAT3 slotPos = {
			startPosX + i * (slotSize.x + iconOffset),
			position.y,
			position.z
		};
		m_slots[i]->SetPosition(slotPos, textOffset);

		// Hit rect設定
		m_slots[i]->SetHitRect();
	}

	// 選択中スロットのフレーム設定
	m_pChosenSlotFrame->GetTransform().SetPosition({ startPosX, position.y, position.z });
	m_pChosenSlotFrame->GetTransform().SetScale({
		slotScale * slotSize.x,
		slotScale * slotSize.y,
		slotSize.z
		});
}

void UIInventory::LoadSizeAndPos(nlohmann::json& j, const char* uiName)
{
	if (!j.contains(uiName))
	{
		DebugLog::Log("UIInventory data is not found");
		return;
	}

	auto& ui = j[uiName];

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

		m_slots[i]->SetHitRect();
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
	for (size_t i = 0; i < m_slots.size(); i++)
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
	for (size_t i = 0; i < m_slots.size(); i++)
	{
		if (m_pInventory->GetSlots()[i].has_value())
		{
			std::string texName = m_pInventory->GetSlots()[i].value()->GetProto()->GetIconTextureName();
			// Get Texture
			Texture* iconTex = TextureManager::Instance().GetTexture(texName);
			// Set icon texture to item icon material(passed to shader)
			m_slots[i]->GetItemIcon()->GetRenderComponent()->GetMaterial()->SetTexture(Material::Albedo, iconTex);
		}
		else
		{
			// Set texture nullptr which will skip drawing icon& text
			m_slots[i]->GetItemIcon()->GetRenderComponent()->GetMaterial()->SetTexture(Material::Albedo, nullptr);
		}

		m_slots[i]->Draw();
	}

	// Draw chosen frame if a slot is chosen
	m_pChosenSlotFrame->Draw();

}

UIComponent* UIInventory::HitTest(float x, float y)
{
	for (auto& slot : m_slots)
	{
		if (slot->IsActive())
		{
			if (UIComponent* hit = slot->HitTest(x, y))
				return hit; 
		}
	}
	return nullptr; // 自己没被点中
}


