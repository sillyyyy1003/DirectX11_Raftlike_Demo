#include "D3DApp.h"


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

protected:
	D3D11_TEXTURE2D_DESC MakeTexDesc(DXGI_FORMAT format, UINT width, UINT height);
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData);

protected:
	UINT m_width;	///< 横幅
	UINT m_height;	///< 縦幅
	ComPtr<ID3D11ShaderResourceView> m_pSRV;
	ComPtr<ID3D11Texture2D> m_pTex;
};

/// <summary>
/// レンダーターゲット
/// </summary>
class RenderTarget : public Texture
{
public:
	RenderTarget();
	~RenderTarget();
	void Clear();
	void Clear(const float* color);
	HRESULT Create(DXGI_FORMAT format, UINT width, UINT height);
	HRESULT CreateFromScreen();
	ID3D11RenderTargetView* GetView() const;

protected:
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData = nullptr);

private:
	ComPtr<ID3D11RenderTargetView> m_pRTV;
};

/// <summary>
/// 深度テクスチャ
/// </summary>
class DepthStencil : public Texture
{
public:
	DepthStencil();
	~DepthStencil();
	void Clear();
	HRESULT Create(UINT width, UINT height, bool useStencil);
	ID3D11DepthStencilView* GetView() const;

protected:
	virtual HRESULT CreateResource(D3D11_TEXTURE2D_DESC& desc, const void* pData = nullptr);

private:
	ComPtr<ID3D11DepthStencilView> m_pDSV;
};

