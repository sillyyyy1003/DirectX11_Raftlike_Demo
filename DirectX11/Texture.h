#pragma once
#include <d3d11_1.h>
#include <vector>
#include <wrl/client.h>

/// <summary>
/// テクスチャ
/// </summary>
class Texture
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	Texture();
	virtual ~Texture();
	HRESULT Create(const char* fileName);
	HRESULT Create(DXGI_FORMAT format, UINT width, UINT height, const void* pData = nullptr);

	UINT GetWidth() const;
	UINT GetHeight() const;
	ID3D11ShaderResourceView* GetResource() const;
	ID3D11Texture2D* GetTexture() const;

protected:
	D3D11_TEXTURE2D_DESC MakeTexDesc(DXGI_FORMAT format, UINT width, UINT height);
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData);

protected:
	UINT m_width;	///< 横幅
	UINT m_height;	///< 縦幅
	ComPtr<ID3D11ShaderResourceView> m_pSRV;
	ComPtr<ID3D11Texture2D> m_pTex;
};

class TextureCube:
	public Texture
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	TextureCube();
	~TextureCube() override = default;
	void CreateTextureCube(UINT width, UINT height, DXGI_FORMAT format, UINT mipLevels);
	void CopyTextures(const std::vector<Texture*>& textures);

};