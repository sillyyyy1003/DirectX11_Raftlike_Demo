#pragma once
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "D3DApp.h"

/// @brief Fontを管理するクラス
class UIFontSet
{
protected:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct Font
	{
		std::string FontName;	// 用途名
		std::wstring FontFamily;// フォント
		float FontSize;			// フォントのサイズ
		ComPtr<IDWriteTextFormat> FontFormat;

		Font(const std::string& name, const std::wstring& font, float size, bool isCenter) : FontName(name),FontFamily(font), FontSize(size),FontFormat(nullptr) {}
		Font() : FontName("Default"), FontFamily(L"Arial"),FontSize(1.f), FontFormat(nullptr) {}
	};

	typedef std::unordered_map<std::string, Font> FontList;
	FontList m_fontList;	//すべてのFontを保存する



public:

	UIFontSet() = default;
	~UIFontSet() = default;

	/// @brief Fontデータの初期化
	///	todo: jsonファイルからの読み込みに変更する
	void InitFontList();

	/// @brief json fileから設定を読み込み
	/// @param fileName 
	bool LoadFontList(const char* fileName);

	/// @brief FontFormatを作成する
	void CreateTextFormat(ID2D1RenderTarget* d2dRenderTarget, IDWriteFactory* writeFactory);

	/// @brief TextFormatを取得する
	/// @param fontName 
	/// @return 
	IDWriteTextFormat* GetTextFormat(const char* fontName);
};


class UIBrush
{
protected:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID2D1SolidColorBrush> m_pSolidBrush;		//Mono tone
	ComPtr<ID2D1RadialGradientBrush>  m_pRadialGradientBrush;		//Radian
	ComPtr<ID2D1LinearGradientBrush>  m_pLinearGradientBrush;		//Linear

	D2D1::ColorF m_frontColor = D2D1::ColorF::White;		//Front Color
	D2D1::ColorF m_backColor = D2D1::ColorF::Black;			//Back Color


public:

	UIBrush();
	~UIBrush() = default;

	void InitBrush(ID2D1RenderTarget* d2dRenderTarget);

	ID2D1SolidColorBrush* GetSolidBrush() { return m_pSolidBrush.Get(); };

	/// @brief フロントカラー変更
	/// @param frontColor 
	void SetFrontColor(D2D1::ColorF frontColor);

};