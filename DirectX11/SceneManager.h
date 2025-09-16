#pragma once
#include <unordered_map>
#include "SceneBase.hpp"
#include "CameraBase.h"
#include "CameraController.h"
#include "Shader.h"
#include "LightBase.h"
#include "GameObject.h"
#include "IEffect.h"
#include "PBREffect.h"
#include "Player.h"
#include "GameSignalBus.h"
#include "UIBasicEffect.h"
#include "UIElement.h"
#include "UIBar.h"
#include "UIButton.h"
#include "Item.h"

namespace SceneConfig{
	enum SceneIndex :uint8_t
	{
		SCENE_TITLE = 1, 
		SCENE_GAME = 2,
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

	GameSignalBus* m_pGameSignalBus;
	CameraBase* m_pCurrentCamera;

private:

	SceneManager();
	~SceneManager() override;


	/// @brief シーンデータ初期化
	bool InitSceneMap();

	/// @brief ゲームリソース初期化
	bool InitResource();

	/// @brief Fx初期化
	bool InitEffect();


public:

	static SceneManager* Get() { static SceneManager instance; return &instance; };

	/// @brief 共通オブジェクトの読み込み・作成
	void Init() override;

	/// @brief D2D UIリソース初期化
	void InitD2DResource();

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

	void SetCurrentScene(const char* sceneName) override;

	void ChangeScene();

	void SetCurrentCamera(CameraBase* pCamera) { m_pCurrentCamera = pCamera; }

};

