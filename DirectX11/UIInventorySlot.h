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

	void SetPosition(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& textOffset={0,0});

	/// @brief Set click function callback
	/// @param callback 
	void SetOnClick(const std::function<void()>& callback) { m_onClick = callback; }
	void OnClick(float x, float y) override;
private:
	std::unique_ptr<UIRender> m_pBackground;			// 背景表示するUIレンダラー
	std::unique_ptr<UIRender> m_pItemIcon;				// アイテムアイコンを表示するUIレンダラー
	std::unique_ptr<UIText>	m_pText;					// アイテムの数を表示するUIレンダラー
	std::function<void()> m_onClick;					// Click event handler
    
};

class UIInventory :
	public UIComponent
{
public:
	UIInventory() = default;
	~UIInventory() override = default;

	void Update(float dt) override;

	/// @brief　Initialize the inventory UI with given parameters
	/// @param inventory 表示するインベントリ
	/// @param effect Shader effect for rendering
	/// @param bgMat 背景Material
	/// @param slotBgMat Slot背景Material
	/// @param iconMat Icon Material
	/// @param chosenSlotMat 
	/// @param model Square Primitive for background and slots
	/// @param fontSet Item countフォントセット
	/// @param fontName Item count
	/// @param brush Item count text brush
	void Init(Inventory* inventory, IEffect* effect, Material* bgMat, Material* slotBgMat, Material* iconMat, Material* chosenSlotMat, Primitive* model, UIFontSet* fontSet, const char* fontName, UIBrush* brush);

	/// @brief Set inventory ui position & size
	/// @param size inventory slot size
	/// @param pos inventory position
	void InitSizeAndPos(const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT3& pos);

	/// @brief Set inventory ui position & size from config file
	/// @param filePath 
	void LoadSizeAndPos(const char* filePath);

	/// @brief Set inventory ui position & size from config file
	/// @param filePath 
	void LoadSizeAndPos(nlohmann::json& j, const char* uiName);

	void SetInventory(Inventory* inventory);

	/// @brief Set player& Update item of player 
	/// @param player 
	void SetPlayer(Player* player);

	void Draw() override;
private:
	std::vector<std::unique_ptr<UIInventorySlot>> m_slots;	// スロットのリスト
	std::unique_ptr<UIRender> m_pBackground;	// インベントリの背景
	std::unique_ptr<UIRender> m_pChosenSlotFrame;

	Inventory* m_pInventory;	// 所持アイテムのインベントリ
	Player* m_pPlayer;

	int m_currentIndex = 0;
};