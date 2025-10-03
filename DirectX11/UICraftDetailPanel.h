#pragma once
#include "CraftSystem.h"
#include "UIButton.h"
#include "UIComponent.h"

/// @brief 具体的なレシピを表示するUI層:
class UICraftDetailPanel :
    public UIComponent
{

public:
    UICraftDetailPanel();
    ~UICraftDetailPanel() override = default;

    void InitRender(Material* backgroundMaterial, Material* buttonMaterial, Material* iconBackGroundMaterial, Material* iconsMaterial,IEffect* effect, Primitive* primitive);
    void InitSizeAndPos(const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT3& pos);

    /// @brief Init UITexts
    /// @param fontSet 
    /// @param brush 
    /// @param itemNameFont TitleItemFont
    /// @param descriptionFont DescriptionFont
    /// @param requiresFont "Requires"Font
    /// @param ingredientNameFont ingredients name font
    /// @param numberFont number text Font
    void InitFonts(UIFontSet* fontSet, UIBrush* brush, const char* itemNameFont, const char* descriptionFont, const char* requiresFont, const char* ingredientNameFont, const char* numberFont);
    void LoadPanelConfig(nlohmann::json& j, const char* panelName);
    void UpdatePanelInfo(std::string& recipeName);
    void Draw() override;

    //=========Button Event
	UIComponent* HitTest(float x, float y) override;
    
private:

    // Panel anchor pos.
    // All components pos is based on panel position
    // Alignment : leftTop
    DirectX::XMFLOAT3 m_position;    
    DirectX::XMFLOAT2 m_size;                       // Panel size. basic size //background size is m_size+ingredientNum*ingredient height
    int   m_ingredientsSize;                    // max ingredient size
    float m_iconSize;                        // all icon size
    float m_buttonHeight;                    // craft button height
    float m_buttonWidth;                    // craft button width
    float m_horizontalPadding;               // Horizontal distance between uis& edge
    float m_verticalPadding;                 // Vertical distance between uis& edge
    float m_horizontalSpacing;               // Horizontal spacing between ui
    float m_verticalSpacing;                 // Horizontal spacing between ui
    float m_rectWidth;
    float m_titleTextUIHeight;
	float m_descriptionHeight;
    float m_requireTextHeight;
    

    std::unique_ptr<UIRender> m_backgroundRender;   // 背景を表示する
    std::unique_ptr<UIRender> m_craftIcon;          // Icon表示
    std::unique_ptr<UIRender> m_craftIconBackground;// Icon 背景
    Texture* m_pIconTexture = nullptr;

    std::unique_ptr<UIText>   m_itemNameUI;           // 名前文字表示
    std::unique_ptr<UIText>   m_descriptionTextUI;    // 機能文字表示
    std::unique_ptr<UIText>   m_requiresTextUI;       // "Requires"

    std::unique_ptr<UIButton> m_craftButton;          // Button to craft

    struct UIIngredient
    {
        std::unique_ptr<UIRender> ingredientIconUI;
        std::unique_ptr<UIRender> ingredientIconBackgroundUI;
        std::unique_ptr<UIText> ingredientNameUI;         // Ingredient Name
        std::unique_ptr<UIText> requiredNumberTextUI;     // Format: X/M X:count in inventory M:Required count
        Texture* ingredientIconTexture = nullptr;
    };

    std::vector<UIIngredient> m_ingredients;
    int m_currentRecipeIngredientCount = 0;

};

