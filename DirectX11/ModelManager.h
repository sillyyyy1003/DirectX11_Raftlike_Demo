﻿#pragma once
#include <unordered_map>
#include "Model.h"


/// @brief 全てのモデルを管理するクラス
class ModelManager
{
public:
	static ModelManager& Instance();
	Primitive* GetModel(const char* modelName);

	/// @brief モデルをロードする
	///	@param modelName モデルの名前
	/// @param filePath モデルのファイルパス
	void LoadModel(const char* modelName, const char* filePath);

	/// @brief Json Fileからモデルをロードする
	/// @param jsonFilePath 
	/// @return true: 成功, false: 失敗 
	bool LoadModels(const char* jsonFilePath);

	/// @brief Load Model from Program
	/// @param modelName Model name
	/// @param model shared pointer to Model
	void LoadModel(const char* modelName, const std::shared_ptr<Primitive>& model);

	void UnInit();
private:
	ModelManager()=default;
	~ModelManager() = default;

private:

	struct ModelData
	{
		std::shared_ptr<Primitive> model;
		std::string name;		// model name as map key
		std::string filePath;	// Load path for the model file
	};

	using Models = std::unordered_map<std::string, ModelData>;
	Models m_models; ///< モデルの名前とモデルデータのマップ

};

