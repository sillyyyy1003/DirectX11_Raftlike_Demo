#include "UIElement.h"

#include "RenderComponent.h"
#include "UIBasicEffect.h"

UIMesh::UIMesh()
{
	m_pRenderComponent = std::make_unique<UIRenderComponent>();
}

void UIMesh::SetViewSize(const DirectX::XMFLOAT2& _viewSize)
{
	dynamic_cast<UIBasicEffect*>(m_pRenderComponent->GetEffect())->SetViewSize(_viewSize);
}

void UIMesh::Draw()
{
	assert(m_pRenderComponent != nullptr);
	m_pRenderComponent->Render(m_transform);
}

UIElement::UIElement(ID2D1RenderTarget* renderTarget):
m_pUiMesh(nullptr),
m_pTextFormat(nullptr),
m_pSolidBrush(nullptr),
m_pd2dRenderTarget(renderTarget),
m_textRect(::DefaultRectSize)
{
}


void UIElement::Init(IEffect* effect, Material* material, Primitive* model, UIFontSet* fontSet, const char* fontName,UIBrush* uiBrush)
{

	// UIMeshの初期化
	m_pUiMesh = std::make_unique<UIMesh>();

	m_pUiMesh->SetEffect(effect);
	m_pUiMesh->SetMaterial(material);
	m_pUiMesh->SetModel(model);

	// TextFormatの初期化
	m_pTextFormat = fontSet->GetFont(fontName);

	// Brushの初期化
	m_pSolidBrush = uiBrush->GetSolidBrush();
}


void UIElement::Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush)
{
	// TextFormatの初期化
	m_pTextFormat = fontSet->GetFont(fontName);

	// Brushの初期化
	m_pSolidBrush = uiBrush->GetSolidBrush();
}

void UIElement::Init(IEffect* effect, Material* material, Primitive* model)
{
	// UIMeshの初期化
	m_pUiMesh = std::make_unique<UIMesh>();

	m_pUiMesh->SetEffect(effect);
	m_pUiMesh->SetMaterial(material);
	m_pUiMesh->SetModel(model);
}


void UIElement::DrawUi(const char* text)
{
	// 背景描画
	DrawMesh();

	// 文字描画
	DrawTextW(text);
}

void UIElement::DrawTextW(const char* text)
{
	if (m_pTextFormat == nullptr || m_pSolidBrush == nullptr)return;

	// 文字変換 const char->std::wstring
	std::string str(text);
	int strSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wStr(strSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wStr[0], strSize);

	m_pd2dRenderTarget->BeginDraw();

	m_pd2dRenderTarget->DrawTextW(wStr.c_str(), (UINT32)wStr.size(), m_pTextFormat, m_textRect, m_pSolidBrush);
	m_pd2dRenderTarget->EndDraw();
}

void UIElement::DrawMesh()
{
	if (m_pUiMesh == nullptr)return;
	m_pUiMesh->Draw();
}

void UIElement::Update(float dt)
{
}

void UIElement::AdjustTextRectPos(float x, float y)
{
	float width = m_textRect.right - m_textRect.left;
	float height = m_textRect.bottom - m_textRect.top;

	m_textRect.left = x - width / 2.0f; // 中心に配置するために調整
	m_textRect.top = y - height / 2.0f; // 中心に配置するために調整
	m_textRect.right = m_textRect.left + width; // 右端を更新
	m_textRect.bottom = m_textRect.top + height; // 下端を更新
}

void UIElement::AdjustTextRectSize(float width, float height)
{
	float midX = (m_textRect.right + m_textRect.left) / 2;
	float midY = (m_textRect.top + m_textRect.bottom) / 2;
	m_textRect.left = midX - width / 2;
	m_textRect.top = midY - height / 2;
	m_textRect.right = m_textRect.left + width; // 右端を更新
	m_textRect.bottom = m_textRect.top + height; // 下端を更新
}

void UIElement::UpdateScale()
{
	m_pUiMesh->SetViewSize(m_pUiScaler->GetScale());
}

void UIElement::SetPosition(const DirectX::XMFLOAT3& pos)
{
	//m_transform.SetPosition(pos);
	//mesh位置設定
	m_pUiMesh->GetTransform().SetPosition(pos);

	//Convert D3D to Screen
	float x, y;
	x = pos.x + WIN_WIDTH / 2;
	y = WIN_HEIGHT / 2 - pos.y;

	//text位置設定
	AdjustTextRectPos(x, y);

}

void UIElement::SetPosition(float x, float y, float z)
{
	SetPosition(DirectX::XMFLOAT3(x, y, z)); 
}

void UIElement::SetPosition(const float* pos)
{
	SetPosition(DirectX::XMFLOAT3(pos[0], pos[1], pos[2]));
}

void UIElement::SetScale(const DirectX::XMFLOAT3& scale)
{
	//m_transform.SetScale(scale);
	//Mesh Size 設定
	m_pUiMesh->GetTransform().SetScale(scale);

	//Text Rect Size設定
	AdjustTextRectSize(scale.x, scale.y);

}

void UIElement::SetScale(float x, float y, float z)
{
	SetScale(DirectX::XMFLOAT3(x, y, z));
}

void UIElement::SetScale(const float* scale)
{
	SetScale(DirectX::XMFLOAT3(scale[0], scale[1], scale[2]));
}
