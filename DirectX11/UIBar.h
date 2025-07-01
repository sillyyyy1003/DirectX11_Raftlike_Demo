#pragma once
#include "UIElement.h"

/// @brief UI Bar Type Render Component
///	include two mesh: background & bar
///	Base: Component
///	todo: 之后要考虑屏幕大小变化的时候所有的大小需要适配另一个函数
class UIBar :public Component
{

public:
	UIBar();
	~UIBar() override = default;

	/// @brief UIの位置とサイズを設定する
	/// @param pos 位置
	/// @param scale サイズ
	void Init(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale);

	/// @brief Update bar render status
	/// @param volume current percentage (0~1)
	void UpdateUI(float volume);



private:
	std::unique_ptr<UIMesh> m_backgroundMesh;
	std::shared_ptr<UIMesh> m_barMesh;

	DirectX::XMFLOAT3 m_position;		// UI position
	DirectX::XMFLOAT3 m_scale;			// UI scale (width & height)


};

