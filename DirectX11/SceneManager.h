#pragma once
#include <unordered_map>
#include "SceneBase.hpp"
#include "CameraBase.h"
#include "Shader.h"
#include "LightBase.h"
#include "Cube.h"
#include "GameObject.h"
#include "IEffect.h"
#include "Model.h"
#include "SignalBus.h"

namespace SceneConfig{
	enum SceneIndex :uint8_t
	{
		SCENE_TITLE = 1, 
		SCENE_OPTION = 2,
		SCENE_GAME = 3,
		SCENE_TEST = 4,
		SCENE_NONE = 0,
	};
}

class SceneManager:public SceneBase
{
private:

	SceneConfig::SceneIndex m_currentSceneIndex;	//今はどのシーン
	typedef std::unordered_map<std::string, SceneConfig::SceneIndex> SceneMap;
	SceneMap m_sceneMap;	//シーンのリストを保存する

	bool isChangeScene;		//シーン切り替えたか？


private:

	std::shared_ptr<PixelShader> m_pBasicBlinnPhongPixelShader;
	std::shared_ptr<VertexShader> m_pBasicPosNormalTexVertexShader;
	std::shared_ptr<CameraBase> m_pFirstPersonCamera;
	std::shared_ptr<LightBase> lightBase;	

	//Model
	std::shared_ptr<Cube> cube;
	std::shared_ptr<Model> model;

	// Material
	std::shared_ptr<Material> m_pCubeMaterial;

	// Effect
	std::shared_ptr<BasicEffect> m_pBasicEffect;

	// Game Object
	std::unique_ptr<GameObject> m_pGameObject;

	std::shared_ptr<Texture> albedoTex;



	GameSignalBus* m_pGameSignalBus;
	CameraBase* m_pCurrentCamera;

private:

	SceneManager();
	~SceneManager() = default;

	/// @brief シーンデータ初期化
	bool InitSceneMap();

	/// @brief ゲームリソース初期化
	bool InitResource();

	/// @brief Fx初期化
	bool InitEffect();

	void SetCurrentCamera(CameraBase* camera);

public:

	static SceneManager* Get() { static SceneManager instance; return &instance; };

	/// @brief 共通オブジェクトの読み込み・作成
	void Init() override;

	/// @brief データ保存・リソースリリース
	void UnInit() override;

	/// @brief ここでゲーム全体の時間処理などを行う
	/// @param dt deltaTime
	void Update(float dt) override;

	/// @brief 描画
	void Draw() override;

	/// @brief SignalBus設定
	/// @param _signal 
	void SetSignalBus(GameSignalBus* _signal);

	
};

