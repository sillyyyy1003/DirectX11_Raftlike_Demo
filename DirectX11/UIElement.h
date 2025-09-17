#pragma once
#include "UIComponent.h"
#include "ITextBind.h"

/// @brief UI描画の基底クラス
///	Mesh & Textの描画を行う
class UIElement :
	public UIComponent
{
protected:

	std::unique_ptr<UIRender> m_pUiRender;				// UIメッシュ
	std::unique_ptr<UIText> m_pUiText;					// UI文字

	UIScaler* m_pUiScaler = nullptr;					// UIスケーリング

public:
	UIElement();

	~UIElement() override = default;

	/// @brief 背景含むメッシュの初期化
	/// @param effect 描画用FX
	/// @param material 背景メッシュ用マテリアル
	/// @param model 背景メッシュ
	/// @param fontSet フォントフォーマット
	/// @param fontName 
	/// @param uiBrush 
	virtual void Init(IEffect* effect, Material* material, Primitive* model, UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush);

	/// @brief 文字だけの初期化
	/// @param fontSet Font lib
	/// @param fontName	フォント名
	/// @param uiBrush Brush lib
	virtual void Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush);

	/// @brief Meshだけの初期化
	/// @param effect 描画用FX
	/// @param material 背景メッシュ用マテリアル
	/// @param model 背景メッシュ
	virtual void Init(IEffect* effect, Material* material, Primitive* model);

	/// @brief 位置設定
	/// @param pos 背景位置
	/// @param offset 文字ずれ
	void SetPosition(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& offset = { 0,0 });
	void SetPosition(float x, float y, float z = 0.1f);
	void SetPosition(const float* pos);

	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(float x, float y, float z = 1.0f);
	void SetScale(const float* scale);

	//=====描画
	/// @brief 文字&メッシュ描画
	virtual void Draw() override;

	void SetStaticText(const std::string& text);

	void SetTextProvider(ITextBind::TextProvider provider);

	RenderComponent* GetUiRenderComponent() const	{ return m_pUiRender->GetRenderComponent(); }
	UIText* GetUiText() const { return m_pUiText.get(); }

protected:

	void UpdateScale();
};

