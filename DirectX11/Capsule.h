#pragma once
#include "Primitive.h"

///@brief Default Capsule
///半径:0.25,高さ:1.0
class Capsule:
	public Primitive
{
public:
	Capsule();

	/// @brief Capsule constructor
	/// @param levels  球体垂直分割数
	/// @param slices  球体/シリンダー水平分割数
	///	@param stacks  シリンダー垂直分割数
	void Init(UINT levels, UINT slices,UINT stacks);
	void Draw() override;

	
private:
	std::unique_ptr<MeshBuffer> m_Mesh;
	

};

