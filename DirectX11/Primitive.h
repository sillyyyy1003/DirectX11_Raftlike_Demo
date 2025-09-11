#pragma once
#include <memory>
#include <vector>

#include "MeshBuffer.h"

/// @brief base class for all primitives
class Primitive
{
public:

	Primitive() = default;
	virtual ~Primitive() = default;

	virtual void Draw() = 0; // pure virtual function to draw the primitive
	virtual DirectX::XMFLOAT3 GetModelSize() const = 0;

};

/// <summary>
/// SquareMesh Mesh
/// </summary>
class SquareMesh : public Primitive
{
private:

	std::unique_ptr<MeshBuffer> m_pMeshBuffer;

public:
	SquareMesh() = default;
	~SquareMesh() override = default;

	void Init();
	void Draw() override;
	DirectX::XMFLOAT3 GetModelSize() const override { return { 1,1,0.1f }; }
};


class PlaneMesh :
	public Primitive
{
private:
	std::unique_ptr<MeshBuffer> m_pMeshBuffer;
public:
	PlaneMesh() = default;

	/// @brief メッシュ初期化
	/// @param slices Mesh slicesメッシュ分割
	/// @param size メッシュサイズ
	void Init(UINT slices, float size = 1);

	/// @brief メッシュ初期化 (頂点情報を外に渡す)
	void Init(UINT slices, float size, std::vector<MeshBuffer::Vertex>& vertices);

	void Draw() override;

	MeshBuffer* GetMeshBuffer() const
	{
		return m_pMeshBuffer.get();
	}



	DirectX::XMFLOAT3 GetModelSize() const override { return { 1,1,0.1f }; }
};