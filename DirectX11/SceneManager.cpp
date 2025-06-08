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


	//BaseCamera作成
    m_pFirstPersonCamera = std::make_shared<FirstPersonCamera>();
    m_pFirstPersonCamera->SetPos({ 0,0,-10 });
    //現在使っているカメラとして設定
    SetCurrentCamera(m_pFirstPersonCamera.get());


	//Base Light作成
    lightBase = std::make_shared<DirLight>();
    lightBase->SetPosition({ 0,10,0 });

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
    m_pCurrentCamera->Update(dt);
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



}

void SceneManager::SetSignalBus(GameSignalBus* _signal)
{
    m_pGameSignalBus = _signal;
}

bool SceneManager::InitResource()
{
	//====test
	cube = std::make_shared<Cube>();
	cube->Init();
    model = std::make_shared<Model>();
    model->Init("Assets/Model/Apple.fbx");

    m_pGameObject = std::make_unique<GameObject>();
    m_pGameObject->SetModel(model.get());
    m_pGameObject->SetMaterial(m_pCubeMaterial.get());
    m_pGameObject->SetEffect(m_pBasicEffect.get());

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

    m_pBasicPosNormalTexVertexShader->Load("Assets/Shader/VS_Geometry.cso");
    m_pBasicBlinnPhongPixelShader->Load("Assets/Shader/PS_Blinn-Phong.cso");

    //=======Init Effect
    m_pBasicEffect = std::make_shared<BasicEffect>();
    m_pBasicEffect->InitPixelShader(m_pBasicBlinnPhongPixelShader.get());
    m_pBasicEffect->InitVertexShader(m_pBasicPosNormalTexVertexShader.get());
    m_pBasicEffect->InitCamera(m_pCurrentCamera);
    m_pBasicEffect->InitEffectDirLight(lightBase.get());

    //=======Init Texture
    albedoTex = std::make_shared<Texture>();
    albedoTex->Create("Assets/Texture/Foods_DefaultMaterial_AlbedoTransparency.png");
    

    //Init Material
    m_pCubeMaterial = std::make_shared<Material>();
    m_pCubeMaterial->SetAmbient({ 1,1,1,1 });
    m_pCubeMaterial->SetDiffuse({ 1,1,1,1.f });
    m_pCubeMaterial->SetSpecular({ 0,0,0,64 });
    m_pCubeMaterial->SetTexture(Material::Albedo, albedoTex.get());
	return true;
}

void SceneManager::SetCurrentCamera(CameraBase* camera)
{
    m_pCurrentCamera = camera;
}
