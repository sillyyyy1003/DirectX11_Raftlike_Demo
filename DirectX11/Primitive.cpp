#include "Primitive.h"

#include <vector>

void SquareMesh::Init()
{
	// Create Mesh
	// Pos Normal TexCoord ’¸“_‚ðŽg‚¤
	const float d = 0.5f;
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 pos[] = {
		DirectX::XMFLOAT3(-d, d, 0),	//left top
		DirectX::XMFLOAT3(d, d, 0),	//right top
		DirectX::XMFLOAT3(d, -d,0),	//right bot
		DirectX::XMFLOAT3(-d, -d, 0)	//left bot
	};

	std::vector<MeshBuffer::Vertex> vtx;
	vtx.resize(4);

	vtx = {
		//+y
		{pos[0],DirectX::XMFLOAT3(0.0f,0.0f,-1.0f),DirectX::XMFLOAT2(0.f,0.f)},
		{pos[1],DirectX::XMFLOAT3(0.0f,0.0f,-1.0f),DirectX::XMFLOAT2(1.f,0.f)},
		{pos[2],DirectX::XMFLOAT3(0.0f,0.0f,-1.0f),DirectX::XMFLOAT2(1.f,1.f)},
		{pos[3],DirectX::XMFLOAT3(0.0f,0.0f,-1.0f),DirectX::XMFLOAT2(0.f,1.f)},
	};

	std::vector<DWORD> indexData;
	indexData.resize(6);
	indexData = {
		0,1,2,2,3,0
	};

	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtx.data();
	desc.vertexSize = sizeof(MeshBuffer::Vertex);
	desc.vertexCount = static_cast<UINT>(vtx.size());
	desc.pIndex = indexData.data();
	desc.indexSize = sizeof(DWORD);
	desc.indexCount = static_cast<UINT>(indexData.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pMeshBuffer = std::make_unique<MeshBuffer>(desc);

}

void SquareMesh::Draw()
{
	m_pMeshBuffer->Draw();
}
