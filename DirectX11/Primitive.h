#pragma once
#include <memory>

#include "MeshBuffer.h"

/// @brief base class for all primitives
class Primitive
{
public:

	Primitive() = default;
	virtual ~Primitive() = default;

	virtual void Draw() = 0; // pure virtual function to draw the primitive
};

/// <summary>
/// Square Mesh
/// </summary>
class Square : public Primitive
{
private:

	std::unique_ptr<MeshBuffer> m_pMeshBuffer;

public:
	Square() = default;
	~Square() override = default;

	void Init();
	void Draw() override;
};

