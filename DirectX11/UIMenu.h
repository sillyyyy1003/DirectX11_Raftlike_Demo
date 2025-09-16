#pragma once
#include "UIBar.h"
#include "UIBasicEffect.h"
#include "UIButtonMove.h"
#include "UIComponent.h"
class SceneBase;

class UIMenu :
    public UIComponent
{
public:

	struct MenuResource
	{
		UIBasicEffect* effect;
		UIFontSet* fontSet;
		UIBrush* brush;

		Material* menuMat;
		Material* barMat;
		Material* barBgMat;
		Material* bgmLeftArrowMat;
		Material* bgmRightArrowMat;
		Material* seLeftArrowMat;
		Material* seRightArrowMat;
		Material* environmentLeftArrowMat;
		Material* environmentRightArrowMat;
		Material* resumeMat;
		Material* backToTitleMat;

		Primitive* square;
	};


	UIMenu();
	~UIMenu() override = default;

	void Init(MenuResource& resource);
	void SetButton(SceneBase* scene);

	void Update(float dt) override;

	void Draw() override;

	/// @brief メニューの有効/無効を設定する
	void SetActive(bool isActive) override;

	/// @brief メニューが開いているか？
	bool IsOpen() const { return m_isActive; }

	UIButtonMove* GetResumeButton() { return m_resumeButton.get(); }

private:

	// UI
	std::unique_ptr<UIRender> m_menuBg;
	std::unique_ptr<UIText> m_bgmText;
	std::unique_ptr<UIText> m_seText;
	std::unique_ptr<UIText> m_environmentText;

	std::unique_ptr<UIBar> m_bgmBar;
	std::unique_ptr<UIBar> m_seBar;
	std::unique_ptr<UIBar> m_environmentBar;

	std::unique_ptr<UIButtonMove> m_bgmLeftArrow;
	std::unique_ptr<UIButtonMove> m_bgmRightArrow;
	std::unique_ptr<UIButtonMove> m_seLeftArrow;
	std::unique_ptr<UIButtonMove> m_seRightArrow;
	std::unique_ptr<UIButtonMove> m_environmentLeftArrow;
	std::unique_ptr<UIButtonMove> m_environmentRightArrow;
	std::unique_ptr<UIButtonMove> m_backToTitleButton;
	std::unique_ptr<UIButtonMove> m_resumeButton;




};

