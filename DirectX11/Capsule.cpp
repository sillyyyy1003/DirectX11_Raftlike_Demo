#include "Capsule.h"
#include <vector>

Capsule::Capsule() :
	m_Mesh(nullptr)
{
}

void Capsule::Init(UINT levels, UINT slices, UINT stacks)
{
	//VERTEX List
	std::vector<MeshBuffer::Vertex> vtxData;
	std::vector<DWORD> idx;

	float phi = 0.0f, theta = 0.0f;
	float radius = 0.5f;
	float height = 1.0f;
	float h2 = height / 2.0f;
	float per_phi = DirectX::XM_PI / levels;
	float per_theta = DirectX::XM_2PI / slices;
	float stackHeight = height / stacks;

	// Cylinder body vertices
	for (UINT i = 0; i <= stacks; i++)
	{
		float y = -h2 + i * stackHeight;
		for (UINT j = 0; j <= slices; j++)
		{
			theta = j * per_theta;
			float x = radius * cosf(theta);
			float z = radius * sinf(theta);

			float u = theta / (DirectX::XM_2PI);
			float v = 1.0f - static_cast<float>(i) / (stacks);

			vtxData.push_back({
				DirectX::XMFLOAT3(x, y, z),
				DirectX::XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
				DirectX::XMFLOAT2(u,v)
				});
		}
	}

	//Top Semi_Sphere
	vtxData.push_back({
		DirectX::XMFLOAT3(0.0f, radius + h2, 0.0f),
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
		DirectX::XMFLOAT2(0.0f, 0.0f),
		});
	for (UINT i = 1; i < levels / 2; i++)
	{
		phi = per_phi * i;

		// slices+1->スタートとエンドは同じけど、uvは逆
		for (UINT j = 0; j <= slices; j++)
		{
			theta = per_theta * j;
			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi) + h2;
			float z = radius * sinf(phi) * sinf(theta);
			DirectX::XMFLOAT3 pos = { x,y,z };
			DirectX::XMFLOAT3 centerToVertex = { x, y - h2, z }; // Subtract h2 to move the vertex relative to the sphere's center
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&centerToVertex)));

			vtxData.push_back({
				pos,
				normal,
				DirectX::XMFLOAT2(theta / (DirectX::XM_2PI), phi / (DirectX::XM_PI))
				});
		}
	}

	//Bot Hemisphere
	for (UINT i = levels / 2; i <= levels; i++)
	{
		phi = per_phi * i;

		// slices+1->スタートとエンドは同じけど、uvは逆
		for (UINT j = 0; j <= slices; j++)
		{
			theta = per_theta * j;
			float x = radius * sinf(phi) * cosf(theta);
			float y = -h2 + radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);
			DirectX::XMFLOAT3 pos = { x,y,z };
			DirectX::XMFLOAT3 centerToVertex = { x, y + h2, z }; // Subtract h2 to move the vertex relative to the sphere's center
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&centerToVertex)));

			vtxData.push_back({
				pos,
				normal,
				DirectX::XMFLOAT2(theta / (DirectX::XM_2PI), phi / (DirectX::XM_PI))
				});
		}
	}

	//Bot point
	vtxData.push_back({
		DirectX::XMFLOAT3(0.0f, -h2 - radius, 0.0f),
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
		DirectX::XMFLOAT2(0.0f, 0.0f),
		});

	//Cylinder
	for (UINT i = 0; i < stacks; i++)
	{
		for (UINT j = 0; j < slices; j++)
		{
			idx.push_back(i * (slices + 1) + j);
			idx.push_back((i + 1) * (slices + 1) + j);
			idx.push_back((i + 1) * (slices + 1) + j + 1);

			idx.push_back(i * (slices + 1) + j);
			idx.push_back((i + 1) * (slices + 1) + j + 1);
			idx.push_back(i * (slices + 1) + j + 1);
		}
	}

	// Offset to move to the top hemisphere vertices
	UINT offset = (slices + 1) * (stacks + 1);

	// Top hemisphere indices
	for (UINT i = 0; i <= slices; i++)
	{
		// Top point of the hemisphere
		idx.push_back(offset + 0);
		idx.push_back(offset + i % (slices + 1) + 1);
		idx.push_back(offset + i);
	}

	for (UINT i = 1; i < levels / 2; i++)
	{
		for (UINT j = 1; j <= slices; j++)
		{
			idx.push_back(offset + (i - 1) * (slices + 1) + j);
			idx.push_back(offset + (i - 1) * (slices + 1) + j % (slices + 1) + 1);
			idx.push_back(offset + i * (slices + 1) + j % (slices + 1) + 1);

			idx.push_back(offset + i * (slices + 1) + j % (slices + 1) + 1);
			idx.push_back(offset + i * (slices + 1) + j);
			idx.push_back(offset + (i - 1) * (slices + 1) + j);
		}
	}

	// Offset to move to the bottom hemisphere vertices
	offset += (slices + 1) * (levels / 2);

	// Bottom hemisphere indices
	for (UINT i = 0; i <= levels / 2; i++)
	{
		for (UINT j = 0; j <= slices; j++)
		{
			idx.push_back(offset + (i - 1) * (slices + 1) + j);
			idx.push_back(offset + (i - 1) * (slices + 1) + j % (slices + 1) + 1);
			idx.push_back(offset + i * (slices + 1) + j % (slices + 1) + 1);

			idx.push_back(offset + i * (slices + 1) + j % (slices + 1) + 1);
			idx.push_back(offset + i * (slices + 1) + j);
			idx.push_back(offset + (i - 1) * (slices + 1) + j);
		}
	}

	for (UINT i = 0; i < slices; i++)
	{
		// Bottom point of the hemisphere
		idx.push_back(offset + (levels - 2) * (slices + 1) + i);
		idx.push_back(offset + (levels - 2) * (slices + 1) + i % (slices + 1) + 1);
		idx.push_back(offset + (levels - 1) * (slices + 1) + 1);
	}

	MeshBuffer::MeshData desc = {};
	desc.pVertex = vtxData.data();
	desc.vertexSize = sizeof(MeshBuffer::Vertex);
	desc.vertexCount = static_cast<UINT>(vtxData.size());
	desc.pIndex = idx.data();
	desc.indexSize = sizeof(DWORD);
	desc.indexCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_Mesh = std::make_unique<MeshBuffer>(desc);

}



void Capsule::Draw()
{
    m_Mesh->Draw();
}
