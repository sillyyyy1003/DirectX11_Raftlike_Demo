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
	desc.SampleDesc.Count = 1;
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
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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
