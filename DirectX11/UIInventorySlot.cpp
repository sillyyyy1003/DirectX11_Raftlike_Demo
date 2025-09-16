#include "UIInventorySlot.h"
#include "d3dUtil.h"
#include "Inventory.h"
#include "KInput.h"
#include "TextureManager.h"
#include "d3dUtil.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
	// Set background position
	m_pBackground->GetTransform().SetPosition(pos);

	// Set icon position
	DirectX::XMFLOAT3 iconPos = pos;
	iconPos.z -= 0.1f; // アイコンは背景の少し手前に表示
	m_pItemIcon->GetTransform().SetPosition(pos);

	// Set text position
	m_pText->SetPosition({
		pos.x + textOffset.x,
		pos.y + textOffset.y,
		pos.z
		}); // アイテム数は少し上に表示

}

void UIInventorySlot::SetHitRect()
{
	float x = m_pItemIcon->GetTransform().GetPosition().x + WIN_WIDTH / 2.f;
	float y = WIN_HEIGHT / 2.f- m_pItemIcon->GetTransform().GetPosition().y  ;
	float width = m_pItemIcon->GetTransform().GetScale().x;
	float height = m_pItemIcon->GetTransform().GetScale().y;

	m_slotRect.left = (LONG)(x - width / 2);
	m_slotRect.right = (LONG)(x + width / 2);
	m_slotRect.top = (LONG)(y - height / 2);
	m_slotRect.bottom = (LONG)(y + height / 2);

}

void UIInventorySlot::OnClick(float x, float y)
{
	if (m_onClick)
		m_onClick();
}

UIComponent* UIInventorySlot::HitTest(float x, float y)
{
	if (x >= m_slotRect.left && x <= m_slotRect.right
		&& y >= m_slotRect.top && y <= m_slotRect.bottom)
		return this;

	return nullptr;
}

void UIInventorySlot::OnMouseEnter()
{
	m_isHovered = true;
	if (m_onHover)
		m_onHover();
}

void UIInventorySlot::OnMouseExit()
{
	m_isHovered = false;
	if (m_onExit)
		m_onExit();
}

void UIInventorySlot::OnMouseUp(float x, float y)
{
	m_isPressed = false;
}

void UIInventorySlot::OnMouseDown(float x, float y)
{
	m_isPressed = true;
}




