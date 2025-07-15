#pragma once
#include "UIComponent.h"

/// @brief Ui図形描画
class UIMesh:
	public UIComponent
{
public:
	UIMesh();
	~UIMesh() override = default;

	virtual void Init(IEffect* effect, Material* material, Primitive* model);

	virtual void Draw() override;

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetScale(const DirectX::XMFLOAT3& scale);

private:
	std::unique_ptr<UIRender> m_pRender;
	UIScaler* m_pUiScaler = nullptr;
};

