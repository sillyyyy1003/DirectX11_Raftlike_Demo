#pragma once
#include <unordered_map>
#include "SceneBase.hpp"
#include "CameraBase.h"
#include "CameraController.h"
#include "Shader.h"
#include "LightBase.h"
#include "Cube.h"
#include "GameObject.h"
#include "IEffect.h"
#include "Model.h"
#include "PBREffect.h"
#include "Player.h"
#include "SignalBus.h"
#include "UIBasicEffect.h"
#include "UIElement.h"

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
	
	std::unique_ptr<Player> m_pPlayer;
	// Cube
	std::shared_ptr<PixelShader> m_pBasicBlinnPhongPixelShader;
	std::shared_ptr<VertexShader> m_pBasicPosNormalTexVertexShader;
	// PBR Model
	std::shared_ptr<PixelShader> m_pPBRPixelShader;
	std::shared_ptr<VertexShader> m_pPBRVertexShader;
	// UI Basic
	std::shared_ptr<PixelShader> m_pUIElementPixelShader;
	std::shared_ptr<VertexShader> m_pUIElementVertexShader;

	// Camera
	CameraBase* m_pCurrentCamera;
	std::shared_ptr<LightBase> lightBase;	

	// Model
	std::shared_ptr<Cube> cube;
	std::shared_ptr<Model> model;
	std::shared_ptr<SquareMesh> square;

	// Physical Component
	std::shared_ptr<PhysicsComponent> m_pAppleCollider;
	std::shared_ptr<PhysicsComponent> m_pCubeCollider;
	std::shared_ptr<PhysicsComponent> m_pFloorCollider;
	std::shared_ptr<PhysicsComponent> m_pPlayerCollider;

	std::shared_ptr<RenderComponent>  m_pDebugColliderRender;


	// Material
	std::shared_ptr<Material> m_pBlinnPhongMaterial;
	std::shared_ptr<Material> m_pDebugMaterial;
	std::shared_ptr<Material> m_pPBRFoodMaterial;
	std::shared_ptr<Material> m_pFloorMaterial;
	std::shared_ptr<Material> m_pUIMaterial;
	std::shared_ptr<Material> m_pUIAimMaterial;

	// Effect
	std::shared_ptr<BasicEffect> m_pBasicEffect;
	std::shared_ptr<BasicEffect> m_pDebugEffect;
	std::shared_ptr<PBREffect> m_pPBREffect;
	std::shared_ptr<UIBasicEffect> m_pUIBasicEffect;

	// Game Object
	std::unique_ptr<GameObject> m_pApple;
	std::unique_ptr<GameObject> m_pBanana;
	std::unique_ptr<GameObject> m_pCubeObject;
	std::unique_ptr<GameObject> m_pFloor;


	// Ui
	std::unique_ptr<UIElement> m_pUIElement;
	std::unique_ptr<UIElement> m_pUIAim;

	// Texture
	std::shared_ptr<Texture> albedoTex;
	std::shared_ptr<Texture> normalTex;
	std::shared_ptr<Texture> metallicTex;

	std::shared_ptr<Texture> m_pUiAimTex;



	// Uiに関するLib
	std::shared_ptr<UIFontSet> m_pUiFontSet;
	std::shared_ptr<UIBrush> m_pUiBrush;

	GameSignalBus* m_pGameSignalBus;
	ID2D1RenderTarget* d2dRenderTarget;


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

	/// @brief D2D UIリソース初期化
	void InitD2DResource(ID2D1RenderTarget* d2dRenderTarget, IDWriteFactory* writeFactory);

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

