#include <nlohmann/json.hpp>
#include "UIFontSet.h"
#include "DebugLog.h"
#include "DXTrace.h"

void UIFontSet::InitFontList()
{
	//For Title
	Font titleFont = { "TitleFont", L"Arial",90,true };
	Font optionFont = { "OptionFont",L"Arial", 35,true };
	Font messageFont = { "MessageFont",L"Arial",25,false };

	m_fontList["TitleFont"] = titleFont;
	m_fontList["OptionFont"] = optionFont;
	m_fontList["MessageFont"] = messageFont;
}

void UIFontSet::LoadFontList(const char* fileName)
{
	std::ifstream inFile(fileName);
	if (!inFile)
	{
		throw std::runtime_error(std::string("Cannot open font json file: ") + fileName);
	}

	nlohmann::json j;
	inFile >> j;

	if (!j.contains("Fonts") || !j["Fonts"].is_array())
	{
		throw std::runtime_error("Invalid or missing 'Fonts' array in JSON");
	}

	for (const auto& item : j["Fonts"])
	{
		std::string name = item.value("Name", "Unnamed");	  // Load Name
		std::string family = item.value("FontFamily", "Arial"); // Load FontFamily, Default>> Arial
		std::wstring ws(family.begin(), family.end());// Transfer string to wstring
		float size = item.value("FontSize", 12.0f);	// Load font size
		bool isCentered = item.value("IsCentered", false);	 // Load text format

		m_fontList[name] = Font(name, ws, size, isCentered);
	}
}


void UIFontSet::CreateTextFormat(ID2D1RenderTarget* d2dRenderTarget, IDWriteFactory* writeFactory)
{
	for (auto& font : m_fontList)
	{
		//todo:Font Family Arial
		HR(writeFactory->CreateTextFormat(
			font.second.FontFamily.c_str(),
			nullptr,        // Font collection (nullptr for system default)
			DWRITE_FONT_WEIGHT_NORMAL,	// Font weight (e.g., Normal, Bold)
			DWRITE_FONT_STYLE_NORMAL,   // Font style (e.g., Normal, Italic)
			DWRITE_FONT_STRETCH_EXPANDED, // Font stretch (e.g., Normal, Condensed)
			font.second.FontSize, // Font size
			L"ja-JP",
			font.second.FontFormat.GetAddressOf() // Output text format
		));

		//文字揃いの設定
		if (font.second.isCentered)// 中央寄せ
			font.second.FontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		else//左寄せ
			font.second.FontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}
}

IDWriteTextFormat* UIFontSet::GetFont(const char* fontName)
{
#if defined(_DEBUG) || defined(DEBUG)
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
