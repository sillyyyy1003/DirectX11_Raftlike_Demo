#pragma once
#include <memory>
#include <vector>
#include "MeshBuffer.h"

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;
};

class Cube
{
private:
	struct MeshData
	{
		std::unique_ptr<MeshBuffer> mesh;
	};
	using Meshes = std::vector<MeshData>;
	Meshes m_meshes;

public:
	Cube();
	void Init(const char* fileName);
	void Draw();

};

