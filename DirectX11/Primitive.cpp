#include "Primitive.h"
#include <vector>

void SquareMesh::Init()
{
	// Create Mesh
	// Pos Normal TexCoord 頂点を使う
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

	std::vector<DWORD> idx;
	idx.resize(6);
	idx = {
		0,1,2,2,3,0
	};

	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtx.data();
	desc.vertexSize = sizeof(MeshBuffer::Vertex);
	desc.vertexCount = static_cast<UINT>(vtx.size());
	desc.pIndex = idx.data();
	desc.indexSize = sizeof(DWORD);
	desc.indexCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pMeshBuffer = std::make_unique<MeshBuffer>(desc);

}

void SquareMesh::Draw()
{
	m_pMeshBuffer->Draw();
}


//===============Plane==============================
void PlaneMesh::Init(UINT slices, float size)
{
	uint32_t vertexCount = (slices + 1) * (slices + 1);
	uint32_t indexCount = 6 * slices * slices;
	std::vector<MeshBuffer::Vertex> vtx;
	std::vector<DWORD> idx;
	vtx.resize(vertexCount);
	idx.resize(indexCount);

	float sliceWidth = size / slices;
	float sliceDepth = size / slices;
	float leftBottomX = -size / 2;
	float leftBottomZ = -size / 2;
	float posX, posZ;
	float sliceTexWidth = 1.0f / slices;
	float sliceTexDepth = 1.0f / slices;
	uint32_t vIndex = 0;
	uint32_t iIndex = 0;
	//  __ __
	  // | /| /|
	  // |/_|/_|
	  // | /| /| 
	  // |/_|/_|
	for (uint32_t z = 0; z <= slices; ++z)
	{
		posZ = leftBottomZ + z * sliceDepth;
		for (uint32_t x = 0; x <= slices; ++x)
		{
			posX = leftBottomX + x * sliceWidth;
			DirectX::XMFLOAT3 normal = { 0,1,0 };
			vtx[vIndex] = {
				{posX,0.0f,posZ},
				normal,
				{x * sliceTexWidth,1.0f - z * sliceTexDepth}
			};
			vIndex++;
		}
	}

	for (uint32_t i = 0; i < slices; ++i)
	{
		for (uint32_t j = 0; j < slices; ++j)
		{

			idx[iIndex++] = i * (slices + 1) + j;
			idx[iIndex++] = (i + 1) * (slices + 1) + j;
			idx[iIndex++] = (i + 1) * (slices + 1) + j + 1;

			idx[iIndex++] = (i + 1) * (slices + 1) + j + 1;
			idx[iIndex++] = i * (slices + 1) + j + 1;
			idx[iIndex++] = i * (slices + 1) + j;
		}
	}

	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtx.data();
	desc.vertexSize = sizeof(MeshBuffer::Vertex);
	desc.vertexCount = static_cast<UINT>(vtx.size());
	desc.pIndex = idx.data();
	desc.indexSize = sizeof(DWORD);
	desc.indexCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pMeshBuffer = std::make_unique<MeshBuffer>(desc);

}




void PlaneMesh::Draw()
{
	m_pMeshBuffer->Draw();
}

void PlaneMesh::Init(UINT slices, float size, std::vector<MeshBuffer::Vertex>& vertices)
{
	uint32_t vertexCount = (slices + 1) * (slices + 1);
	uint32_t indexCount = 6 * slices * slices;
	std::vector<MeshBuffer::Vertex> vtx;
	std::vector<DWORD> idx;
	vtx.resize(vertexCount);
	idx.resize(indexCount);

	float sliceWidth = size / slices;
	float sliceDepth = size / slices;
	float leftBottomX = -size / 2;
	float leftBottomZ = -size / 2;
	float posX, posZ;
	float sliceTexWidth = 1.0f / slices;
	float sliceTexDepth = 1.0f / slices;
	uint32_t vIndex = 0;
	uint32_t iIndex = 0;
	//  __ __
	  // | /| /|
	  // |/_|/_|
	  // | /| /| 
	  // |/_|/_|
	for (uint32_t z = 0; z <= slices; ++z)
	{
		posZ = leftBottomZ + z * sliceDepth;
		for (uint32_t x = 0; x <= slices; ++x)
		{
			posX = leftBottomX + x * sliceWidth;
			DirectX::XMFLOAT3 normal = { 0,1,0 };
			vtx[vIndex] = {
				{posX,0.0f,posZ},
				normal,
				{x * sliceTexWidth,1.0f - z * sliceTexDepth}
			};
			vIndex++;
		}
	}

	for (uint32_t i = 0; i < slices; ++i)
	{
		for (uint32_t j = 0; j < slices; ++j)
		{

			idx[iIndex++] = i * (slices + 1) + j;
			idx[iIndex++] = (i + 1) * (slices + 1) + j;
			idx[iIndex++] = (i + 1) * (slices + 1) + j + 1;

			idx[iIndex++] = (i + 1) * (slices + 1) + j + 1;
			idx[iIndex++] = i * (slices + 1) + j + 1;
			idx[iIndex++] = i * (slices + 1) + j;
		}
	}

	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtx.data();
	desc.vertexSize = sizeof(MeshBuffer::Vertex);
	desc.vertexCount = static_cast<UINT>(vtx.size());
	desc.pIndex = idx.data();
	desc.indexSize = sizeof(DWORD);
	desc.indexCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pMeshBuffer = std::make_unique<MeshBuffer>(desc);

	// copy to output
	vertices = std::move(vtx);
}
