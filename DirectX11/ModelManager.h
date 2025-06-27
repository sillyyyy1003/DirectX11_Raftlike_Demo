#pragma once
#include <unordered_map>
#include "Model.h"

/// @brief 全てのモデルを管理するクラス
class ModelManager
{
public:
	static ModelManager& Instance();
	Model* GetModel(const char* modelName);

	void LoadModel();
private:
	ModelManager()=default;
	~ModelManager() = default;

private:

	struct ModelData
	{
		std::shared_ptr<Model> model;
		std::string name;		// model name as map key
		std::string filePath;	// Load path for the model file
	};

	using Models = std::unordered_map<std::string, ModelData>;
	Models m_models; ///< モデルの名前とモデルデータのマップ

};

