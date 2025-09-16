#include "SceneManager.h"

#include "AudioManager.h"
#include "Capsule.h"
#include "DirLight.h"
#include "GameApp.h"
#include "Geometry.h"
#include "ItemDataBase.h"
#include "KInput.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "PhysicsManager.h"
#include "RenderState.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "ShapeFactory.h"
#include "Skybox.h"
#include "UIManager.h"
#include "SkyboxEffect.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "UIMenu.h"

namespace
{
    static constexpr UINT TextureCubeSize = 1024;
}


SceneManager::SceneManager():
m_currentSceneIndex(SceneConfig::SCENE_NONE),
isChangeScene(false),
m_pGameSignalBus(nullptr)
{
}

SceneManager::~SceneManager()
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
	return true;
}

void SceneManager::Init()
{
    // 物理システムの初期化
    PhysicsManager::Instance().Init();

	//Base Light作成
	DirLight* systemLight = CreateObj<DirLight>("SystemLight");
    systemLight->SetPosition({ 0,10,0 });
    systemLight->SetAmbient({ 0.5f,0.5f,0.5f,1 });
    systemLight->SetDiffuse({ 0.5f,0.5f,0.5f,1.f });
	systemLight->SetIntensity(10.f);

	// Load models
    ModelManager::Instance().LoadModels("Assets/ConfigFile/Config.json");


    // ======Audio
    AudioManager::Instance().Init();
    AudioManager::Instance().LoadAudio("SE_WaveBackGround", L"Assets/Sound/SE/SeaBackGround.wav", AudioManager::AudioType::ENVIRONMENT);
    AudioManager::Instance().LoadAudio("SE_Button", L"Assets/Sound/SE/ButtonClick.wav", AudioManager::AudioType::SE);
    AudioManager::Instance().LoadAudio("SE_Drink", L"Assets/Sound/SE/Drink.wav", AudioManager::AudioType::SE);
    AudioManager::Instance().LoadAudio("BGM_Title", L"Assets/Sound/BGM/BGM1.wav", AudioManager::AudioType::BGM);
    AudioManager::Instance().LoadAudio("BGM_Game", L"Assets/Sound/BGM/BGM2.wav", AudioManager::AudioType::BGM);

	//==========Shader
    InitEffect();

    //==========Model
    InitResource();

    //==========Scene map
    InitSceneMap();

    //==========Init Menu UI

    PixelShader* uiElementPS = GetObj<PixelShader>("UIElementPS");
    VertexShader* uiElementVS = GetObj<VertexShader>("UIElementVS");
    UIBasicEffect* uiBasicEffect = CreateObj<UIBasicEffect>("UiBasicEffect");
    uiBasicEffect->InitPixelShader(uiElementPS);
    uiBasicEffect->InitVertexShader(uiElementVS);

    //==============Init Text Font& Brush
    UIFontSet* uiFontSet = GetObj<UIFontSet>("UIFontSet");
    UIBrush* uiBrush = GetObj<UIBrush>("UiBrush");

    //==============Init Tex
    Texture* leftArrowTex = TextureManager::Instance().GetTexture("UI_LeftArrow");
    Texture* rightArrowTex = TextureManager::Instance().GetTexture("UI_RightArrow");


    //==============Init Material
    Material* uiMenuMat = MaterialManager::Instance().GetMaterial("UiMenuMaterial");
    Material* uiSoundBarMat = MaterialManager::Instance().GetMaterial("UiBarMaterial");
    Material* uiSoundBarBackGroundMat = MaterialManager::Instance().GetMaterial("UiBarBgMaterial");
    Material* resumeMat = MaterialManager::Instance().GetMaterial("UiMenuResumeButtonMaterial");
    Material* backToTileMat = MaterialManager::Instance().GetMaterial("UiMenuBackToTitleButton");

    Material* bgmLeftArrowMat = CreateObj<Material>("BgmLeftArrowMat");
    bgmLeftArrowMat->SetTexture(Material::Albedo, leftArrowTex);
    Material* bgmRightArrowMat = CreateObj<Material>("BgmRightArrowMat");
    bgmRightArrowMat->SetTexture(Material::Albedo, rightArrowTex);

    Material* seLeftArrowMat = CreateObj<Material>("SeLeftArrowMat");
    seLeftArrowMat->SetTexture(Material::Albedo, leftArrowTex);
    Material* seRightArrowMat = CreateObj<Material>("SeRightArrowMat");
    seRightArrowMat->SetTexture(Material::Albedo, rightArrowTex);

    Material* environLeftArrowMat = CreateObj<Material>("EnvironLeftArrowMat");
    environLeftArrowMat->SetTexture(Material::Albedo, leftArrowTex);
    Material* environRightArrowMat = CreateObj<Material>("EnvironRightArrowMat");
    environRightArrowMat->SetTexture(Material::Albedo, rightArrowTex);


    //UIManager::Instance().ClearLayers();


    //==============Init Effect
    UIMenu::MenuResource resource = {
        uiBasicEffect,
        uiFontSet,
        uiBrush,
        uiMenuMat,
        uiSoundBarMat,
        uiSoundBarBackGroundMat,
        bgmLeftArrowMat,
        bgmRightArrowMat,
        seLeftArrowMat,
        seRightArrowMat,
        environLeftArrowMat,
        environRightArrowMat,
        resumeMat,
        backToTileMat,
        ModelManager::Instance().GetModel("Square")
    };

    UIMenu* uiMenu = CreateObj<UIMenu>("UIMenu");
    uiMenu->Init(resource);
   //uiMenu->SetButton(this);

    SetCurrentScene("Title");
  

}

void SceneManager::UnInit()
{
    Geometry::Uninit(); //Geometryの終了処理
    UIManager::Instance().UnInit();

    ItemDataBase::Instance().UnInit();
    MaterialManager::Instance().UnInit(); // マテリアルマネージャーの終了処理
	ModelManager::Instance().UnInit(); // モデルマネージャーの終了処理
	TextureManager::Instance().UnInit(); // テクスチャマネージャーの終了処理
	PhysicsManager::Instance().UnInit(); // 物理システムの終了処理
}

void SceneManager::Update(float dt)
{

	//===============UI Update
    UIManager::Instance().Update(dt);
}

void SceneManager::Draw()
{
    if (m_currentSceneIndex==SceneConfig::SCENE_GAME){
        GameApp::SetDepthStencilState(RenderStates::DSSLessEqual);
#if defined(_DEBUG) || defined(DEBUG)
        //Geometry
        DirectX::XMFLOAT4X4 fmat;
        DirectX::XMStoreFloat4x4(&fmat, DirectX::XMMatrixIdentity());
        Geometry::SetWorld(fmat);
        Geometry::SetView(m_pCurrentCamera->GetViewXMF());
        Geometry::SetProjection(m_pCurrentCamera->GetProjXMF());
        const int GridSize = 10;
        Geometry::SetColor(DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 0.5f));
        for (int i = 1; i <= GridSize; ++i)
        {
            float g = (float)i;
            Geometry::AddLine(DirectX::XMFLOAT3(g, 0.0f, -GridSize), DirectX::XMFLOAT3(g, 0.0f, GridSize));
            Geometry::AddLine(DirectX::XMFLOAT3(-g, 0.0f, -GridSize), DirectX::XMFLOAT3(-g, 0.0f, GridSize));
            Geometry::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, g), DirectX::XMFLOAT3(GridSize, 0.0f, g));
            Geometry::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, -g), DirectX::XMFLOAT3(GridSize, 0.0f, -g));
        }
        // 軸描画
        Geometry::SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
        Geometry::AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(GridSize, 0.0f, 0.0f));
        Geometry::SetColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
        Geometry::AddLine(DirectX::XMFLOAT3(0.0f, -GridSize, 0.0f), DirectX::XMFLOAT3(0.0f, GridSize, 0.0f));
        Geometry::SetColor(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
        Geometry::AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, GridSize));

        Geometry::DrawLines();
#endif
    }

    // Ui描画
    GameApp::SetDepthStencilState(RenderStates::DSSNoDepthTest);
    UIManager::Instance().Draw();
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
    switch(m_currentSceneIndex)
    {
    case SceneConfig::SCENE_TITLE:AddSubScene<SceneTitle>();
        DebugLog::Log("[SceneManager] Current Scene: Scene title!");
        break;
    case SceneConfig::SCENE_GAME:AddSubScene<SceneGame>();
        DebugLog::Log("[SceneManager] Current Scene: Scene game!");
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

	//====Textureの読み込み
	//Skyboxのテクスチャを作成
    Texture* dayLight0 = CreateObj<Texture>("DayLight0");
    Texture* dayLight1 = CreateObj<Texture>("DayLight1");
    Texture* dayLight2 = CreateObj<Texture>("DayLight2");
    Texture* dayLight3 = CreateObj<Texture>("DayLight3");
    Texture* dayLight4 = CreateObj<Texture>("DayLight4");
    Texture* dayLight5 = CreateObj<Texture>("DayLight5");
    dayLight0->Create("Assets/Texture/Skybox/daylight0.png");
    dayLight1->Create("Assets/Texture/Skybox/daylight1.png");
    dayLight2->Create("Assets/Texture/Skybox/daylight2.png");
    dayLight3->Create("Assets/Texture/Skybox/daylight3.png");
    dayLight4->Create("Assets/Texture/Skybox/daylight4.png");
    dayLight5->Create("Assets/Texture/Skybox/daylight5.png");
    std::vector<Texture*> textures;
    textures.push_back(dayLight0);
    textures.push_back(dayLight1);
    textures.push_back(dayLight2);
    textures.push_back(dayLight3);
    textures.push_back(dayLight4);
    textures.push_back(dayLight5);

	//Skyboxのテクスチャキューブを作成
    TextureCube* textureCube = CreateObj<TextureCube>("TextureCube");
    textureCube->CreateTextureCube(TextureCubeSize, TextureCubeSize, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
    textureCube->CopyTextures(textures);



    //=====Skyboxの初期化
	// SkyboxEffectの初期化
    VertexShader* skyboxVS = GetObj<VertexShader>("SkyboxVS");
    PixelShader* skyboxPS = GetObj<PixelShader>("SkyboxPS");
    SkyboxEffect* skyboxEffect = CreateObj<SkyboxEffect>("SkyboxEffect");
    skyboxEffect->InitPixelShader(skyboxPS);
    skyboxEffect->InitVertexShader(skyboxVS);
    skyboxEffect->InitTexture(textureCube);

	// Init Skybox
    Skybox* skybox = CreateObj<Skybox>("Skybox");
    skybox->Init(skyboxEffect);


	//=====Geometryの初期化
	Geometry::Init();


	//=====Textureの初期化
    TextureManager::Instance().LoadTextures("Assets/ConfigFile/Config.json");

    //=====Materialの初期化
    MaterialManager::Instance().RegisterMaterials("Assets/ConfigFile/Config.json");

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
    VertexShader* waterVS = CreateObj<VertexShader>("WaterVS");
    PixelShader* waterPS = CreateObj<PixelShader>("WaterPS");
 


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
    waterVS->Load("Assets/Shader/VS_Water.cso");
    waterPS->Load("Assets/Shader/PS_Water.cso");

	return true;
	
}
