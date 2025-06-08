#pragma once
#include <unordered_map>

#include "D3DApp.h"

class UIFontSet
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct Font
	{
		std::string FontName;	//用途名
		float FontSize;			//フォントのサイズ
		ComPtr<IDWriteTextFormat> FontFormat;
	};

	typedef std::unordered_map<std::string, Font> FontList;

	FontList m_fontList;	//すべてのFontを保存する



};

