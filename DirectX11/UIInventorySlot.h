#pragma once
#include "UIComponent.h"

class Model;
class UIBasicEffect;

/// @brief 持ちアイテムを表示するUIコンポーネント
class UIInventorySlot :
    public UIComponent
{
public:

	UIInventorySlot();
	~UIInventorySlot() override = default;

	/// @brief Initialize the ui elements with given parameters
	/// @param effect Shader effect for rendering
	/// @param bgMat background material&tex 
	/// @param iconMat icon material&tex
	/// @param model square primitives
	/// @param fontSet text font set
	/// @param fontName font name to use for text rendering
	/// @param brush color brush for text 
	void Init(IEffect* effect, Material* bgMat, Material* iconMat, Primitive* model, UIFontSet* fontSet, const char* fontName, UIBrush* brush);

	void Draw() override;

	UIRender* GetBackground() const { return m_pBackground.get(); }
	UIRender* GetItemIcon() const { return m_pItemIcon.get(); }
	UIText* GetText() const { return m_pText.get(); }

	/// Set slot render(background/icon/text) position
	void SetPosition(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& textOffset={0,0});

	/// @brief Set hit rect for click/hover event(assume position& size has already set)
	void SetHitRect();

	/// @brief Set click function callback
	/// @param callback 
	void SetOnClick(const std::function<void()>& callback) { m_onClick = callback; }
	void OnClick(float x, float y) override;

	UIComponent* HitTest(float x, float y) override;
	void OnMouseEnter() override;
	void OnMouseExit() override;
	void OnMouseUp(float x, float y) override;
	void OnMouseDown(float x, float y) override;


private:
	std::unique_ptr<UIRender> m_pBackground;			// 背景表示するUIレンダラー
	std::unique_ptr<UIRender> m_pItemIcon;				// アイテムアイコンを表示するUIレンダラー
	std::unique_ptr<UIText>	m_pText;					// アイテムの数を表示するUIレンダラー

	RECT m_slotRect;
	bool m_isHovered;									// Button hovered state
	bool m_isPressed;									// Button pressed state

	std::function<void()> m_onClick;					// Click event handler
	std::function<void()> m_onHover;					// Hover event handler
	std::function<void()> m_onExit;						// Exit Event handler
};
