#include "UICraftSlot.h"

UICraftSlot::UICraftSlot():
	m_slotRect({0,0,100,100})
{
}

UICraftSlot::~UICraftSlot()
{
}

void UICraftSlot::Init(IEffect* effect, Material* bgMat, Material* iconMat, Primitive* model)
{
	// Init pointer
	m_pBackground = std::make_unique<UIRender>();
	m_pItemIcon = std::make_unique<UIRender>();

	// Set Material effect & model to background
	m_pBackground->Init(bgMat, effect, model);

	// Set Material effect & model to item icon
	m_pItemIcon->Init(iconMat, effect, model);
}

void UICraftSlot::Draw()
{
	m_pBackground->Draw();

	//もしアイテムがない場合はアイコンを非表示にする
	if (m_pItemIcon->GetRenderComponent()->GetMaterial()->GetTexture(Material::Albedo) == nullptr)return; // アイコンが設定されていない場合は描画しない

	m_pItemIcon->Draw();
}

void UICraftSlot::OnClick(float x, float y)
{
	if (m_onClick)
		m_onClick();
}

UIComponent* UICraftSlot::HitTest(float x, float y)
{
	if (x >= m_slotRect.left && x <= m_slotRect.right
		&& y >= m_slotRect.top && y <= m_slotRect.bottom)
		return this;

	return nullptr;
}



void UICraftSlot::OnMouseUp(float x, float y)
{
	m_isPressed = false;
}

void UICraftSlot::OnMouseDown(float x, float y)
{
	m_isPressed = true;
}

void UICraftSlot::SetSlotPosition(const DirectX::XMFLOAT3& pos)
{
	// set background
	m_pBackground->GetTransform().SetPosition(pos);

	// set icon
	DirectX::XMFLOAT3 iconPos = pos;
	iconPos.z += 0.1f;
	m_pItemIcon->GetTransform().SetPosition(iconPos);
}

void UICraftSlot::SetHitRect()
{
	float x = m_pItemIcon->GetTransform().GetPosition().x + WIN_WIDTH / 2.f;
	float y = WIN_HEIGHT / 2.f - m_pItemIcon->GetTransform().GetPosition().y;
	float width = m_pItemIcon->GetTransform().GetScale().x;
	float height = m_pItemIcon->GetTransform().GetScale().y;

	m_slotRect.left = (LONG)(x - width / 2);
	m_slotRect.right = (LONG)(x + width / 2);
	m_slotRect.top = (LONG)(y - height / 2);
	m_slotRect.bottom = (LONG)(y + height / 2);
}
