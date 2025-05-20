#pragma once
#include<D3DApp.h>

class MeshBuffer
{
public:

	struct MeshData
	{
		const void* pVertex; //頂点　異なる型のデータを処理する際に汎用的なポインタ
		UINT vertexSize;
		UINT vertexCount;
		const void* pIndex;
		UINT indexSize;//
		UINT indexCount;
		D3D11_PRIMITIVE_TOPOLOGY topology;
		bool isWrite = true;	//データの書き換えが可能かどうか
	};

	MeshData mData = {};

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Buffer> pVertexBuffer;
	ComPtr<ID3D11Buffer> pIndexBuffer;

public:

	MeshBuffer(const MeshData& _data);
	~MeshBuffer();

	ID3D11Buffer* GetVertexBuffer() const { return pVertexBuffer.Get(); };
	ID3D11Buffer* GetIndexBuffer() const { return pIndexBuffer.Get(); };

	/// @brief 頂点バッファを書き換える
	/// @param pVertex 頂点情報
	HRESULT Write(void* pVertex) const;

	void Draw(int count = 0);
private:

	/// @brief Create VtxPosColorNormal Buffer
	HRESULT CreateVertexBuffer(const void* pVertex, UINT size, UINT vertexCount);

	/// @brief  Create Index Buffer
	HRESULT CreateIndexBuffer(const void* pIndex, UINT size, UINT indexCount);
};

