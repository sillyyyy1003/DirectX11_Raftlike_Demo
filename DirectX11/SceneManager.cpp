#include "SceneManager.h"
#include "Capsule.h"
#include "DirLight.h"
#include "Geometry.h"
#include "ItemDataBase.h"
#include "KInput.h"
#include "ModelManager.h"
#include "PhysicsManager.h"
#include "SceneGame.h"
#include "SceneOption.h"
#include "SceneTitle.h"
#include "ShapeFactory.h"
#include "UIManager.h"


SceneManager::SceneManager():
m_currentSceneIndex(SceneConfig::SCENE_NONE),
isChangeScene(false),
m_pGameSignalBus(nullptr)
{
}

void SceneManager::InitD2DResource()
{
    UIFontSet* uiFonts = CreateObj<UIFontSet>("UIFontSet");
    uiFonts->InitFontList();
    uiFonts->LoadFontList("Assets/ConfigFile/Font.json");
    uiFonts->CreateTextFormat(gD3D->GetD2DRenderTarget(), gD3D->GetWriteFactory());

    UIBrush* uiBrush = CreateObj<UIBrush>("UiBrush");
    uiBrush->InitBrush(gD3D->GetD2DRenderTarget());

}

bool SceneManager::InitSceneMap()
{
	m_sceneMap["Title"] = SceneConfig::SCENE_TITLE;
	m_sceneMap["Game"] = SceneConfig::SCENE_GAME;
	m_sceneMap["Option"] = SceneConfig::SCENE_OPTION;
	m_sceneMap["Test"] = SceneConfig::SCENE_TEST;
	return true;
}

void SceneManager::Init()
{
    // 物理システムの初期化
    PhysicsManager::Instance().Init();

	//Base Light作成
    lightBase = std::make_shared<DirLight>();
    lightBase->SetPosition({ 0,10,0 });
    lightBase->SetAmbient({ 0.5f,0.5f,0.5f,1 });
    lightBase->SetDiffuse({ 0.5f,0.5f,0.5f,1.f });
	lightBase->SetIntensity(10.f);

	// Load models
    ModelManager::Instance().LoadModels("Assets/ConfigFile/Config.json");

	//==========Shader
    InitEffect();

    //==========Model
    InitResource();

    //==========Scene map
    InitSceneMap();

    SetCurrentScene("Title");

}

void SceneManager::UnInit()
{
    Geometry::Uninit(); //Geometryの終了処理

	ModelManager::Instance().UnInit(); // モデルマネージャーの終了処理

	PhysicsManager::Instance().UnInit(); // 物理システムの終了処理
}

void SceneManager::Update(float dt)
{
    lightBase->Update(dt);

}

void SceneManager::Draw()
{

}

void SceneManager::SetSignalBus(GameSignalBus* _signal)
{
    m_pGameSignalBus = _signal;
}

void SceneManager::SetCurrentScene(const char* sceneName)
{
    if (m_sceneMap.find(std::string(sceneName)) != m_sceneMap.end())
    {
        m_currentSceneIndex = m_sceneMap[std::string(sceneName)];
        ChangeScene();
    }
}

void SceneManager::ChangeScene()
{
    //clear all layers for next scene
    UIManager::GetInstance().ClearLayers();

    switch(m_currentSceneIndex)
    {
    case SceneConfig::SCENE_TITLE:AddSubScene<SceneTitle>();
        DebugLog::Log("[SceneManager] Current Scene: Scene title!");
        break;
    case SceneConfig::SCENE_GAME:AddSubScene<SceneGame>();
        DebugLog::Log("[SceneManager] Current Scene: Scene game!");
        break;
    case SceneConfig::SCENE_OPTION:
        AddSubScene<SceneOption>();
        DebugLog::Log("[SceneManager] Current Scene: Scene option!");
        break;
    }
}


bool SceneManager::InitResource()
{
    //====モデル作成
    std::shared_ptr<Cube> cube = std::make_shared<Cube>();
	cube->Init();
    ModelManager::Instance().LoadModel("Cube",cube); //Add Model to Model Manager as an instance

    std::shared_ptr<SquareMesh> square = std::make_shared<SquareMesh>();
    square->Init();
    ModelManager::Instance().LoadModel("Square", square);   //Add Model to Model Manager as an instance 

    std::shared_ptr<Capsule> capsule = std::make_shared<Capsule>();
	capsule->Init(16, 16,16);
	ModelManager::Instance().LoadModel("Capsule", capsule); //Add Model to Model Manager as an instance

	//=====Geometryの初期化
	Geometry::Init();

	//ゲームセーブデータの読み込み

    return true;
}

bool SceneManager::InitEffect()
{
    //=======Init shader
    VertexShader* basicPosNormalTexVS = CreateObj<VertexShader>("BasicPosNormalTexVS");
	PixelShader* basicBlinnPhongPS = CreateObj<PixelShader>("BasicBlinnPhongPS");
	PixelShader* pbrPS = CreateObj<PixelShader>("PBRPS");
	VertexShader* pbrVS = CreateObj<VertexShader>("PBRVS");
	PixelShader* uiElementPS = CreateObj<PixelShader>("UIElementPS");
    VertexShader* uiElementVS = CreateObj<VertexShader>("UIElementVS");
    PixelShader* uiGlossPS = CreateObj<PixelShader>("UIGlossPS");
    VertexShader* skyboxVS = CreateObj<VertexShader>("SkyboxVS");
    PixelShader* skyboxPS = CreateObj<PixelShader>("SkyboxPS");


	//=======Load shader
    basicPosNormalTexVS->Load("Assets/Shader/VS_Geometry.cso");
    basicBlinnPhongPS->Load("Assets/Shader/PS_Blinn-Phong.cso");
    pbrPS->Load("Assets/Shader/PS_PBR.cso");
    pbrVS->Load("Assets/Shader/VS_PBR.cso");
    uiElementPS->Load("Assets/Shader/PS_UIElement.cso");
    uiElementVS->Load("Assets/Shader/VS_UIElement.cso");
    uiGlossPS->Load("Assets/Shader/PS_UIGloss.cso");
	skyboxVS->Load("Assets/Shader/VS_Skybox.cso");
	skyboxPS->Load("Assets/Shader/PS_Skybox.cso");

	return true;
	
}
