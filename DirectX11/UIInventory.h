#pragma once
#include "UIComponent.h"
#include "UIInventorySlot.h"

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

	UIComponent* HitTest(float x, float y) override;
	void OnMouseEnter() override;
	void OnMouseExit() override;

private:
	std::vector<std::unique_ptr<UIInventorySlot>> m_slots;	// スロットのリスト
	std::unique_ptr<UIRender> m_pBackground;	// インベントリの背景
	std::unique_ptr<UIRender> m_pChosenSlotFrame;

	UIInventorySlot* m_pressedSlot = nullptr;
	UIInventorySlot* m_hoverSlot = nullptr;

	Inventory* m_pInventory;	// 所持アイテムのインベントリ
	Player* m_pPlayer;

	int m_currentIndex = 0;

};
