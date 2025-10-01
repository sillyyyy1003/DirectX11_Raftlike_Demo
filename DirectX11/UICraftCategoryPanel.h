#pragma once
#include "UIComponent.h"
#include "UICraftCategoryDetailPanel.h"
#include "UICraftDetailPanel.h"
#include "UICraftSlot.h"

/// @brief All Category ui panel
class UICraftCategoryPanel :
    public UIComponent
{
public:
    UICraftCategoryPanel();
    ~UICraftCategoryPanel() override;

    /// @brief Init all slots render
	/// @param panelBackgroundMaterial panel background
    /// @param backgroundMaterial slot background
    /// @param iconMaterial slot icon
    /// @param effect effect
    /// @param model primitive model
    void Init(Material* panelBackgroundMaterial, Material* backgroundMaterial, Material* iconMaterial,  IEffect* effect, Primitive* model);

    /// @brief Init panel position & size
    /// @param pos d3d position
    /// @param size slot size
    void InitPosAndSize(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT2& size);
    void LoadSizeAndPos(nlohmann::json& j, const char* uiName);

	void Draw() override;

    /// @brief Hit test of all slots button
    /// @param x 
    /// @param y 
    /// @return if any slot is hit
    UIComponent* HitTest(float x, float y) override;
    void SetPanels(UICraftCategoryDetailPanel* categoryDetailPanel, UICraftDetailPanel* recipeDetailPanel);

private:
   std::unique_ptr<UIRender> m_pBackground;    // Panel background render

    std::vector<std::unique_ptr<UICraftSlot>> m_slots;  // 第一層：レシピの種類を表示
	int m_currentIndex = -1;    // if any slot is clicked, change material color, else make any

	UICraftCategoryDetailPanel* m_categoryPanel;    // 第二層：レシピの種類を表示
    UICraftDetailPanel* m_detailPanel;              // 第三層：レシピの具体内容を表示
};

