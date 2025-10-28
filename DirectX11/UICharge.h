#pragma once
#include "UIComponent.h"


class UICharge :public UIComponent
{
public:
	void Init(Material* mat, IEffect* effect, Primitive* model);
	void SetScale(DirectX::XMFLOAT2 scale);
	void SetPosition(DirectX::XMFLOAT3 pos);
	
	void SetProgress(float progress);

	void Draw() override;
private:
	std::unique_ptr<UIRender> m_pUiRender;				// UIメッシュ

};

