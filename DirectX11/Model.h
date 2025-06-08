#pragma once
#include <memory>
#include "MeshBuffer.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"


//頂点データを扱う
class Model
{
private:

	std::unique_ptr<Assimp::Importer> importer = nullptr;
	const aiScene* m_scene = nullptr;

	typedef std::vector<std::shared_ptr<MeshBuffer>> Meshes;
	Meshes m_meshes;

public:

	Model();
	/// @brief モデルを読み込む
	/// @param _filePath ファイルパス
	bool Load(const char* file, bool flip = false, bool simpleMode = false);

	void Init(const char* filePath);

	void Draw();

};

