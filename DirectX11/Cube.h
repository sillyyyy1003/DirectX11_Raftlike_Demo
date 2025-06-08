#pragma once
#include <memory>
#include <vector>
#include "MeshBuffer.h"



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
	void Init();
	void Draw();

};

