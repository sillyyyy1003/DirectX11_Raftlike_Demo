#include "MeshBuffer.h"

#include "DXTrace.h"

MeshBuffer::MeshBuffer(const MeshData& _data)
{
    HR(CreateVertexBuffer(_data.pVertex, _data.vertexSize, _data.vertexCount));

    //Index あるなら
    if (_data.pIndex)
    {
        HR(CreateIndexBuffer(_data.pIndex, _data.indexSize, _data.indexCount));
    }

    mData = _data;


    rsize_t vertexTotalSize = static_cast<size_t>(_data.vertexSize) * _data.vertexCount;
    void* pVertex = new char[vertexTotalSize];
    memcpy_s(pVertex, vertexTotalSize, _data.pVertex, vertexTotalSize);
    mData.pVertex = pVertex;


    rsize_t indexTotalSize = static_cast<size_t>(_data.indexSize) * _data.indexCount;
    void* pIndex = new char[indexTotalSize];
    memcpy_s(pIndex, indexTotalSize, _data.pIndex, indexTotalSize);
    mData.pIndex = pIndex;

    delete[] pVertex;
    delete[] pIndex;
}

MeshBuffer::~MeshBuffer()
{
}

HRESULT MeshBuffer::Write(void* pVertex) const
{
	if (!mData.isWrite) { return E_FAIL; }

	HRESULT hr;
	ID3D11Device* pDevice =gD3D->GetDevice();
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// データコピー
	hr = pContext->Map(pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = static_cast<size_t>(mData.vertexCount) * mData.vertexSize;
		memcpy_s(mapResource.pData, size, pVertex, size);
		pContext->Unmap(pVertexBuffer.Get(), 0);
	}
	return hr;
}

void MeshBuffer::Draw(int count)
{
	ID3D11DeviceContext* pContext = gD3D->GetContext();
	UINT stride = mData.vertexSize;
	UINT offset = 0;

	// トポロジの設定
	ID3D11HullShader* hullShader;
	pContext->HSGetShader(&hullShader, nullptr, nullptr);
	if (hullShader)
		hullShader->Release();
	else
		pContext->IASetPrimitiveTopology(mData.topology);

	// 頂点バッファ設定
	pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// 描画
	if (mData.indexCount > 0)
	{
		DXGI_FORMAT format = {};
		switch (mData.indexSize)
		{
		case 4: format = DXGI_FORMAT_R32_UINT; break;
		case 2: format = DXGI_FORMAT_R16_UINT; break;
		}
		pContext->IASetIndexBuffer(pIndexBuffer.Get(), format, 0);
		pContext->DrawIndexed(count ? count : mData.indexCount, 0, 0);
	}
	else
	{
		// 頂点バッファのみで描画
		pContext->Draw(count ? count : mData.vertexCount, 0);
	}

}

HRESULT MeshBuffer::CreateVertexBuffer(const void* pVertex, UINT size, UINT vertexCount)
{//--- 作成するバッファの情報
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * vertexCount;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (mData.isWrite)
	{
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	//--- バッファの初期値を設定
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pVertex;

	//--- 頂点バッファの作成
	HRESULT hr;
	ID3D11Device* pDevice = gD3D->GetDevice();
	hr=pDevice->CreateBuffer(&bufDesc, &subResource, pVertexBuffer.GetAddressOf());
	return hr;
}

HRESULT MeshBuffer::CreateIndexBuffer(const void* pIndex, UINT size, UINT indexCount)
{// インデックスサイズの確認
	switch (size)
	{
	default:
		return E_FAIL;
	case 2:
	case 4:
		break;
	}

	// バッファの情報を設定
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * indexCount;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// バッファの初期データ
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pIndex;

	// インデックスバッファ生成
	ID3D11Device* pDevice = gD3D->GetDevice();
	HRESULT hr;
	hr=pDevice->CreateBuffer(&bufDesc, &subResource, pIndexBuffer.GetAddressOf());
	return hr;
}
