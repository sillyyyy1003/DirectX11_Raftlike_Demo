#pragma once
#include <memory>
#include "MeshBuffer.h"
#include "Primitive.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>



//頂点データを扱う
class Model :public Primitive
{
private:

	std::unique_ptr<Assimp::Importer> importer = nullptr;
	const aiScene* m_scene = nullptr;

	typedef std::vector<std::shared_ptr<MeshBuffer>> Meshes;
	Meshes m_meshes;

public:

	Model();
	/// @brief モデルを読み込む
	/// @param file ファイルパス
	///	@param flip Y軸を反転するかどうか
	///	@param simpleMode trueならば、頂点データのみを読み込む。falseならば、マテリアルやテクスチャも読み込む。
	bool Load(const char* file, bool flip = false, bool simpleMode = false);

	void Init(const char* filePath);

	void Draw() override;

};

