#pragma once
#include "IEffect.h"

/// 実際の描画
class MonoChrome
{
public:
	MonoChrome();
	~MonoChrome() {};

	void Init(PixelShader* ps);

	void Update(float tick);

	/// @brief Switch render target in order to output screen to monochrome render target
	void PreDraw();
	void DrawRenderTarget();

	void DeActive() { m_isActive = false; }
	void Activate() { m_isActive = true; }

	void SetActive(bool isActive);

private:

	std::shared_ptr<RenderTarget> m_pRTV;
	PixelShader* m_ps;	// Pixel Shader for 
	DirectX::XMFLOAT4 m_colorEffector;// make monochrome changed 

	float m_timer = 0;
	bool m_isActive = false;
};