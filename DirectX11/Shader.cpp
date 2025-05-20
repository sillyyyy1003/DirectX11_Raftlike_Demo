#include "Shader.h"
#include <d3dcompiler.h>
#include "d3dUtil.h"

#pragma comment(lib, "d3dcompiler.lib")

//----------
// 基本クラス
Shader::Shader(Kind kind)
	: m_kind(kind)
{
}
Shader::~Shader()
{
}
HRESULT Shader::Load(const char* pFileName)
{
	HRESULT hr = E_FAIL;

	// ファイルを読み込む
	FILE* fp;
	fopen_s(&fp, pFileName, "rb");
	if (!fp) { return hr; }

	// ファイルのサイズを調べる
	int fileSize = 0;
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);

	// メモリに読み込み
	fseek(fp, 0, SEEK_SET);
	char* pData = new char[fileSize];
	fread(pData, fileSize, 1, fp);
	fclose(fp);

	// シェーダー作成
	hr = Make(pData, fileSize);

	// 終了処理
	if (pData) { delete[] pData; }
	return hr;
}
HRESULT Shader::Compile(const char* pCode)
{
	static const char* pTargetList[] =
	{
		"vs_5_0",
		"gs_5_0",
		"hs_5_0",
		"ds_5_0",
		"ps_5_0"
	};

	HRESULT hr;
	ID3DBlob* pBlob;
	ID3DBlob* error;
	UINT compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	hr = D3DCompile(pCode, strlen(pCode), nullptr, nullptr, nullptr,
		"main", pTargetList[m_kind], compileFlag, 0, &pBlob, &error);
	if (FAILED(hr)) { return hr; }

	// シェーダ作成
	hr = Make(pBlob->GetBufferPointer(), (UINT)pBlob->GetBufferSize());
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(error);
	return hr;
}

void Shader::WriteBuffer(UINT slot, void* pData)
{
	if (slot < m_pBuffers.size())
		gD3D->GetContext()->UpdateSubresource(m_pBuffers[slot].Get(), 0, nullptr, pData, 0, 0);
	assert(m_pBuffers[slot] != nullptr);
}
void Shader::SetTexture(UINT slot, Texture* tex)
{
	if (slot >= m_pTextures.size()) { return; }
	ID3D11ShaderResourceView* pTex = tex ? tex->GetResource() : nullptr;
	m_pTextures[slot] = pTex;
	switch (m_kind)
	{
	case Vertex:	gD3D->GetContext()->VSSetShaderResources(slot, 1, &pTex); break;
	case Pixel:		gD3D->GetContext()->PSSetShaderResources(slot, 1, &pTex); break;
	case Hull:		gD3D->GetContext()->HSSetShaderResources(slot, 1, &pTex); break;
	case Domain:	gD3D->GetContext()->DSSetShaderResources(slot, 1, &pTex); break;
	}
}

HRESULT Shader::Make(void* pData, UINT size)
{
	HRESULT hr;
	ID3D11Device* pDevice = gD3D->GetDevice();

	// 解析用のリフレクション作成
	ID3D11ShaderReflection* pReflection;
	hr = D3DReflect(pData, size, IID_PPV_ARGS(&pReflection));
	if (FAILED(hr)) { return hr; }

	// 定数バッファ作成
	D3D11_SHADER_DESC shaderDesc;
	pReflection->GetDesc(&shaderDesc);
	m_pBuffers.resize(shaderDesc.ConstantBuffers, nullptr);
	for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		// シェーダーの定数バッファの情報を取得
		D3D11_SHADER_BUFFER_DESC shaderBufDesc;
		ID3D11ShaderReflectionConstantBuffer* cbuf = pReflection->GetConstantBufferByIndex(i);
		cbuf->GetDesc(&shaderBufDesc);

		// 作成するバッファの情報
		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.ByteWidth = shaderBufDesc.Size;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		// バッファの作成
		hr = pDevice->CreateBuffer(&bufDesc, nullptr, m_pBuffers[i].GetAddressOf());
		if (FAILED(hr)) { return hr; }
	}
	// テクスチャ領域作成
	m_pTextures.resize(shaderDesc.BoundResources, nullptr);

	return MakeShader(pData, size);
}

//----------
// 頂点シェーダ
VertexShader::VertexShader()
	: Shader(Shader::Vertex)
	, m_pVS(nullptr)
	, m_pInputLayout(nullptr)
{
}

VertexShader::~VertexShader()
{
	//SAFE_RELEASE(m_pVS);
}

void VertexShader::Bind(void)
{
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	pContext->VSSetShader(m_pVS.Get(), NULL, 0);
	pContext->IASetInputLayout(m_pInputLayout.Get());
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->VSSetConstantBuffers(i, 1, m_pBuffers[i].GetAddressOf());
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->VSSetShaderResources(i, 1, m_pTextures[i].GetAddressOf());
}

HRESULT VertexShader::MakeShader(void* pData, UINT size)
{
	HRESULT hr;
	ID3D11Device* pDevice = gD3D->GetDevice();

	// シェーダー作成
	hr = pDevice->CreateVertexShader(pData, size, NULL, m_pVS.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	/*
	シェーダ作成時にシェーダリフレクションを通してインプットレイアウトを取得
	セマンティクスの配置などから識別子を作成
	識別子が登録済→再利用、なければ新規作成
	https://blog.techlab-xe.net/dxc-shader-reflection/
	*/

	ID3D11ShaderReflection* pReflection;
	D3D11_SHADER_DESC shaderDesc;
	D3D11_INPUT_ELEMENT_DESC* pInputDesc;
	D3D11_SIGNATURE_PARAMETER_DESC sigDesc;

	DXGI_FORMAT formats[][4] =
	{
		{
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32B32_UINT,
			DXGI_FORMAT_R32G32B32A32_UINT,
		}, {
			DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT_R32G32_SINT,
			DXGI_FORMAT_R32G32B32_SINT,
			DXGI_FORMAT_R32G32B32A32_SINT,
		}, {
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		},
	};

	hr = D3DReflect(pData, size, IID_PPV_ARGS(&pReflection));
	if (FAILED(hr)) { return hr; }

	pReflection->GetDesc(&shaderDesc);
	pInputDesc = new D3D11_INPUT_ELEMENT_DESC[shaderDesc.InputParameters];
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
	{
		pReflection->GetInputParameterDesc(i, &sigDesc);
		pInputDesc[i].SemanticName = sigDesc.SemanticName;
		pInputDesc[i].SemanticIndex = sigDesc.SemanticIndex;

		// http://marupeke296.com/TIPS_No17_Bit.html
		BYTE elementCount = sigDesc.Mask;
		elementCount = (elementCount & 0x05) + ((elementCount >> 1) & 0x05);
		elementCount = (elementCount & 0x03) + ((elementCount >> 2) & 0x03);

		switch (sigDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			pInputDesc[i].Format = formats[0][elementCount - 1];
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			pInputDesc[i].Format = formats[1][elementCount - 1];
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			pInputDesc[i].Format = formats[2][elementCount - 1];
			break;
		}
		pInputDesc[i].InputSlot = 0;
		pInputDesc[i].AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		pInputDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pInputDesc[i].InstanceDataStepRate = 0;
	}

	hr = pDevice->CreateInputLayout(
		pInputDesc, shaderDesc.InputParameters,
		pData, size, m_pInputLayout.GetAddressOf()
	);

	delete[] pInputDesc;
	return hr;
}

//----------
// ピクセルシェーダ
PixelShader::PixelShader()
	: Shader(Shader::Pixel)
	, m_pPS(nullptr)
{
}
PixelShader::~PixelShader()
{
	//SAFE_RELEASE(m_pPS);
}
void PixelShader::Bind(void)
{
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	pContext->PSSetShader(m_pPS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->PSSetConstantBuffers(i, 1, m_pBuffers[i].GetAddressOf());
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->PSSetShaderResources(i, 1, m_pTextures[i].GetAddressOf());
}
HRESULT PixelShader::MakeShader(void* pData, UINT size)
{
	return gD3D->GetDevice()->CreatePixelShader(pData, size, NULL, m_pPS.GetAddressOf());
}

//----------
// ハルシェーダ
HullShader::HullShader()
	: Shader(Shader::Hull)
	, m_pHS(nullptr)
{
}
HullShader::~HullShader()
{
	//SAFE_RELEASE(m_pHS);
}
void HullShader::Bind(void)
{
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	pContext->HSSetShader(m_pHS.Get(), nullptr, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->HSSetConstantBuffers(i, 1, m_pBuffers[i].GetAddressOf());
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->HSSetShaderResources(i, 1, m_pTextures[i].GetAddressOf());
}
void HullShader::Unbind(void)
{
	gD3D->GetContext()->HSSetShader(nullptr, nullptr, 0);
}

HRESULT HullShader::MakeShader(void* pData, UINT size)
{
	return gD3D->GetDevice()->CreateHullShader(pData, size, NULL, &m_pHS);
}

//----------
// ドメインシェーダ
DomainShader::DomainShader()
	: Shader(Shader::Domain)
	, m_pDS(nullptr)
{
}
DomainShader::~DomainShader()
{
	//SAFE_RELEASE(m_pDS);
}
void DomainShader::Bind(void)
{
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	pContext->DSSetShader(m_pDS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->DSSetConstantBuffers(i, 1, m_pBuffers[i].GetAddressOf());
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->DSSetShaderResources(i, 1, m_pTextures[i].GetAddressOf());
}
void DomainShader::Unbind(void)
{
	gD3D->GetContext()->DSSetShader(nullptr, nullptr, 0);
}
HRESULT DomainShader::MakeShader(void* pData, UINT size)
{
	return gD3D->GetDevice()->CreateDomainShader(pData, size, NULL, &m_pDS);
}

//----------
// ジオメトリシェーダー
GeometryShader::GeometryShader()
	: Shader(Shader::Geometry)
	, m_pGS(nullptr)
{
}
GeometryShader::~GeometryShader()
{
	//SAFE_RELEASE(m_pGS);
}
void GeometryShader::Bind()
{
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	pContext->GSSetShader(m_pGS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->GSSetConstantBuffers(i, 1, m_pBuffers[i].GetAddressOf());
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->GSSetShaderResources(i, 1, m_pTextures[i].GetAddressOf());
}
void GeometryShader::Unbind()
{
	gD3D->GetContext()->GSSetShader(nullptr, nullptr, 0);
}
HRESULT GeometryShader::MakeShader(void* pData, UINT size)
{
	return gD3D->GetDevice()->CreateGeometryShader(pData, size, NULL, m_pGS.GetAddressOf());
}