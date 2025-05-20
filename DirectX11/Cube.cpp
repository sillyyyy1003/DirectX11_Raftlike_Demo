#include "Cube.h"
using namespace DirectX;

Cube::Cube()
{
}

void Cube::Init(const char* fileName)
{

	//Create Mesh 
	m_meshes.resize(6);
	const float d = 0.5f;
	XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);

	XMFLOAT3 faceNormal[] = {
		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f,-1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f,-1.0f),
	};

	XMFLOAT3 pos[] = {
		//+X面
		XMFLOAT3(d, -d, -d),
		XMFLOAT3(d, d, -d),
		XMFLOAT3(d, d, d),
		XMFLOAT3(d, -d, d),
		// -X面
		XMFLOAT3(-d, -d, d),
		XMFLOAT3(-d, d, d),
		XMFLOAT3(-d, d, -d),
		XMFLOAT3(-d, -d, -d),
		// +Y面
		XMFLOAT3(-d, d, -d),
		XMFLOAT3(-d, d, d),
		XMFLOAT3(d, d, d),
		XMFLOAT3(d, d, -d),
		// -Y面
		XMFLOAT3(d, -d, -d),
		XMFLOAT3(d, -d, d),
		XMFLOAT3(-d, -d, d),
		XMFLOAT3(-d, -d, -d),
		// +Z面
		XMFLOAT3(d, -d, d),
		XMFLOAT3(d, d, d),
		XMFLOAT3(-d, d, d),
		XMFLOAT3(-d, -d, d),
		// -Z面
		XMFLOAT3(-d, -d, -d),
		XMFLOAT3(-d, d, -d),
		XMFLOAT3(d, d, -d),
		XMFLOAT3(d, -d, -d),
	};

	std::vector<XMFLOAT2> texCoord(4);
	texCoord = {
		XMFLOAT2(0.0f, 1.0f ),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.f , 0.0f),
		XMFLOAT2(1.0f , 1.0f),
	};

	//VERTEX List
	std::vector<Vertex> vtxData;

	vtxData.resize(24);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			vtxData[i * 4 + j] = { pos[i * 4 + j], faceNormal[i], texCoord[j] };
		}
	}

	//SetVertices(vtxData);

	for (UINT i = 0; i < m_meshes.size(); ++i)
	{
		//std::vector<Vertex::VtxPosColorNormal> vertexData;
		std::vector<Vertex> vertexData;
		std::vector<DWORD> indexData;
		// 每个面有4个顶点
		vertexData.resize(4);
		indexData.resize(6);
		for (int j = 0; j < 4; j++)
		{
			vertexData[j] = vtxData[i * 4 + j];
		}
		indexData = {
			0,1,2,2,3,0
		};

		MeshBuffer::MeshData desc = {};
		desc.pVertex = vertexData.data();
		desc.vertexSize = sizeof(Vertex);
		desc.vertexCount = static_cast<UINT>(vertexData.size());
		desc.pIndex = indexData.data();
		desc.indexSize = sizeof(DWORD);
		desc.indexCount = static_cast<UINT>(indexData.size());
		desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_meshes[i].mesh = std::make_unique<MeshBuffer>(desc);
	}
}

void Cube::Draw()
{
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		it->mesh->Draw();
	}
}
