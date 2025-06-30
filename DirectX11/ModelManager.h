#pragma once
#include <unordered_map>
#include "Model.h"
#include <json.hpp>

/// @brief �S�Ẵ��f�����Ǘ�����N���X
class ModelManager
{
public:
	static ModelManager& Instance();
	Model* GetModel(const char* modelName);

	/// @brief ���f�������[�h����
	///	@param modelName ���f���̖��O
	/// @param filePath ���f���̃t�@�C���p�X
	void LoadModel(const char* modelName, const char* filePath);

	/// @brief Json File���烂�f�������[�h����
	/// @param jsonFilePath 
	/// @return true: ����, false: ���s 
	bool LoadModels(const char* jsonFilePath);

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
	Models m_models; ///< ���f���̖��O�ƃ��f���f�[�^�̃}�b�v

};

