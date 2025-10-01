#pragma once
#include "UIButton.h"
#include "UIComponent.h"

class UICraftDetailPanel;

/// @brief レシピの種類を表示するUI層
class UICraftCategoryDetailPanel :
    public UIComponent
{
public:
    typedef std::vector<std::unique_ptr<UIButton>> Slots;
    typedef std::vector<std::unique_ptr<UIText>> SlotsTexts;

    UICraftCategoryDetailPanel();
    ~UICraftCategoryDetailPanel() override = default;

    /// @brief UI初期化
	/// @param fontSet 文字を描画するためのフォントセット 
    /// @param brush 描画に使用するブラシ
    /// @param titleFont タイトル用フォント
    /// @param slotText スロット用テキスト
    /// @param PanelBackground パネル背景
    /// @param slotMaterial スロット用マテリアル
    /// @param effect エフェクト
    /// @param model モデル
    void Init(UIFontSet* fontSet, UIBrush* brush, const char* titleFont, const char* slotText, Material* PanelBackground, Material* slotMaterial, IEffect* effect, Primitive* model);

    UIComponent* HitTest(float x, float y) override;
    void UpdatePanel(int currentIndex);

    void SetActive(bool isActive) override;
	//void SetCategoryName(const std::string& name) { m_currentCategoryName = name; }

    void Draw() override;

    void SetDetailPanel(UICraftDetailPanel* detailPanel) { m_pDetailPanel = detailPanel; }

	/// @brief パネルの位置と最大サイズを初期化する
	///// @param pos Panel pos(left-top anchor)
 //   /// @param panelSize Panel size
    void InitPosAndSize(const DirectX::XMFLOAT3& pos, float panelWidth, const DirectX::XMFLOAT2& titleSize, const DirectX::XMFLOAT2& slotSize);

private:
    std::unique_ptr<UIRender> m_pBackground;        // Panel 背景
    std::unique_ptr<UIText> m_pTitle;               // Panel title

	Slots m_slots;                                 // Slot list
   //SlotsTexts m_slotTexts;                        // Slot text

    std::string m_currentCategoryName;
	std::vector<std::string> m_iconNames;           // icon names for slots

    UICraftDetailPanel* m_pDetailPanel = nullptr;
    //todo: 種類によって複数のslot setにする
    
};

