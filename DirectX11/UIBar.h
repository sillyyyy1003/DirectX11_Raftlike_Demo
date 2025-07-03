#pragma once
#include "UIElement.h"

/// @brief UI Bar Type Render Component
///	include two mesh: background & bar
///	Base: Component
///	todo: 之后要考虑屏幕大小变化的时候所有的大小需要适配另一个函数
class UIBar :public UIComponent
{

public:
	UIBar();
	~UIBar() override = default;

	/// @brief UIBar Init
	/// @param pos 
	/// @param scale 
	/// @param bgMaterial 
	/// @param barMaterial 
	/// @param bgEffect 
	/// @param barEffect 
	void Init(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& scale, Material* bgMaterial, Material* barMaterial,IEffect* bgEffect, IEffect* barEffect);


	/// @brief Update bar render status
	/// @param volume current percentage (0~1)
	void UpdateUI(float volume);

	void Draw(const char* string = nullptr) override;

	void Update(float dt) override;

private:
	std::unique_ptr<UIMesh> m_backgroundMesh;
	std::unique_ptr<UIMesh> m_barMesh;

	DirectX::XMFLOAT3 m_position;		// UI position AnchorPos:LeftMiddle
	DirectX::XMFLOAT3 m_scale;			// UI scale (width & height)


};

