#include "SceneManager.h"
#include "Capsule.h"
#include "DirLight.h"
#include "Geometry.h"
#include "ItemDataBase.h"
#include "KInput.h"
#include "ModelManager.h"
#include "PhysicsManager.h"
#include "ShapeFactory.h"
#include "UIManager.h"


SceneManager::SceneManager():
m_currentSceneIndex(SceneConfig::SCENE_NONE),
isChangeScene(false),
m_pCurrentCamera(nullptr),
m_pGameSignalBus(nullptr)
{
}

void SceneManager::InitD2DResource()
{
    m_pUiFontSet = std::make_shared<UIFontSet>();
    m_pUiBrush = std::make_shared<UIBrush>();

    //m_pUiFontSet->InitFontList();   // Textデータ設定
    m_pUiFontSet->LoadFontList("Assets/ConfigFile/Font.json");
    m_pUiFontSet->CreateTextFormat(gD3D->GetD2DRenderTarget(), gD3D->GetWriteFactory());   //Textフォーマット設定

    m_pUiBrush->InitBrush(gD3D->GetD2DRenderTarget());
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

    //Player
    m_pPlayer = std::make_shared<Player>();
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

	// Load models
    ModelManager::Instance().LoadModels("Assets/ConfigFile/Config.json");

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

	ModelManager::Instance().UnInit(); // モデルマネージャーの終了処理

	PhysicsManager::Instance().UnInit(); // 物理システムの終了処理
}

void SceneManager::Update(float dt)
{
    lightBase->Update(dt);
	PhysicsManager::Instance().Update(dt); // 物理システムの更新

	m_pCubeObject->Update(dt);
	m_pFloor->Update(dt);

    m_appleInstance->Update(dt);
    m_appleInstance1->Update(dt);

	m_pPlayer->Update(dt);

	UIManager::GetInstance().Update(dt); //UIManagerの更新処理
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

    m_pCubeObject->Draw();
    m_pFloor->Draw();
    m_pPlayer->Draw();
    m_appleInstance->Draw();
    m_appleInstance1->Draw();

    //Ui描画
    m_pUiAim->Draw();
    m_pUiBar->Draw();

    //m_pUIElement->Draw();
    UIManager::GetInstance().Draw();
}

void SceneManager::SetSignalBus(GameSignalBus* _signal)
{
    m_pGameSignalBus = _signal;
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


    //=====GameObjectの初期化
    std::shared_ptr<RenderComponent> appleRenderComponent = std::make_shared<RenderComponent>();
    appleRenderComponent->SetModel(ModelManager::Instance().GetModel("Food_Apple"));
    appleRenderComponent->SetMaterial(m_pPBRFoodMaterial.get());
    appleRenderComponent->SetEffect(m_pPBREffect.get());

    std::shared_ptr<Food> apple = std::make_shared<Food>(20.f);
    ItemDataBase::Instance().RegisterItem("Apple", apple);

    m_appleInstance = make_unique<ItemInstance>(ItemDataBase::Instance().GetItem("Apple"), 5);
    m_appleInstance->AddComponent(MyComponent::ComponentType::Render, appleRenderComponent);

    m_appleInstance1 = make_unique<ItemInstance>(ItemDataBase::Instance().GetItem("Apple"), 2);
    m_appleInstance1->AddComponent(MyComponent::ComponentType::Render, appleRenderComponent);

    m_pCubeObject = std::make_unique<GameObject>();
	std::shared_ptr<RenderComponent> cubeRenderComponent = std::make_shared<RenderComponent>();
	m_pCubeObject->AddComponent(MyComponent::ComponentType::Render, cubeRenderComponent);
    cubeRenderComponent->SetModel(ModelManager::Instance().GetModel("Cube"));
    cubeRenderComponent->SetMaterial(m_pBlinnPhongMaterial.get());
    cubeRenderComponent->SetEffect(m_pBasicEffect.get());

    m_pFloor = std::make_unique<GameObject>();
	std::shared_ptr<RenderComponent> floorRenderComponent = std::make_shared<RenderComponent>();
	m_pFloor->AddComponent(MyComponent::ComponentType::Render, floorRenderComponent);
	floorRenderComponent->SetModel(ModelManager::Instance().GetModel("Cube"));
	floorRenderComponent->SetMaterial(m_pFloorMaterial.get());
	floorRenderComponent->SetEffect(m_pBasicEffect.get());

    m_pUIElement = std::make_unique<UIElement>();
    m_pUIElement->Init(m_pUIBasicEffect.get(), m_pUIMaterial.get(), square.get(), m_pUiFontSet.get(), "OptionFont", m_pUiBrush.get());
    m_pUIElement->SetPosition(0, 0);
    m_pUIElement->SetScale(200, 200);
	m_pUIElement->SetStaticText("Hello World!");


    m_pUiAim = make_unique<UIRender>();
    m_pUiAim->SetEffect(m_pUIBasicEffect.get());
    m_pUiAim->SetMaterial(m_pUIAimMaterial.get());
    m_pUiAim->SetModel(ModelManager::Instance().GetModel("Square"));
    m_pUiAim->GetTransform().SetPosition({0,0,0.1f});
    m_pUiAim->GetTransform().SetScale({ 32,32,1.f });

    m_pUiBar = make_unique<UIBar>();
    m_pUiBar->Init({ -500.f,-200.f,0.1f },
        { 300.f,32.f },
        m_pUiBarBgMaterial.get(),
        m_pUiBarMaterial.get(),
        m_pUIBasicEffect.get(),
        m_pUIBasicEffect.get());

    m_pUiButton = make_shared<UIButton>();
    m_pUiButton->Init(m_pUIBasicEffect.get(), m_pUIMaterial.get(), square.get(), m_pUiFontSet.get(), "OptionFont", m_pUiBrush.get());
    m_pUiButton->SetButton({ -300,0,0.5f }, 100.f, 30.f);
	m_pUiButton->SetStaticText("Button1");
    m_pUiButton->SetCenterAlignment();

    UIManager::GetInstance().AddUiLayer("Button", 1);
    UIManager::GetInstance().GetUILayer("Button")->AddComponent(m_pUiButton);

    //=====物理の初期化

    // Create the settings for the collision volume (the shape).

    // Create the shape

    // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.

    // Create the actual rigid body


    // Create BodyCreationSettings
    RefConst<Shape> defaultBoxSettings = ShapeFactory::Instance().GetOrCreateBox(RVec3(0.5f, 0.5f, 0.5f));
    BodyCreationSettings cubeBoxSettings(defaultBoxSettings, {0,0,0 }, Quat::sIdentity(), EMotionType::Kinematic, Layers::ITEM);
    m_pCubeCollider = make_shared<PhysicsComponent>();
    m_pCubeCollider->Init(cubeBoxSettings, EActivation::Activate);  //Create& Add
    m_pCubeObject->AddComponent(MyComponent::ComponentType::Physics, m_pCubeCollider);
    m_pCubeObject->GetTransform().SetScale(1, 1, 1);
	m_pCubeObject->GetTransform().SetPosition({ -3, 0.5f, 0 });


    BodyCreationSettings appleBoxSettings(new BoxShape(RVec3(0.25f, 0.25f, 0.25f)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM);
    PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings);// Set the mass properties for the apple box
    std::shared_ptr<PhysicsComponent> appleCollider = make_shared<PhysicsComponent>();
    appleCollider->Init(appleBoxSettings, EActivation::Activate);  //Create& Add
	m_appleInstance->AddComponent(MyComponent::ComponentType::Physics, appleCollider);
    m_appleInstance->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
    m_appleInstance->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(0, 3, 3);

    BodyCreationSettings appleBoxSettings1(new BoxShape(RVec3(0.25f, 0.25f, 0.25f)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM);
    PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings1);// Set the mass properties for the apple box
    std::shared_ptr<PhysicsComponent> appleCollider1 = make_shared<PhysicsComponent>();
    appleCollider1->Init(appleBoxSettings1, EActivation::Activate);  //Create& Add
    m_appleInstance1->AddComponent(MyComponent::ComponentType::Physics, appleCollider1);
    m_appleInstance1->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
    m_appleInstance1->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(3, 3, 0);



    BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(5.f, 0.05f, 5.f)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Kinematic, Layers::BOAT);
    m_pFloorCollider = make_shared<PhysicsComponent>();
    m_pFloorCollider->Init(floorBoxSettings, EActivation::Activate);
    m_pFloor->AddComponent(MyComponent::ComponentType::Physics, m_pFloorCollider);
    m_pFloor->GetTransform().SetScale(10.f, .1f, 10.f);
    m_pFloor->GetTransform().SetPosition(0, -0.05f, 0);



	//Collider Debug Render Component配置
    m_pDebugColliderRender = make_shared<RenderComponent>();
    m_pDebugColliderRender->SetEffect(m_pDebugEffect.get());
    m_pDebugColliderRender->SetMaterial(m_pDebugMaterial.get());
    m_pDebugColliderRender->SetModel(ModelManager::Instance().GetModel("Capsule"));

	//Debug Collider Render ComponentをPlayerに追加
    m_pPlayer->AddComponent(MyComponent::ComponentType::DebugRender, m_pDebugColliderRender);
    //Set Ui Component to hunger component
    m_pPlayer->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->SetUIComponent(m_pUiBar.get());
    // m_pUIElement->SetProvider(m_pPlayer.get(), UIFormat::FormatHunger);


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

    m_pPBRFoodMaterial = std::make_shared<Material>();
    m_pPBRFoodMaterial->SetTexture(Material::Albedo, albedoTex.get());
    m_pPBRFoodMaterial->SetTexture(Material::Normal, normalTex.get());
    m_pPBRFoodMaterial->SetTexture(Material::Metallic_Smooth, metallicTex.get());

    m_pUIMaterial = std::make_shared<Material>();
    m_pUIMaterial->SetDiffuse({ 0,0,1,1 });

	m_pDebugMaterial = std::make_shared<Material>();
    m_pDebugMaterial->SetDiffuse({ 0,1,0,0.2f });

    m_pFloorMaterial = std::make_shared<Material>();
	m_pFloorMaterial->SetDiffuse({ 0.2f,0.2f,0.6f,0.4f });

    m_pUIAimMaterial = std::make_shared<Material>();
    m_pUIAimMaterial->SetTexture(Material::Albedo, m_pUiAimTex.get());

    m_pUiBarMaterial = std::make_shared<Material>();
    m_pUiBarMaterial->SetDiffuse({ 1.f,0.f,0.f,1.f });

    m_pUiBarBgMaterial = std::make_shared<Material>();
    m_pUiBarBgMaterial->SetDiffuse({ 0,0,1.f,1.f });

	return true;
}

void SceneManager::SetCurrentCamera(CameraBase* camera)
{
    m_pCurrentCamera = camera;
}
