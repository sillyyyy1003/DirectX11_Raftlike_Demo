#pragma once
#include "IEffect.h"
#include "Material.h"
#include "UIFontSet.h"
#include "UIScaler.h"
#include "ITextBind.h"
#include "RenderComponent.h"

/// @brief virtual class for UI Component
class UIComponent
{
public:
	virtual ~UIComponent() = default;
	virtual void Update(float dt)
	{
		// 全てのUIコンポーネントでUpdateを実装する必要はないが、
		// Updateを呼び出すために仮想関数として定義しておく
	};	

	/// @brief 描画 
	virtual void Draw() = 0;

	/// @brief UiComponentの有効/無効を設定する
	virtual void SetActive(bool isActive) { m_isActive = isActive; }
	virtual bool IsActive() const { return m_isActive; }

	//============== UIイベント処理 ===================
	virtual UIComponent* HitTest(float x, float y) { return nullptr; };
	virtual void OnMouseEnter() {}
	virtual void OnMouseExit() {}
	virtual void OnMouseDown(float x, float y) {}
	virtual void OnMouseUp(float x, float y) {}
	virtual void OnClick(float x, float y) {}

protected:
	bool m_isActive = true;		// True>>作動
};


/// <summary>
/// UI図形の基底クラス
/// </summary>
class UIRender:
	public UIComponent
{
public:
	UIRender();
	void SetViewSize(const DirectX::XMFLOAT2& _viewSize);
	void Draw() override;

	void Init(Material* mat, IEffect* effect, Primitive* model);
	void SetMaterial(Material* mat) { m_pRenderComponent->SetMaterial(mat); }
	void SetEffect(IEffect* iEffect) { m_pRenderComponent->SetEffect(iEffect); }
	void SetModel(Primitive* p) { m_pRenderComponent->SetModel(p); }

	Transform& GetTransform() { return m_transform; }
	RenderComponent* GetRenderComponent() { return m_pRenderComponent.get(); }

protected:

	Transform m_transform;
	std::shared_ptr<RenderComponent> m_pRenderComponent;

};


/// @brief UI文字描画
class UIText :
	public UIComponent, public ITextBind
{
public:
	enum class TextAlign : uint8_t
	{
		Left,
		Center,
		Right
	};

	enum class ParagraphAlign : uint8_t
	{
		Top,
		Middle,
		Bottom
	};

	UIText();
	~UIText() override = default;

	void Init(UIFontSet* fontSet, const char* fontName, UIBrush* uiBrush);

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetScale(const DirectX::XMFLOAT3& scale);

	void Draw() override;

	void SetStaticText(const std::string& text) override;
	void SetTextProvider(TextProvider provider) override;

	/// @brief 文字揃い設定(水平方向) default:left
	///	@param textAlign Left: 左揃え, Center: 中央揃え, Right: 右揃え
	void SetTextAlignment(TextAlign textAlign);
	/// @brief 文字揃い設定(垂直方向) default:top
	///	@param paragraphAlign Top: 上揃え, Middle: 中央揃え, Bottom: 下揃
	void SetParagraphAlignment(ParagraphAlign paragraphAlign);

	void SetTextColor(D2D1_COLOR_F color);

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
	//ID2D1RenderTarget* m_pd2dRenderTarget = nullptr;	// 描画コマンド
	D2D1_RECT_F m_textRect;								// 文字の描画領域
	std::string m_staticText;
	TextProvider m_textProvider;

	D2D1_COLOR_F m_color;								// 文字色
	
};

