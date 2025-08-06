#include "SceneGame.h"

#include <DirectXMath.h>
#include "d3dUtil.h"
#include "DayLight.h"
#include "GameApp.h"
#include "Geometry.h"
#include "HungerComponent.h"
#include "ItemDataBase.h"
#include "KInput.h"
#include "ModelManager.h"
#include "PBREffect.h"
#include "PhysicsManager.h"
#include "Player.h"
#include "RenderState.h"
#include "ShapeFactory.h"
#include "SkyboxEffect.h"
#include "UIBasicEffect.h"
#include "UIButton.h"
#include "UIManager.h"
#include "Skybox.h"


namespace
{
	static constexpr DirectX::XMFLOAT4 DefaultLightColor = { 1.0f, 1.0f, 1.0, 1.0f }; // Ambient light color
	static constexpr float WaterWidth = 50.f;
	static constexpr float waterHeight = 1.f;

	//Collider Setting
	static constexpr DirectX::XMFLOAT3 HalfUnitScale = { 0.5f,0.5f,0.5f };	//Default Cube Size (length,height,width={1,1,1})
	static constexpr DirectX::XMFLOAT3 HalfFloorScale = { 5,0.1f,5 };

	static constexpr DirectX::XMFLOAT3 DefaultCameraPos = { 0,10,-10 };	//Default Camera Position
	static constexpr DirectX::XMFLOAT3 DefaultCameraTarget = { 0,0,0 };	//Default Camera Target Position

	static constexpr DirectX::XMFLOAT3 DefaultLightPosition = { 0,10,0 };	//Default Light Position


	static constexpr DirectX::XMFLOAT3 UIAimSize = { 32,32,1.f };
}
void SceneGame::Init()
{
	//============Init Player
	Player* player = CreateObj<Player>("Player");
	player->Init();
	player->GetCameraController()->GetCamera()->SetPos(DefaultCameraPos);
	player->GetCameraController()->GetCamera()->SetTarget(DefaultCameraTarget);
	m_pCurrentCamera = player->GetCameraController()->GetCamera();

	//============ Init light
	DirLight* light = CreateObj<DayLight>("DayLight");
	light->SetPosition(DefaultLightPosition);
	light->SetAmbient(DefaultLightColor);
	light->SetDiffuse({0.5,0.5,0.5,1});

	//===========Init Texture
	Texture* albedoTex = CreateObj<Texture>("AlbedoTex");
	Texture* normalTex = CreateObj<Texture>("NormalTex");
	Texture* metallicTex = CreateObj<Texture>("MetallicTex");
	Texture* uiAimTex = CreateObj<Texture>("UiAimTex");

	albedoTex->Create("Assets/Texture/Foods_DefaultMaterial_AlbedoTransparency.png");
	normalTex->Create("Assets/Texture/Foods_DefaultMaterial_Normal.png");
	metallicTex->Create("Assets/Texture/Foods_DefaultMaterial_MetallicSmoothness.png");
	uiAimTex->Create("Assets/Texture/UI/UI_Aim_128x128.png");
	

	//============Get Shader
	VertexShader* basicPosNormalTexVS = GetObj<VertexShader>("BasicPosNormalTexVS");
	PixelShader* basicBlinnPhongPS = GetObj<PixelShader>("BasicBlinnPhongPS");
	PixelShader* pbrPS = GetObj<PixelShader>("PBRPS");
	VertexShader* pbrVS = GetObj<VertexShader>("PBRVS");
	PixelShader* uiElementPS = GetObj<PixelShader>("UIElementPS");
	VertexShader* uiElementVS = GetObj<VertexShader>("UIElementVS");


	//===========Init Effect
	BasicEffect* basicEffect = CreateObj<BasicEffect>("BasicEffect");
	basicEffect->InitPixelShader(basicBlinnPhongPS);
	basicEffect->InitVertexShader(basicPosNormalTexVS);
	basicEffect->InitCamera(player->GetCameraController()->GetCamera());
	basicEffect->InitEffectDirLight(light);

	PBREffect* pbrEffect = CreateObj<PBREffect>("PBREffect");
	pbrEffect->InitPixelShader(pbrPS);
	pbrEffect->InitVertexShader(pbrVS);
	pbrEffect->InitCamera(player->GetCameraController()->GetCamera());
	pbrEffect->InitEffectDirLight(light);

	UIBasicEffect* uiBasicEffect = CreateObj<UIBasicEffect>("UiBasicEffect");
	uiBasicEffect->InitPixelShader(uiElementPS);
	uiBasicEffect->InitVertexShader(uiElementVS);

	BasicEffect* debugEffect = CreateObj<BasicEffect>("DebugEffect");
	debugEffect->InitPixelShader(basicBlinnPhongPS);
	debugEffect->InitVertexShader(basicPosNormalTexVS);
	debugEffect->InitCamera(player->GetCameraController()->GetCamera());
	debugEffect->InitEffectDirLight(light);



	//===========Init Material
	Material* blinnPhongMat = CreateObj<Material>("BlinnPhongMaterial");

	Material* foodMaterial = CreateObj<Material>("FoodMaterial");
	foodMaterial->SetTexture(Material::Albedo, albedoTex);
	foodMaterial->SetTexture(Material::Normal, normalTex);
	foodMaterial->SetTexture(Material::Metallic_Smooth, metallicTex);

	Material* uiMaterial = CreateObj<Material>("UiMaterial");
	uiMaterial->SetDiffuse({ 0,0,1,1 });

	Material* debugMaterial = CreateObj<Material>("DebugMaterial");
	debugMaterial->SetDiffuse({ 0,1,0,0.2f });

	Material* floorMaterial = CreateObj<Material>("FloorMaterial");
	floorMaterial->SetDiffuse({ 0.2f,0.2f,0.6f,1.f });

	Material* uiAimMaterial = CreateObj<Material>("UIAimMaterial");
	uiAimMaterial->SetTexture(Material::Albedo, uiAimTex);

	Material* uiBarMaterial = CreateObj<Material>("UiBarMaterial");
	uiBarMaterial->SetDiffuse({ 1.f,0.f,0.f,1.f });

	Material* uiBarBgMaterial = CreateObj<Material>("UiBarBgMaterial");
	uiBarBgMaterial->SetDiffuse({ 0,0,1.f,1.f });

	//===========Register food data
	std::shared_ptr<Food> apple = std::make_shared<Food>(20.f);
	ItemDataBase::Instance().RegisterItem("Apple", apple);
	std::shared_ptr<Food> banana = std::make_shared<Food>(10.f);
	ItemDataBase::Instance().RegisterItem("Banana", banana);


	//=====GameObjectの初期化
	std::shared_ptr<RenderComponent> appleRenderComponent = std::make_shared<RenderComponent>();
	appleRenderComponent->SetModel(ModelManager::Instance().GetModel("Food_Apple"));
	appleRenderComponent->SetMaterial(foodMaterial);
	appleRenderComponent->SetEffect(pbrEffect);
	

	std::shared_ptr<RenderComponent> bananaRenderComponent = std::make_shared<RenderComponent>();
	bananaRenderComponent->SetModel(ModelManager::Instance().GetModel("Food_Banana"));
	bananaRenderComponent->SetMaterial(foodMaterial);
	bananaRenderComponent->SetEffect(pbrEffect);

	//===========Init item
	std::shared_ptr<ItemInstance> appleInstance = make_shared<ItemInstance>();
	RegisterSceneObject(appleInstance);
	appleInstance->InitItem(ItemDataBase::Instance().GetItem("Apple"), 5);
	appleInstance->AddComponent(MyComponent::ComponentType::Render, appleRenderComponent);


	std::shared_ptr <ItemInstance> bananaInstance = make_shared<ItemInstance>();
	RegisterSceneObject(bananaInstance);
	bananaInstance->InitItem(ItemDataBase::Instance().GetItem("Banana"), 2);
	bananaInstance->AddComponent(MyComponent::ComponentType::Render, bananaRenderComponent);
	


	GameObject* floor = CreateObj<GameObject>("Floor");
	std::shared_ptr<RenderComponent> floorRenderComponent = std::make_shared<RenderComponent>();
	floor->AddComponent(MyComponent::ComponentType::Render, floorRenderComponent);
	floorRenderComponent->SetModel(ModelManager::Instance().GetModel("Cube"));
	floorRenderComponent->SetMaterial(floorMaterial);
	floorRenderComponent->SetEffect(basicEffect);

	//===========UI初期化
	UIRender* uiAim = CreateObj<UIRender>("UiAim");
	uiAim->SetEffect(uiBasicEffect);
	uiAim->SetMaterial(uiAimMaterial);
	uiAim->SetModel(ModelManager::Instance().GetModel("Square"));
	uiAim->GetTransform().SetPosition({ 0,0,0.1f });
	uiAim->GetTransform().SetScale(UIAimSize);

	UIBar* uiBar = CreateObj<UIBar>("UiBar");
	uiBar->Init({ -500.f,-200.f,0.1f },
		{ 300.f,32.f },
		uiBarBgMaterial,
		uiBarMaterial,
		uiBasicEffect,
		uiBasicEffect);


	UIManager::GetInstance().ClearLayers();
	UIManager::GetInstance().AddUiLayer("HungerBar", 2);
	UIManager::GetInstance().GetUILayer("HungerBar")->AddComponent(uiBar);

	UIManager::GetInstance().AddUiLayer("Aim", 3);
	UIManager::GetInstance().GetUILayer("Aim")->AddComponent(uiAim);
	//=====物理の初期化

	// Create the settings for the collision volume (the shape).

	// Create the shape

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.

	// Create the actual rigid body

	RefConst<Shape> defaultBoxSettings = ShapeFactory::Instance().GetOrCreateBox(RVec3(HalfUnitScale.x,HalfUnitScale.y,HalfUnitScale.z));
	

	std::shared_ptr<PhysicsComponent> appleCollider = make_shared<PhysicsComponent>();
	appleInstance->AddComponent(MyComponent::ComponentType::Physics, appleCollider);
	float appleScale = 0.05f;
	DirectX::XMFLOAT3 appleColliderSize = ModelManager::Instance().GetModel("Food_Apple")->GetModelSize();
	BodyCreationSettings appleBoxSettings(new BoxShape(RVec3(appleColliderSize.x * 0.5f * appleScale, appleColliderSize.y * 0.5f * appleScale, appleColliderSize.z * 0.5f * appleScale)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM); //Init apple cube
	PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings);// Set the mass properties for the apple box
	appleCollider->Init(appleBoxSettings, EActivation::Activate);  //Create& Add
	appleInstance->GetTransform().SetScale(appleScale, appleScale, appleScale);
	appleInstance->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(0, 3, 3);

	std::shared_ptr<PhysicsComponent> bananaCollider = make_shared<PhysicsComponent>();
	bananaInstance->AddComponent(MyComponent::ComponentType::Physics, bananaCollider);
	float bananaScale = 0.05f;
	DirectX::XMFLOAT3 bananaColliderSize = ModelManager::Instance().GetModel("Food_Apple")->GetModelSize();
	BodyCreationSettings bananaBoxSettings(new BoxShape(RVec3(bananaColliderSize.x * 0.5f * bananaScale, bananaColliderSize.y * 0.5f * bananaScale, bananaColliderSize.z * 0.5f * bananaScale)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM); //Init apple cube
	PhysicsManager::Instance().SetBodyCreationMass(1.f, bananaBoxSettings);// Set the mass properties for the apple box
	bananaCollider->Init(bananaBoxSettings, EActivation::Activate);  //Create& Add
	bananaInstance->GetTransform().SetScale(bananaScale,bananaScale,bananaScale);
	bananaInstance->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(-3, 3, 0);


	BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(HalfFloorScale.x,HalfFloorScale.y,HalfFloorScale.z)), Vec3().sZero(), Quat::sIdentity(), EMotionType::Dynamic, Layers::BOAT);
	std::shared_ptr<PhysicsComponent> floorCollider = make_shared<PhysicsComponent>();
	floorCollider->Init(floorBoxSettings, EActivation::Activate);
	floor->AddComponent(MyComponent::ComponentType::Physics, floorCollider);
	floor->GetTransform().SetScale(HalfFloorScale * 2.f);

	// Init Buoyancy system
	BuoyancySystem* buoyancySystem = CreateObj<BuoyancySystem>("BuoyancySystem");
	buoyancySystem->Init(WaterWidth,waterHeight);


	//Collider Debug Render Component配置
	std::shared_ptr<RenderComponent> debugColliderRender = std::make_shared<RenderComponent>();
	debugColliderRender->SetEffect(debugEffect);
	debugColliderRender->SetMaterial(debugMaterial);
	debugColliderRender->SetModel(ModelManager::Instance().GetModel("Capsule"));


	//Debug Collider Render ComponentをPlayerに追加
	player->AddComponent(MyComponent::ComponentType::DebugRender, debugColliderRender);
	//Set Ui Component to hunger component
	player->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->SetUIComponent(uiBar);

	//===========Set skybox camera
	GetObj<Skybox>("Skybox")->GetSkyboxEffect()->InitCamera(m_pCurrentCamera);

}

void SceneGame::UnInit()
{
}

void SceneGame::Update(float tick)
{
	//===============Handle Input
	GetObj<Skybox>("Skybox")->Update(tick);

	//===============Physics Update
	GetObj<BuoyancySystem>("BuoyancySystem")->PreUpdate(tick);
	PhysicsManager::Instance().Update(tick); // 物理システムの更新

	//===============Light Update
	GetObj<DayLight>("DayLight")->Update(tick);

	//===============Object Update
	GetObj<GameObject>("Floor")->Update(tick);

	for(const auto& object:m_sceneObjects)
	{
		object->Update(tick);
	}

	//===============Player Update
	GetObj<Player>("Player")->Update(tick);

	//===============UI Update
	UIManager::GetInstance().Update(tick);

	//===============Clear all inactive game objects
	DeleteInactiveSceneObject();
}

void SceneGame::Draw()
{
	GetObj<Skybox>("Skybox")->Draw();

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

	GetObj<GameObject>("Floor")->Draw();

	for (const auto& object : m_sceneObjects)
	{
		object->Draw();
	}

	GetObj<Player>("Player")->Draw();

	//Ui描画
	GameApp::SetDepthStencilState(RenderStates::DSSNoDepthTest);
	UIManager::GetInstance().Draw();

}

void SceneGame::RegisterSceneObject(std::shared_ptr<GameObject> object)
{
	m_sceneObjects.push_back(object);
}

void SceneGame::DeleteInactiveSceneObject()
{
	for (auto it = m_sceneObjects.begin(); it != m_sceneObjects.end(); )
	{
		if (!(*it)->GetActive())
		{
			it = m_sceneObjects.erase(it);
		}
		else
		{
			++it;
		}
	}
}
