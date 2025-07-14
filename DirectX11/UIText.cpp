#include "UIText.h"
namespace
{
	constexpr D2D1_RECT_F DefaultRectSize = { 0,0,100,100 };//UI Default Size (100x100)
	const D2D1::ColorF DefaultColor = {1,1,1,1};//White Color
}

UIText::UIText(ID2D1RenderTarget* renderTarget):
	m_pTextFormat(nullptr),
	m_pSolidBrush(nullptr),
	m_pd2dRenderTarget(renderTarget),
	m_textRect(DefaultRectSize),
	m_color(DefaultColor)//White Color
{
}

void UIText::Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush)
{
	// TextFormatの初期化
	m_pTextFormat = fontSet->GetFont(fontName);

	// Brushの初期化
	m_pSolidBrush = uiBrush->GetSolidBrush();
}

void UIText::SetPosition(const DirectX::XMFLOAT3& pos)
{
	float x, y;
	x = pos.x + WIN_WIDTH / 2;
	y = WIN_HEIGHT / 2 - pos.y;

	//text位置設定
	AdjustTextRectPos(x, y);
}

void UIText::SetScale(const DirectX::XMFLOAT3& scale)
{
	AdjustTextRectSize(scale.x, scale.y);
}

void UIText::Draw()
{
	// 文字が設定されていない場合は描画しない
	if (!m_textProvider && m_staticText.empty())return;
	// 文字が設定されている場合 
	std::string text = m_textProvider ? m_textProvider() : m_staticText; //外部から文字設定する場合はm_textProviderを使用する
	DrawTextW(text);
}

void UIText::SetStaticText(const std::string& text)
{
	m_staticText = text;
	m_textProvider = nullptr;
}

void UIText::SetTextProvider(TextProvider provider)
{
	m_textProvider = provider;
}

void UIText::DrawTextW(const std::string& str)
{
	// check if there is words
	if (m_pTextFormat == nullptr || m_pSolidBrush == nullptr)return;

	// 文字変換 const char->std::wstring
	int strSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wStr(strSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wStr[0], strSize);

	m_pd2dRenderTarget->BeginDraw();

	m_pd2dRenderTarget->DrawTextW(wStr.c_str(), (UINT32)wStr.size(), m_pTextFormat, m_textRect, m_pSolidBrush);
	m_pd2dRenderTarget->EndDraw();
}

void UIText::AdjustTextRectPos(float x, float y)
{
	float width = m_textRect.right - m_textRect.left;
	float height = m_textRect.bottom - m_textRect.top;

	m_textRect.left = x - width / 2.0f; // 中心に配置するために調整
	m_textRect.top = y - height / 2.0f; // 中心に配置するために調整
	m_textRect.right = m_textRect.left + width; // 右端を更新
	m_textRect.bottom = m_textRect.top + height; // 下端を更新
}

void UIText::AdjustTextRectSize(float width, float height)
{
	float midX = (m_textRect.right + m_textRect.left) / 2;
	float midY = (m_textRect.top + m_textRect.bottom) / 2;
	m_textRect.left = midX - width / 2;
	m_textRect.top = midY - height / 2;
	m_textRect.right = m_textRect.left + width; // 右端を更新
	m_textRect.bottom = m_textRect.top + height; // 下端を更新
}

