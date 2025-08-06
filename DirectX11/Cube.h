#pragma once
#include <memory>
#include <vector>
#include "MeshBuffer.h"
#include "Primitive.h"


class Cube:public Primitive
{
private:

	struct MeshData
	{
		std::unique_ptr<MeshBuffer> mesh;
	};
	using Meshes = std::vector<MeshData>;
	Meshes m_pMeshes;

public:
	Cube();
	void Init();
	void Draw() override;

	DirectX::XMFLOAT3 GetModelSize() const override { return { 1,1,1 }; }
};

/// @brief Used for rendering skybox as a cube
class SkyBoxCube :
	public Primitive
{
public:
	SkyBoxCube();
	~SkyBoxCube() override = default;
	void Init();
	void Draw() override;

	DirectX::XMFLOAT3 GetModelSize() const override { return { 1,1,1 }; }
private:
	struct MeshData
	{
		std::unique_ptr<MeshBuffer> mesh;
	};
	using Meshes = std::vector<MeshData>;
	Meshes m_pMeshes;
};