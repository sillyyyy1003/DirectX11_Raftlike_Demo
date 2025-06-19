#pragma once
#include <DirectXMath.h>
#include "Cube.h"

/// <summary>
/// ウィンドウサイズ調整用UIのスケールを調整するクラス
/// </summary>
class UIScaler
{
private:
	bool m_isDirty = false;								//Uiスケールが変更されたかどうか

	DirectX::XMFLOAT2 m_scale;		//UIスケール
	DirectX::XMFLOAT2 m_referenceWindowSize = {};			//ウィンドウサイズ
	DirectX::XMFLOAT2 m_currentWindowSize = {};			//現在のウィンドウサイズ

public:
	UIScaler();
	~UIScaler() = default;

	void UpdateScaler();
	const DirectX::XMFLOAT2& GetScale() const { return m_scale; }	//UIスケールを取得
	const DirectX::XMFLOAT2& GetReferenceWindowSize() const { return m_referenceWindowSize; }	//参照ウィンドウサイズを取得
	const DirectX::XMFLOAT2 GetCurrentWindowSize() const { return m_currentWindowSize; };

	void SetWindowSize(const DirectX::XMFLOAT2& windowSize);

};


