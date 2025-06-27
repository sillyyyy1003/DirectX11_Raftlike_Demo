#include "Model.h"
#include "DebugLog.h"
#include <assimp/postprocess.h>

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif

Model::Model()
{
	importer = std::make_unique<Assimp::Importer>();
}

bool Model::Load(const char* file, bool flip, bool simpleMode)
{
	DebugLog::Log("モデル読み込み開始");
	int flag = 0;
	if (simpleMode)
	{
		flag |= aiProcess_Triangulate;					// 非三角ポリゴンを三角に割る
		flag |= aiProcess_JoinIdenticalVertices;		// 同一位置頂点を一つに統合する
		flag |= aiProcess_FlipUVs;						//　UV値をY軸を基準に反転させる
		flag |= aiProcess_PreTransformVertices;			// ノードを一つに統合 !!アニメーション情報が消えることに注意!!
		if (flip) flag |= aiProcess_MakeLeftHanded;		// 左手系座標に変換
	}
	else
	{
		flag |= aiProcessPreset_TargetRealtime_MaxQuality;	// リアルタイム レンダリング用にデータを最適化するデフォルトの後処理構成。
		flag |= aiProcess_PopulateArmatureData;				// 標準的なボーン,アーマチュアの設定
		if (flip) flag |= aiProcess_ConvertToLeftHanded;	// 左手系変更オプションがまとまったもの
	}

	// assimpで読み込み
	m_scene = importer.get()->ReadFile(file, flag);
	if (!m_scene)
	{
		//Error(importer->GetErrorString());
		DebugLog::Log("{} Assimpモデルロード失敗", file);
		return false;
	}

	// メッシュの作成
	aiVector3D zero(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < m_scene->mNumMeshes; ++i)
	{
		std::shared_ptr<MeshBuffer> mesh;

		// 頂点の作成
		std::vector <MeshBuffer::VtxPosNormalTexTangent> vtx;
		vtx.resize(m_scene->mMeshes[i]->mNumVertices);
		for (unsigned int j = 0; j < vtx.size(); ++j)
		{
			// 値の吸出し
			aiVector3D pos = m_scene->mMeshes[i]->mVertices[j];
			aiVector3D uv = m_scene->mMeshes[i]->HasTextureCoords(0) ?
				m_scene->mMeshes[i]->mTextureCoords[0][j] : zero;
			aiVector3D normal = m_scene->mMeshes[i]->HasNormals() ?
				m_scene->mMeshes[i]->mNormals[j] : zero;
			aiVector3D tangent = m_scene->mMeshes[i]->HasTangentsAndBitangents() ?
				m_scene->mMeshes[i]->mTangents[j] : zero;
			// 値を設定
			vtx[j] = {
				DirectX::XMFLOAT3(pos.x, pos.y, pos.z),
				DirectX::XMFLOAT3(normal.x, normal.y, normal.z),
				DirectX::XMFLOAT2(uv.x, 1 - uv.y),
				DirectX::XMFLOAT3(tangent.x,tangent.y,tangent.z),
			};
		}


		// インデックスの作成
		std::vector<unsigned int> idx;
		idx.resize(m_scene->mMeshes[i]->mNumFaces * 3); // faceはポリゴンの数を表す(１ポリゴンで3インデックス
		for (unsigned int j = 0; j < m_scene->mMeshes[i]->mNumFaces; ++j)
		{
			aiFace face = m_scene->mMeshes[i]->mFaces[j];
			int faceIdx = j * 3;
			idx[faceIdx + 0] = face.mIndices[0];
			idx[faceIdx + 1] = face.mIndices[1];
			idx[faceIdx + 2] = face.mIndices[2];
		}

		// メッシュを元に頂点バッファ作成
		MeshBuffer::MeshData desc = {};
		desc.pVertex = vtx.data();
		desc.vertexSize = sizeof(MeshBuffer::VtxPosNormalTexTangent);
		desc.vertexCount = static_cast<UINT>(vtx.size());
		desc.pIndex = idx.data();
		desc.indexSize = sizeof(unsigned int);
		desc.indexCount = static_cast<UINT>(idx.size());
		desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mesh = std::make_shared<MeshBuffer>(desc);

		// メッシュ追加
		m_meshes.push_back(mesh);
	}

	DebugLog::Log("モデル読み込み完了");
	return true;
}

void Model::Init(const char* filePath)
{
	Load(filePath);
}

void Model::Draw()
{
	for(const auto& it:m_meshes)
	{
		it->Draw();
	}
}
