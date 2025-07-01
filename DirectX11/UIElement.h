#pragma once
#include "GameObject.h"
#include "IEffect.h"
#include "Material.h"
#include "MeshBuffer.h"
#include "UIFontSet.h"
#include "UIScaler.h"

namespace
{
	constexpr D2D1_RECT_F DefaultRectSize = { 0,0,100,100 };//UI Default Size (100x100)
}

/// @brief virtual class for UI Component
class UIComponent
{
public:
	virtual ~UIComponent() = default;
	virtual void Update(float dt) = 0;
	virtual void Draw() {}
	virtual void Draw(const char* text = nullptr) {}
};


/// <summary>
/// UI図形の基底クラス
/// </summary>
class UIMesh
{
public:
	UIMesh();
	void SetViewSize(const DirectX::XMFLOAT2& _viewSize);
	void Draw();

	void SetMaterial(Material* mat) { m_pRenderComponent->SetMaterial(mat); }
	void SetEffect(IEffect* iEffect) { m_pRenderComponent->SetEffect(iEffect); }
	void SetModel(Primitive* p) { m_pRenderComponent->SetModel(p); }

	Transform& GetTransform() { return m_transform; }

protected:

	Transform m_transform;
	std::shared_ptr<RenderComponent> m_pRenderComponent;
};


/// @brief UI描画の基底クラス
class UIElement :
	public UIComponent
{
protected:

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::unique_ptr<UIMesh> m_pUiMesh;					// UIメッシュ

	IDWriteTextFormat* m_pTextFormat;					// 文字表示
	ID2D1SolidColorBrush* m_pSolidBrush;				// 文字色
	ID2D1RenderTarget* m_pd2dRenderTarget = nullptr;	// 描画コマンド

	//Transform m_transform;							// 位置・回転・拡縮
	D2D1_RECT_F m_textRect;								// 文字の描画領域

	UIScaler* m_pUiScaler = nullptr;					// UIスケーリング

public:
	UIElement(ID2D1RenderTarget* renderTarget);
	virtual ~UIElement() = default;

	/// @brief 背景含むメッシュの初期化
	/// @param effect 描画用FX
	/// @param material 背景メッシュ用マテリアル
	/// @param model 背景メッシュ
	/// @param fontSet フォントフォーマット
	/// @param fontName 
	/// @param uiBrush 
	virtual void Init(IEffect* effect, Material* material,Primitive* model,UIFontSet* fontSet, const char* fontName,UIBrush* uiBrush);

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

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetPosition(float x, float y, float z = 0.1f);
	void SetPosition(const float* pos);

	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(float x, float y, float z = 1.0f);
	void SetScale(const float* scale);

	//=====描画
	/// @brief 文字&メッシュ描画
	/// @param text 
	virtual void Draw(const char* text = nullptr);

	/// @brief 文字描画
	/// @param text 
	virtual void DrawTextW(const char* text);

	/// @brief UIメッシュの描画
	virtual void DrawMesh();

	virtual void Update(float dt);

	
protected:
	/// @brief 文字描画位置設定　中心位置を基準に文字の描画位置を設定する
	void AdjustTextRectPos(float x, float y);

	/// @brief 文字描画領域のサイズを設定する　中心位置を基準に文字の描画位置を設定する
	void AdjustTextRectSize(float width, float height);

	void UpdateScale();
};

