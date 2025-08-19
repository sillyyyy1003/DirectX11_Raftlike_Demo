#pragma once
#include <unordered_map>
#include "Model.h"


/// @brief 全てのモデルを管理するクラス
class ModelManager
{
public:
	typedef uint32_t ModelID; ///< モデルのID

	static ModelManager& Instance();

    /// @brief 名前からモデルを取得
    Primitive* GetModel(const char* modelName);
	/// @brief IDからモデルを取得
    Primitive* GetModel(ModelID id);

    /// @brief 名前からIDを取得（存在しなければ InvalidId）
    ModelID GetModelId(const std::string& name) const;

    /// @brief 名前からIDを取得（存在しなければ作成）
    ModelID GetOrCreateModelId(const std::string& name);

    /// @brief モデルをロードする（ファイルから）
    void LoadModel(const char* modelName, const char* filePath);

    /// @brief モデルをロードする（外部生成済み）
    void LoadModel(const char* modelName, const std::shared_ptr<Primitive>& model);

    /// @brief Json Fileからモデルをロードする
    bool LoadModels(const char* jsonFilePath);

    /// @brief リソース解放
    void UnInit();

    static constexpr ModelID InvalidId = -1;

private:

	ModelManager()=default;
	~ModelManager() = default;

private:

	using Models = std::unordered_map<ModelID, std::shared_ptr<Primitive>>;
	using ModelList = std::unordered_map<std::string, ModelID>;

	ModelList m_modelList;	///< モデルの名前とIDのマップ
	Models m_models;		///< モデルのIDとモデルデータのマップ

	ModelID m_nextId = 0; ///< 次に割り当てるモデルID

};

