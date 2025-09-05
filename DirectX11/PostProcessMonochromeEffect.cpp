#include "PostProcessMonochromeEffect.h"
#include <memory>

#include "d3dUtil.h"
#include "Sprite.h"

namespace
{
	static constexpr DirectX::XMFLOAT4 MonoColor = { 0.3f,0.6f,0.1f,0.0f};//To control the color fade into monochrome using w(0~1)
	static constexpr float TimeLimit = 2.f;
	static constexpr float Saturation = 0.5f;	// final screen color saturation compared with original color
}

MonoChrome::MonoChrome():
	m_pRTV(nullptr),
	m_ps(nullptr),
	m_colorEffector(MonoColor)
{
}

void MonoChrome::Init(PixelShader* ps)
{
	m_pRTV = std::make_shared<RenderTarget>();
	m_pRTV->Create(DXGI_FORMAT_B8G8R8A8_UNORM,
		gD3D->GetClientWidth(),gD3D->GetClientHeight());

	m_ps = ps;
}

void MonoChrome::Update(float tick)
{
	if (!m_isActive)return;

	// Only execute in 2 sec
	if (m_timer < TimeLimit)
	{
		m_timer += tick;
		m_colorEffector.w = Lerp(0, Saturation, m_timer / TimeLimit);
	}

}

void MonoChrome::PreDraw()
{
	if (!m_isActive)return;

	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pRTV->Clear(color);	//Clear color

	gD3D->SetRenderTargets(1, &m_pRTV, gD3D->GetDefDepthStencil()); // Switch to monochrome render target
}

void MonoChrome::DrawRenderTarget()
{
	if (!m_isActive)return;
	// switch to default render target
	gD3D->SetDefRenderTarget();
	Sprite::SetTexture(m_pRTV.get());
	// 3Dの表示から2Dの表示へ切り替え
	DirectX::XMFLOAT4X4 mat[3];
	DirectX::XMStoreFloat4x4(&mat[0],
		DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&mat[1],
		DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&mat[2],
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixOrthographicLH(1, 1, 0.0f, 1.0f)
		));
	Sprite::SetWorld(mat[0]);
	Sprite::SetView(mat[1]);
	Sprite::SetProjection(mat[2]);
	Sprite::SetSize(DirectX::XMFLOAT2(1.0f, 1.0f));

	Sprite::SetPixelShader(m_ps);

	m_ps->WriteBuffer(0, &m_colorEffector);
	Sprite::Draw();

	// Switch back to default render target
	gD3D->SetDefRenderTarget();

}

void MonoChrome::SetActive(bool isActive)
{
	m_isActive = isActive;
	if (!m_isActive)return;
	m_timer = 0;
	m_colorEffector.w = 0;
}
