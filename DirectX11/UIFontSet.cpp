#include "UIFontSet.h"

#include "DebugLog.h"
#include "DXTrace.h"

void UIFontSet::InitFontList()
{
	Font titleFont = { "TitleFont", 60 };
	Font optionFont = { "OptionFont", 35 };

	m_fontList["TitleFont"] = titleFont;
	m_fontList["OptionFont"] = optionFont;


}


void UIFontSet::CreateTextFormat(ID2D1RenderTarget* d2dRenderTarget, IDWriteFactory* writeFactory)
{
	for (auto& font : m_fontList)
	{
		//todo:Font Family Arial
		HR(writeFactory->CreateTextFormat(
			L"Arial",
			nullptr,        // Font collection (nullptr for system default)
			DWRITE_FONT_WEIGHT_NORMAL,	// Font weight (e.g., Normal, Bold)
			DWRITE_FONT_STYLE_NORMAL,   // Font style (e.g., Normal, Italic)
			DWRITE_FONT_STRETCH_EXPANDED, // Font stretch (e.g., Normal, Condensed)
			font.second.FontSize, // Font size
			L"ja-JP",
			font.second.FontFormat.GetAddressOf() // Output text format
		));
	}
}

IDWriteTextFormat* UIFontSet::GetFont(const char* fontName)
{
#ifdef _DEBUG
	// Check if the font exists in the list
	if (m_fontList.find(fontName) != m_fontList.end())
	{
		return m_fontList[fontName].FontFormat.Get();
	}
	else
	{
		DebugLog::LogError("Font doesn't exit!!");
		return nullptr;
	}

#endif

	return m_fontList[fontName].FontFormat.Get();
}


UIBrush::UIBrush():
m_pSolidBrush(nullptr),
m_pRadialGradientBrush(nullptr),
m_pLinearGradientBrush(nullptr)
{
}

void UIBrush::InitBrush(ID2D1RenderTarget* d2dRenderTarget)
{
	//Init Solid color brush
	d2dRenderTarget->CreateSolidColorBrush(m_frontColor, m_pSolidBrush.GetAddressOf());

}

void UIBrush::SetFrontColor(D2D1::ColorF frontColor)
{
	m_pSolidBrush->SetColor(frontColor);
}
