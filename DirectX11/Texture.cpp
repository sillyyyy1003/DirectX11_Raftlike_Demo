#include "Texture.h"
#include "D3DApp.h"
#include "d3dUtil.h"
#include "DirectXTex.h"
#include "DXTrace.h"
#include "d3dUtil.h"
#include "DebugLog.h"

/// <summary>
/// テクスチャ
/// </summary>
Texture::Texture()
	: m_width(0), m_height(0)
	, m_pTex(nullptr)
	, m_pSRV(nullptr)
{
}
Texture::~Texture()
{
	
}
HRESULT Texture::Create(const char* fileName)
{
	HRESULT hr = S_OK;

	// 文字変換
	wchar_t wPath[MAX_PATH];
	size_t wLen = 0;
	MultiByteToWideChar(0, 0, fileName, -1, wPath, MAX_PATH);

	// ファイル別読み込み
	DirectX::TexMetadata mdata;
	DirectX::ScratchImage image;
	if (strstr(fileName, ".tga"))			// Load from tga file
		hr = DirectX::LoadFromTGAFile(wPath, &mdata, image);
	else if (strstr(fileName, ".dds"))	// Load from dds file
		hr = DirectX::LoadFromDDSFile(wPath, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &mdata, image);
	else //Load from others
		hr = DirectX::LoadFromWICFile(wPath, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &mdata, image);


	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// シェーダリソース生成
	hr = CreateShaderResourceView(gD3D->GetDevice(), image.GetImages(), image.GetImageCount(), mdata, &m_pSRV);
	if (SUCCEEDED(hr))
	{
		m_width = (UINT)mdata.width;
		m_height = (UINT)mdata.height;
		//
		ID3D11Resource* resource = nullptr;
		m_pSRV->GetResource(&resource);
		if (resource)
		{
			resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_pTex.ReleaseAndGetAddressOf()));
			resource->Release();
		}
	}
	return hr;
}
HRESULT Texture::Create(DXGI_FORMAT format, UINT width, UINT height, const void* pData)
{
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(format, width, height);
	return CreateResource(desc, pData);
}

UINT Texture::GetWidth() const
{
	return m_width;
}
UINT Texture::GetHeight() const
{
	return m_height;
}
ID3D11ShaderResourceView* Texture::GetResource() const
{
	return m_pSRV.Get();
}

ID3D11Texture2D* Texture::GetTexture() const
{
	return m_pTex.Get();
}

D3D11_TEXTURE2D_DESC Texture::MakeTexDesc(DXGI_FORMAT format, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	if (gD3D->GetEnable4xMsaa())
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = gD3D->Get4xMsaaQuality() - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}
	return desc;
}
HRESULT Texture::CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData)
{
	HRESULT hr = E_FAIL;

	// テクスチャ作成
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = pData;
	data.SysMemPitch = desc.Width * 4;
	hr = gD3D->GetDevice()->CreateTexture2D(&desc, pData ? &data : nullptr, &m_pTex);
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	switch (desc.Format)
	{
	default:						srvDesc.Format = desc.Format;			break;
	case DXGI_FORMAT_R32_TYPELESS: 	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;	break;
	}
	
	if (desc.SampleDesc.Count > 1)
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;   // MSAA
	else
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;     // 普通 2D

	srvDesc.Texture2D.MipLevels = 1;

	srvDesc.Texture2D.MipLevels = 1;
	// 生成
	hr = gD3D->GetDevice()->CreateShaderResourceView(m_pTex.Get(), &srvDesc, m_pSRV.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		m_width = desc.Width;
		m_height = desc.Height;
	}
	return hr;
}

TextureCube::TextureCube()
{
}

void TextureCube::CreateTextureCube(UINT width, UINT height, DXGI_FORMAT format, UINT mipLevels)
{
	//=======Create texture cube========
	ID3D11Device* device = gD3D->GetDevice();
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = 6;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr;
	hr = device->CreateTexture2D(&texDesc, nullptr, m_pTex.GetAddressOf());
	if (FAILED(hr))DebugLog::LogError("Failed to create texture cube");

	//=======Create shader resource view========
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = mipLevels;

	hr = device->CreateShaderResourceView(m_pTex.Get(), &srvDesc, m_pSRV.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create TextureCube SRV");

}

void TextureCube::CopyTextures(const std::vector<Texture*>& textures)
{
	ID3D11DeviceContext* context = gD3D->GetContext();

	//Error check texture number
	if (textures.size() != 6)
	{
		DebugLog::LogError("TextureCube requires exactly 6 textures for each face.");
		return;
	}

	for (UINT i = 0; i < textures.size(); ++i)
	{
		// Check if the texture is valid
		if (textures[i]->GetTexture() == nullptr) continue;

		// Copy each texture to the corresponding face of the cube
		context->CopySubresourceRegion(
			m_pTex.Get(),
			D3D11CalcSubresource(0, i, 1),
			0, 0, 0,
			textures[i]->GetTexture(),
			0, nullptr);
	}
}


/// <summary>
/// レンダーターゲット
/// </summary>
RenderTarget::RenderTarget()
	: m_pRTV(nullptr)
{
}
RenderTarget::~RenderTarget()
{
	
}
void RenderTarget::Clear()
{
	static float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Clear(color);
}
void RenderTarget::Clear(const float* color)
{
	gD3D->GetContext()->ClearRenderTargetView(m_pRTV.Get(), color);
}

HRESULT RenderTarget::Create(DXGI_FORMAT format, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(format, width, height);
	desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	return CreateResource(desc);
}

HRESULT RenderTarget::CreateFromScreen()
{
	HRESULT hr;

	gD3D->GetSwapChain()->ResizeBuffers(1, gD3D->GetClientWidth(), gD3D->GetClientHeight(), DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	hr = gD3D->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pTex);
	if (FAILED(hr)) { return hr; }

	// バックバッファへのポインタを指定してレンダーターゲットビューを作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	if(gD3D->GetEnable4xMsaa())
	{
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}else
	{
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	}

	rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	hr = gD3D->GetDevice()->CreateRenderTargetView(m_pTex.Get(), &rtvDesc, m_pRTV.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTex->GetDesc(&desc);
		m_width = desc.Width;
		m_height = desc.Height;
	}
	return hr;


}
ID3D11RenderTargetView* RenderTarget::GetView() const
{
	return m_pRTV.Get();
}

ID3D11RenderTargetView** RenderTarget::GetViewAddress()
{
	return m_pRTV.GetAddressOf();
}

HRESULT RenderTarget::CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData)
{
	// テクスチャリソース作成
	HRESULT hr = Texture::CreateResource(desc, nullptr);
	if (FAILED(hr)) return hr;

	// 設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;

	if (desc.SampleDesc.Count > 1)
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;  // MSAA
	else
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;    // 非MSAA

	return gD3D->GetDevice()->CreateRenderTargetView(m_pTex.Get(), &rtvDesc, m_pRTV.GetAddressOf());
}

/// <summary>
/// 深度テクスチャ
/// </summary>
DepthStencil::DepthStencil()
	: m_pDSV(nullptr)
{
}
DepthStencil::~DepthStencil()
{
	
}
void DepthStencil::Clear()
{
	gD3D->GetContext()->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
HRESULT DepthStencil::Create(UINT width, UINT height, bool useStencil)
{
	// https://docs.microsoft.com/ja-jp/windows/win32/direct3d11/d3d10-graphics-programming-guide-depth-stencil#compositing
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(useStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS, width, height);
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	return CreateResource(desc);
}

HRESULT DepthStencil::Create(UINT width, UINT height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = gD3D->GetClientWidth();
	depthStencilDesc.Height = gD3D->GetClientHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 4X MSAA使うかどうか？
	if (gD3D->GetEnable4xMsaa())
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = gD3D->Get4xMsaaQuality() - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr=gD3D->GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, m_pTex.GetAddressOf());
	hr = gD3D->GetDevice()->CreateDepthStencilView(m_pTex.Get(), nullptr, m_pDSV.GetAddressOf());
	return hr;
}

ID3D11DepthStencilView* DepthStencil::GetView() const
{
	return m_pDSV.Get();
	
}
HRESULT DepthStencil::CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData)
{
	// ステンシル使用判定
	bool useStencil = (desc.Format == DXGI_FORMAT_R24G8_TYPELESS);

	// リソース生成
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = Texture::CreateResource(desc, nullptr);
	if (FAILED(hr)) return hr;

	// 設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = useStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;

	if (desc.SampleDesc.Count > 1)
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;  // MSAA
	else
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;    // 非MSAA

	return gD3D->GetDevice()->CreateDepthStencilView(m_pTex.Get(), &dsvDesc, m_pDSV.GetAddressOf());
}
