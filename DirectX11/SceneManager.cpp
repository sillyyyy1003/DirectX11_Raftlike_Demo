#include "SceneManager.h"
#include "DirLight.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "KInput.h"
#include "PhysicsManager.h"
#include "ShapeFactory.h"


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
    //Player
    m_pPlayer = std::make_unique<Player>();
    m_pPlayer->Init();  //Player Controller
    m_pPlayer->GetCameraController()->GetCamera()->SetPos({ 0,10,-10 });
    m_pPlayer->GetCameraController()->GetCamera()->SetTarget({0,0,0});

    //現在使っているカメラとして設定
    SetCurrentCamera(m_pPlayer->GetCameraController()->GetCamera());

	//Scene作成




	//Base Light作成
    lightBase = std::make_shared<DirLight>();
    lightBase->SetPosition({ 0,10,0 });
    lightBase->SetAmbient({ 0.5f,0.5f,0.5f,1 });
    lightBase->SetDiffuse({ 0.5f,0.5f,0.5f,1.f });
	lightBase->SetIntensity(10.f);

    // 物理システムの初期化
    PhysicsManager::Instance().Init();

	//==========Material/Shader/Effect/Texture
    InitEffect();
    //==========Model/GameObject/UiObject/Physics Collider
    InitResource();
    //==========Scene map
    InitSceneMap();

}

void SceneManager::UnInit()
{
    Geometry::Uninit(); //Geometryの終了処理

	PhysicsManager::Instance().UnInit(); // 物理システムの終了処理
}

void SceneManager::Update(float dt)
{
    lightBase->Update(dt);
	PhysicsManager::Instance().Update(dt); // 物理システムの更新

    m_pApple->Update(dt);
	m_pCubeObject->Update(dt);
	m_pFloor->Update(dt);

    m_pPlayer->Update(dt);


	//m_pGameSignalBus->OnResolutionChangeRequest.Emit(Event::ResolutionPreset::R_1080p);
    
}

void SceneManager::Draw()
{
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

    m_pApple->Draw();
    m_pCubeObject->Draw();
    m_pFloor->Draw();
    m_pPlayer->Draw();

    //Ui描画
    m_pUIAim->DrawUi();

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

    square = std::make_shared<SquareMesh>();
    square->Init();

    //=====GameObjectの初期化
    m_pApple = std::make_unique<GameObject>();
    m_pApple->SetModel(model.get());
    m_pApple->SetMaterial(m_pPBRFoodMaterial.get());
    m_pApple->SetEffect(m_pPBREffect.get());

    m_pCubeObject = std::make_unique<GameObject>();
    m_pCubeObject->SetModel(cube.get());
    m_pCubeObject->SetMaterial(m_pBlinnPhongMaterial.get());
    m_pCubeObject->SetEffect(m_pBasicEffect.get());

    m_pFloor = std::make_unique<GameObject>();
    m_pFloor->SetModel(cube.get());
    m_pFloor->SetMaterial(m_pFloorMaterial.get());
    m_pFloor->SetEffect(m_pBasicEffect.get());


    m_pUIElement = std::make_unique<UIElement>(d2dRenderTarget);
    m_pUIElement->Init(m_pUIBasicEffect.get(), m_pUIMaterial.get(), square.get(), m_pUiFontSet.get(), "OptionFont", m_pUiBrush.get());
    m_pUIElement->SetPosition(0, 0);
    m_pUIElement->SetScale(200, 200);

    m_pUIAim = std::make_unique<UIElement>(d2dRenderTarget);
    m_pUIAim->Init(m_pUIBasicEffect.get(), m_pUIAimMaterial.get(), square.get());
    m_pUIAim->SetPosition(0, 0);
    m_pUIAim->SetScale(32, 32);

    //=====物理の初期化

    // Create the settings for the collision volume (the shape).

    // Create the shape

    // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.

    // Create the actual rigid body


    // Create BodyCreationSettings
    RefConst<Shape> defaultBoxSettings = ShapeFactory::Instance().GetOrCreateBox(RVec3(0.5f, 0.5f, 0.5f));
    BodyCreationSettings cubeBoxSettings(defaultBoxSettings, { -3,0.5f,0, }, Quat::sIdentity(), EMotionType::Kinematic, Layers::PLAYER);
    m_pCubeCollider = make_shared<PhysicsComponent>();
    m_pCubeCollider->Init(cubeBoxSettings, EActivation::Activate);  //Create& Add
    m_pCubeObject->AddComponent(MyComponent::ComponentType::Physics, m_pCubeCollider.get());
    m_pCubeObject->GetTransform().SetScale(1, 1, 1);

    BodyCreationSettings appleBoxSettings(new BoxShape(RVec3(0.25f, 0.25f, 0.25f)), { 3,3,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM);

    // Set the mass properties for the apple box
    PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings);

    m_pAppleCollider = make_shared<PhysicsComponent>();
    m_pAppleCollider->Init(appleBoxSettings, EActivation::Activate);  //Create& Add
    m_pApple->AddComponent(MyComponent::ComponentType::Physics, m_pAppleCollider.get());
    m_pApple->GetTransform().SetScale(0.1f, 0.1f, 0.1f);


    BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(5.f, 0.5f, 5.f)), { 0,-.5f,0, }, Quat::sIdentity(), EMotionType::Kinematic, Layers::BOAT);
    m_pFloorCollider = make_shared<PhysicsComponent>();
    m_pFloorCollider->Init(floorBoxSettings, EActivation::Activate);
    m_pFloor->AddComponent(MyComponent::ComponentType::Physics, m_pFloorCollider.get());
    m_pFloor->GetTransform().SetScale(10.f, .5f, 10.f);

    //Player Collider作成
    BodyCreationSettings playerBoxSettings(new BoxShape(RVec3(0.4, 0.8f, 0.4f)), { 0,0,0 }, Quat::sIdentity(), EMotionType::Kinematic, Layers::PLAYER);
    m_pPlayerCollider = make_shared<PhysicsComponent>();
    m_pPlayerCollider->Init(playerBoxSettings, EActivation::Activate);
    m_pPlayer->AddComponent(MyComponent::ComponentType::Physics, m_pPlayerCollider.get());
    m_pPlayer->SetDebugCollisionScale({ .8f,1.6f,.8f });

	//Collider Debug Render Component配置
    m_pDebugColliderRender = make_shared<RenderComponent>();
    m_pDebugColliderRender->SetEffect(m_pDebugEffect.get());
    m_pDebugColliderRender->SetMaterial(m_pDebugMaterial.get());
    m_pDebugColliderRender->SetModel(cube.get());

    m_pPlayer->AddComponent(MyComponent::ComponentType::DebugRender, m_pDebugColliderRender.get());




	//=====Geometryの初期化
	Geometry::Init();

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

    m_pDebugEffect = std::make_shared<BasicEffect>();
	m_pDebugEffect->InitPixelShader(m_pBasicBlinnPhongPixelShader.get());
	m_pDebugEffect->InitVertexShader(m_pBasicPosNormalTexVertexShader.get());
	m_pDebugEffect->InitCamera(m_pCurrentCamera);
	m_pDebugEffect->InitEffectDirLight(lightBase.get());



    //=======Init Texture
    albedoTex = std::make_shared<Texture>();
    normalTex = std::make_shared<Texture>();
    metallicTex = std::make_shared<Texture>();
    m_pUiAimTex = std::make_shared<Texture>();

    // Create texture from filepath
    albedoTex->Create("Assets/Texture/Foods_DefaultMaterial_AlbedoTransparency.png");
	normalTex->Create("Assets/Texture/Foods_DefaultMaterial_Normal.png");
	metallicTex->Create("Assets/Texture/Foods_DefaultMaterial_MetallicSmoothness.png");
    m_pUiAimTex->Create("Assets/Texture/UI/UI_Aim_128x128.png");
    

    //Init Material
    m_pBlinnPhongMaterial = std::make_shared<Material>();
    m_pBlinnPhongMaterial->SetAmbient({ 1,1,1,1 });
    m_pBlinnPhongMaterial->SetDiffuse({ 1,1,1,1.f });
    m_pBlinnPhongMaterial->SetSpecular({ 0,0,0,64 });

    m_pPBRFoodMaterial = std::make_shared<Material>();
    m_pPBRFoodMaterial->SetAmbient({ 1,1,1,1 });
    m_pPBRFoodMaterial->SetDiffuse({ 1,1,1,1.f });
    m_pPBRFoodMaterial->SetSpecular({ 0,0,0,0 });
    m_pPBRFoodMaterial->SetTexture(Material::Albedo, albedoTex.get());
    m_pPBRFoodMaterial->SetTexture(Material::Normal, normalTex.get());
    m_pPBRFoodMaterial->SetTexture(Material::Metallic_Smooth, metallicTex.get());

    m_pUIMaterial = std::make_shared<Material>();
    m_pUIMaterial->SetAmbient({ 1,1,1,1 });
    m_pUIMaterial->SetDiffuse({ 0,0,1,1 });
    m_pUIMaterial->SetSpecular({ 0,0,0,0 });

	m_pDebugMaterial = std::make_shared<Material>();
    m_pDebugMaterial->SetAmbient({ 1,1,1,1 });
    m_pDebugMaterial->SetDiffuse({ 0,1,0,0.2f });
    m_pDebugMaterial->SetSpecular({ 0,0,0,0 });

    m_pFloorMaterial = std::make_shared<Material>();
	m_pFloorMaterial->SetAmbient({ 1,1,1,1 });
	m_pFloorMaterial->SetDiffuse({ 0.2f,0.2f,0.6f,0.4f });
    m_pFloorMaterial->SetSpecular({ 0,0,0,0 });

    m_pUIAimMaterial = std::make_shared<Material>();
    m_pUIAimMaterial->SetDefaultMaterial();
    m_pUIAimMaterial->SetTexture(Material::Albedo, m_pUiAimTex.get());

	return true;
}

void SceneManager::SetCurrentCamera(CameraBase* camera)
{
    m_pCurrentCamera = camera;
}
