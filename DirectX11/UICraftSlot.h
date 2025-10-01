#pragma once
#include "UIComponent.h"

/// @brief Single craft recipe slot
class UICraftSlot:
	public UIComponent
{
public:

	UICraftSlot();
	~UICraftSlot() override;

	/// @brief Initialize the ui elements with given parameters
	/// @param effect Shader effect for rendering
	/// @param bgMat background material&tex 
	/// @param iconMat icon material&tex
	/// @param model 
	void Init(IEffect* effect, Material* bgMat, Material* iconMat, Primitive* model);
	void Draw() override;

	/// @brief Set click function callback
	/// @param callback 
	void SetOnClick(const std::function<void()>& callback) { m_onClick = callback; }
	void OnClick(float x, float y) override;

	UIComponent* HitTest(float x, float y) override;
	void OnMouseUp(float x, float y) override;
	void OnMouseDown(float x, float y) override;

	UIRender* GetItemIcon() { return m_pItemIcon.get(); }
	UIRender* GetBackground() { return m_pBackground.get(); }

	void SetSlotPosition(const DirectX::XMFLOAT3& pos);
	void SetHitRect();
private:

	RECT m_slotRect;	//当たり判定
	std::unique_ptr<UIRender> m_pBackground;			// 背景表示するUIレンダラー
	std::unique_ptr<UIRender> m_pItemIcon;				// アイテムアイコンを表示するUIレンダラー

	bool m_isPressed;									// Button pressed state
	std::function<void()> m_onClick;					// Click event handler

};

