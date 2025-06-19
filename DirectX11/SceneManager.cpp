#include "SceneManager.h"
#include "DirLight.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "KInput.h"


SceneManager::SceneManager():
isChangeScene(false),
m_currentSceneIndex(SceneConfig::SCENE_NONE),
m_pCurrentCamera(nullptr),
m_pGameSignalBus(nullptr)
{
}

void SceneManager::InitD2DResource(ID2D1RenderTarget* _d2dRenderTarget, IDWriteFactory* _writeFactory)
{
    d2dRenderTarget = _d2dRenderTarget;

    m_pUiFontSet = std::make_shared<UIFontSet>();
    m_pUiBrush = std::make_shared<UIBrush>();

    m_pUiFontSet->InitFontList();   // Textデータ設定
    m_pUiFontSet->CreateTextFormat(d2dRenderTarget, _writeFactory);   //Textフォーマット設定

    m_pUiBrush->InitBrush(d2dRenderTarget);
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
	//Scene作成


	//観察用Camera作成
    m_pObserverCamera = std::make_shared<FirstPersonCamera>();
    m_pObserverCamera->SetPos({ 0,10,-10 });
	m_pObserverCamera->SetTarget({ 0,0,0 });

	//CameraController作成
	m_pCameraController = std::make_shared<CameraController>();
	m_pCameraController->GetCamera()->SetPos({ 0,10,-10 });
	m_pCameraController->GetCamera()->SetTarget({ 0,0,0 });

    //現在使っているカメラとして設定
	//SetCurrentCamera(m_pObserverCamera.get());
    SetCurrentCamera(m_pCameraController->GetCamera());


	//Base Light作成
    lightBase = std::make_shared<DirLight>();
    lightBase->SetPosition({ 0,10,0 });
    lightBase->SetAmbient({ 0.5f,0.5f,0.5f,1 });
    lightBase->SetDiffuse({ 0.5f,0.5f,0.5f,1.f });
	lightBase->SetIntensity(10.f);

	//Init Resource
    InitEffect();
    InitResource();
    InitSceneMap();

}

void SceneManager::UnInit()
{
    Geometry::Uninit();
}

void SceneManager::Update(float dt)
{

    lightBase->Update(dt);
	m_pCameraController->Update(dt);  // カメラコントローラーの更新

    m_pGameObject->Update(dt);



	//m_pGameSignalBus->OnResolutionChangeRequest.Emit(Event::ResolutionPreset::R_1080p);
    
}

void SceneManager::Draw()
{
#ifdef _DEBUG
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

    m_pGameObject->Draw();
    m_pUIElement->DrawUi("Test");

}

void SceneManager::SetSignalBus(GameSignalBus* _signal)
{
    m_pGameSignalBus = _signal;
}


bool SceneManager::InitResource()
{
	//====モデルの読み込み
	cube = std::make_shared<Cube>();
	cube->Init();

	model = std::make_shared<Model>();
    model->Init("Assets/Model/Apple.fbx");

	square = std::make_shared<Square>();
	square->Init();

	//=====GameObjectの初期化
    m_pGameObject = std::make_unique<GameObject>();
    m_pGameObject->SetModel(model.get());
	m_pGameObject->SetMaterial(m_pPBRModelMaterial.get());
    m_pGameObject->SetEffect(m_pPBREffect.get());
    m_pGameObject->GetTransform().SetScale(0.1f, 0.1f, 0.1f);

    m_pUIElement = std::make_unique<UIElement>(d2dRenderTarget);
    m_pUIElement->Init(m_pUIBasicEffect.get(), m_pUIMaterial.get(), square.get(), m_pUiFontSet.get(), "OptionFont", m_pUiBrush.get());
    m_pUIElement->SetPosition(0,0);
    m_pUIElement->SetScale(200, 200);

	Geometry::Init();
	//======

	//モデルの読み込み

	//ゲームセーブデータの読み込み
    return true;
}

bool SceneManager::InitEffect()
{
    //=======Init shader
	m_pBasicPosNormalTexVertexShader = std::make_shared<VertexShader>();
	m_pBasicBlinnPhongPixelShader = std::make_shared<PixelShader>();
    m_pPBRPixelShader = std::make_shared<PixelShader>();
	m_pPBRVertexShader = std::make_shared<VertexShader>();
    m_pUIElementPixelShader = std::make_shared<PixelShader>();
	m_pUIElementVertexShader = std::make_shared<VertexShader>();

	//=======Load shader
    m_pBasicPosNormalTexVertexShader->Load("Assets/Shader/VS_Geometry.cso");
    m_pBasicBlinnPhongPixelShader->Load("Assets/Shader/PS_Blinn-Phong.cso");
	m_pPBRPixelShader->Load("Assets/Shader/PS_PBR.cso");
	m_pPBRVertexShader->Load("Assets/Shader/VS_PBR.cso");
    m_pUIElementPixelShader->Load("Assets/Shader/PS_UIElement.cso");
    m_pUIElementVertexShader->Load("Assets/Shader/VS_UIElement.cso");

    //=======Init Effect
    m_pBasicEffect = std::make_shared<BasicEffect>();
    m_pBasicEffect->InitPixelShader(m_pBasicBlinnPhongPixelShader.get());
    m_pBasicEffect->InitVertexShader(m_pBasicPosNormalTexVertexShader.get());
    m_pBasicEffect->InitCamera(m_pCurrentCamera);
    m_pBasicEffect->InitEffectDirLight(lightBase.get());

	m_pPBREffect = std::make_shared<PBREffect>();
    m_pPBREffect->InitPixelShader(m_pPBRPixelShader.get());
	m_pPBREffect->InitVertexShader(m_pPBRVertexShader.get());
	m_pPBREffect->InitCamera(m_pCurrentCamera);
	m_pPBREffect->InitEffectDirLight(lightBase.get());

    m_pUIBasicEffect = std::make_shared<UIBasicEffect>();
    m_pUIBasicEffect->InitPixelShader(m_pUIElementPixelShader.get());
    m_pUIBasicEffect->InitVertexShader(m_pUIElementVertexShader.get());



    //=======Init Texture
    albedoTex = std::make_shared<Texture>();
    normalTex = std::make_shared<Texture>();
    metallicTex = std::make_shared<Texture>();

    albedoTex->Create("Assets/Texture/Foods_DefaultMaterial_AlbedoTransparency.png");
	normalTex->Create("Assets/Texture/Foods_DefaultMaterial_Normal.png");
	metallicTex->Create("Assets/Texture/Foods_DefaultMaterial_MetallicSmoothness.png");
    

    //Init Material
    m_pBlinnPhongMaterial = std::make_shared<Material>();
    m_pBlinnPhongMaterial->SetAmbient({ 1,1,1,1 });
    m_pBlinnPhongMaterial->SetDiffuse({ 1,1,1,1.f });
    m_pBlinnPhongMaterial->SetSpecular({ 0,0,0,64 });
    m_pBlinnPhongMaterial->SetTexture(Material::Albedo, albedoTex.get());

    m_pPBRModelMaterial = std::make_shared<Material>();
    m_pPBRModelMaterial->SetAmbient({ 1,1,1,1 });
    m_pPBRModelMaterial->SetDiffuse({ 1,1,1,1.f });
    m_pPBRModelMaterial->SetSpecular({ 0,0,0,0 });
    m_pPBRModelMaterial->SetTexture(Material::Albedo, albedoTex.get());
    m_pPBRModelMaterial->SetTexture(Material::Normal, normalTex.get());
    m_pPBRModelMaterial->SetTexture(Material::Metallic_Smooth, metallicTex.get());

    m_pUIMaterial = std::make_shared<Material>();
    m_pUIMaterial->SetAmbient({ 1,1,1,1 });
    m_pUIMaterial->SetDiffuse({ 0,0,1,1 });
    m_pUIMaterial->SetSpecular({ 0,0,0,0 });

	return true;
}

void SceneManager::SetCurrentCamera(CameraBase* camera)
{
    m_pCurrentCamera = camera;
}
