#pragma once
#include "UIElement.h"

class UIText :
	public UIComponent, public ITextBind
{
	UIText(ID2D1RenderTarget* renderTarget);
	~UIText() override = default;

	void Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush);

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetScale(const DirectX::XMFLOAT3& scale);

	void Draw() override;

	void SetStaticText(const std::string& text) override;

	void SetTextProvider(TextProvider provider) override;
protected:
	void DrawTextW(const std::string& str);
	/// @brief 文字描画位置設定　
	/// 中心位置を基準に文字の描画位置を設定する
	void AdjustTextRectPos(float x, float y);

	/// @brief 文字描画領域のサイズを設定する　
	/// 中心位置を基準に文字の描画位置を設定する
	void AdjustTextRectSize(float width, float height);



private:
	IDWriteTextFormat* m_pTextFormat;					// 文字表示
	ID2D1SolidColorBrush* m_pSolidBrush;				// Font Brush			
	ID2D1RenderTarget* m_pd2dRenderTarget = nullptr;	// 描画コマンド

	D2D1_RECT_F m_textRect;								// 文字の描画領域
	UIScaler* m_pUiScaler = nullptr;					// UIスケーリング

	std::string m_staticText;
	TextProvider m_textProvider;

	D2D1_COLOR_F m_color;								// 文字色
};

