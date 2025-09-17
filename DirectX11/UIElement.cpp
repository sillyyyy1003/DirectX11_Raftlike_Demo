#include "UIElement.h"
#include "UIBasicEffect.h"

//================UIElement
UIElement::UIElement():
m_pUiRender(nullptr)
{
	m_pUiText = std::make_unique<UIText>();
}

void UIElement::Init(IEffect* effect, Material* material, Primitive* model, UIFontSet* fontSet, const char* fontName,UIBrush* uiBrush)
{

	// UIMeshの初期化
	Init(effect, material, model);

	// UiTextの初期化
	Init(fontSet,fontName,uiBrush);
}


void UIElement::Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush)
{
	m_pUiText->Init(fontSet, fontName, uiBrush);
}

void UIElement::Init(IEffect* effect, Material* material, Primitive* model)
{
	// UIMeshの初期化
	m_pUiRender = std::make_unique<UIRender>();

	m_pUiRender->SetEffect(effect);
	m_pUiRender->SetMaterial(material);
	m_pUiRender->SetModel(model);
}


void UIElement::Draw()
{
	if (!m_isActive)return;
	//===========背景描画
	m_pUiRender->Draw();

	//===========文字描画
	m_pUiText->Draw();
}


void UIElement::SetStaticText(const std::string& text)
{
	m_pUiText->SetStaticText(text);
}

void UIElement::SetTextProvider(ITextBind::TextProvider provider)
{
	m_pUiText->SetTextProvider(provider);
}

void UIElement::UpdateScale()
{
	m_pUiRender->SetViewSize(m_pUiScaler->GetScale());
}


void UIElement::SetPosition(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& offset)
{
	//mesh位置設定
	m_pUiRender->GetTransform().SetPosition(pos);

	//text位置設定
	DirectX::XMFLOAT3 textPos = {
		pos.x + offset.x,
		pos.y + offset.y,
		pos.z 
	};
	m_pUiText->SetPosition(textPos);
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
	//Mesh Size 設定
	m_pUiRender->GetTransform().SetScale(scale);

	//Text Rect Size設定
	m_pUiText->SetScale(scale);
}

void UIElement::SetScale(float x, float y, float z)
{
	SetScale(DirectX::XMFLOAT3(x, y, z));
}

void UIElement::SetScale(const float* scale)
{
	SetScale(DirectX::XMFLOAT3(scale[0], scale[1], scale[2]));
}
