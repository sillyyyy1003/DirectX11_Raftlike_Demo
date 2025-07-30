#include "SceneGame.h"
#include <DirectXMath.h>

#include "Geometry.h"
#include "HungerComponent.h"
#include "ItemDataBase.h"
#include "KInput.h"
#include "ModelManager.h"
#include "PBREffect.h"
#include "PhysicsManager.h"
#include "Player.h"
#include "ShapeFactory.h"
#include "UIBasicEffect.h"
#include "UIButton.h"
#include "UIManager.h"

namespace
{
	static constexpr DirectX::XMFLOAT4 DefaultLightColor = { 0.5f, 0.5f, 0.5f, 1.0f }; // Ambient light color
}
void SceneGame::Init()
{

	// Init Player
	Player* player = CreateObj<Player>("Player");
	player->Init();
	player->GetCameraController()->GetCamera()->SetPos({ 0,10,-10 });
	player->GetCameraController()->GetCamera()->SetTarget({ 0,0,0 });
	m_pCurrentCamera = player->GetCameraController()->GetCamera();

	DirLight* light = CreateObj<DirLight>("DirLight");
	light->SetPosition({ 0, 10, 0 });
	light->SetAmbient(DefaultLightColor);
	light->SetDiffuse(DefaultLightColor);
	light->SetIntensity(10.f);


	// Get Shader
	VertexShader* basicPosNormalTexVS = GetObj<VertexShader>("BasicPosNormalTexVS");
	PixelShader* basicBlinnPhongPS = GetObj<PixelShader>("BasicBlinnPhongPS");
	PixelShader* pbrPS = GetObj<PixelShader>("PBRPS");
	VertexShader* pbrVS = GetObj<VertexShader>("PBRVS");
	PixelShader* uiElementPS = GetObj<PixelShader>("UIElementPS");
	VertexShader* uiElementVS = GetObj<VertexShader>("UIElementVS");

	// Init Effect
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

	// Init Texture
	Texture* albedoTex = CreateObj<Texture>("AlbedoTex");
	Texture* normalTex = CreateObj<Texture>("NormalTex");
	Texture* metallicTex = CreateObj<Texture>("MetallicTex");
	Texture* uiAimTex = CreateObj<Texture>("UiAimTex");

	albedoTex->Create("Assets/Texture/Foods_DefaultMaterial_AlbedoTransparency.png");
	normalTex->Create("Assets/Texture/Foods_DefaultMaterial_Normal.png");
	metallicTex->Create("Assets/Texture/Foods_DefaultMaterial_MetallicSmoothness.png");
	uiAimTex->Create("Assets/Texture/UI/UI_Aim_128x128.png");

	// Init Material
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
	floorMaterial->SetDiffuse({ 0.2f,0.2f,0.6f,0.4f });

	Material* uiAimMaterial = CreateObj<Material>("UIAimMaterial");
	uiAimMaterial->SetTexture(Material::Albedo, uiAimTex);

	Material* uiBarMaterial = CreateObj<Material>("UiBarMaterial");
	uiBarMaterial->SetDiffuse({ 1.f,0.f,0.f,1.f });

	Material* uiBarBgMaterial = CreateObj<Material>("UiBarBgMaterial");
	uiBarBgMaterial->SetDiffuse({ 0,0,1.f,1.f });

	//=====GameObjectの初期化

	std::shared_ptr<RenderComponent> appleRenderComponent = std::make_shared<RenderComponent>();
	appleRenderComponent->SetModel(ModelManager::Instance().GetModel("Food_Apple"));
	appleRenderComponent->SetMaterial(foodMaterial);
	appleRenderComponent->SetEffect(pbrEffect);

	// Register food data
	std::shared_ptr<Food> apple = std::make_shared<Food>(20.f);
	ItemDataBase::Instance().RegisterItem("Apple", apple);

	// Init item
	ItemInstance* appleInstance = CreateObj<ItemInstance>("AppleInstance");
	appleInstance->InitItem(ItemDataBase::Instance().GetItem("Apple"), 5);
	appleInstance->AddComponent(MyComponent::ComponentType::Render, appleRenderComponent);

	ItemInstance* appleInstance1 = CreateObj<ItemInstance>("AppleInstance1");
	appleInstance1->InitItem(ItemDataBase::Instance().GetItem("Apple"), 2);
	appleInstance1->AddComponent(MyComponent::ComponentType::Render, appleRenderComponent);


	GameObject* cubeObject = CreateObj<GameObject>("CubeObject");
	std::shared_ptr<RenderComponent> cubeRenderComponent = std::make_shared<RenderComponent>();
	cubeObject->AddComponent(MyComponent::ComponentType::Render, cubeRenderComponent);
	cubeRenderComponent->SetModel(ModelManager::Instance().GetModel("Cube"));
	cubeRenderComponent->SetMaterial(blinnPhongMat);
	cubeRenderComponent->SetEffect(basicEffect);

	GameObject* floor = CreateObj<GameObject>("Floor");
	std::shared_ptr<RenderComponent> floorRenderComponent = std::make_shared<RenderComponent>();
	floor->AddComponent(MyComponent::ComponentType::Render, floorRenderComponent);
	floorRenderComponent->SetModel(ModelManager::Instance().GetModel("Cube"));
	floorRenderComponent->SetMaterial(floorMaterial);
	floorRenderComponent->SetEffect(basicEffect);


	UIRender* uiAim = CreateObj<UIRender>("UiAim");
	uiAim->SetEffect(uiBasicEffect);
	uiAim->SetMaterial(uiAimMaterial);
	uiAim->SetModel(ModelManager::Instance().GetModel("Square"));
	uiAim->GetTransform().SetPosition({ 0,0,0.1f });
	uiAim->GetTransform().SetScale({ 32,32,1.f });

	UIBar* uiBar = CreateObj<UIBar>("UiBar");
	uiBar->Init({ -500.f,-200.f,0.1f },
		{ 300.f,32.f },
		uiBarBgMaterial,
		uiBarMaterial,
		uiBasicEffect,
		uiBasicEffect);

	UIButton* uiButton = CreateObj<UIButton>("UiButton");
	uiButton->Init(uiBasicEffect, uiMaterial, ModelManager::Instance().GetModel("Square"), GetObj<UIFontSet>("UIFontSet"), "OptionFont", GetObj<UIBrush>("UiBrush"));
	uiButton->SetButton({ -300,0,0.5f }, 100.f, 30.f);
	uiButton->SetStaticText("Button1");
	uiButton->SetCenterAlignment();

	UIManager::GetInstance().AddUiLayer("Button", 1);
	UIManager::GetInstance().GetUILayer("Button")->AddComponent(uiButton);

	//=====物理の初期化

	// Create the settings for the collision volume (the shape).

	// Create the shape

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.

	// Create the actual rigid body

	RefConst<Shape> defaultBoxSettings = ShapeFactory::Instance().GetOrCreateBox(RVec3(0.5f, 0.5f, 0.5f));
	BodyCreationSettings cubeBoxSettings(defaultBoxSettings, { 0,0,0 }, Quat::sIdentity(), EMotionType::Kinematic, Layers::ITEM);

	std::shared_ptr<PhysicsComponent> cubeCollider = make_shared<PhysicsComponent>();
	cubeCollider->Init(cubeBoxSettings, EActivation::Activate);  //Create& Add
	cubeObject->AddComponent(MyComponent::ComponentType::Physics, cubeCollider);
	cubeObject->GetTransform().SetScale(1, 1, 1);
	cubeObject->GetTransform().SetPosition({ -3, 0.5f, 0 });


	BodyCreationSettings appleBoxSettings(new BoxShape(RVec3(0.25f, 0.25f, 0.25f)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM);
	PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings);// Set the mass properties for the apple box
	std::shared_ptr<PhysicsComponent> appleCollider = make_shared<PhysicsComponent>();
	appleCollider->Init(appleBoxSettings, EActivation::Activate);  //Create& Add
	appleInstance->AddComponent(MyComponent::ComponentType::Physics, appleCollider);
	appleInstance->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
	appleInstance->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(0, 3, 3);

	BodyCreationSettings appleBoxSettings1(new BoxShape(RVec3(0.25f, 0.25f, 0.25f)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM);
	PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings1);// Set the mass properties for the apple box
	std::shared_ptr<PhysicsComponent> appleCollider1 = make_shared<PhysicsComponent>();
	appleCollider1->Init(appleBoxSettings1, EActivation::Activate);  //Create& Add
	appleInstance1->AddComponent(MyComponent::ComponentType::Physics, appleCollider1);
	appleInstance1->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
	appleInstance1->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(3, 3, 0);



	BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(5.f, 0.05f, 5.f)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Kinematic, Layers::BOAT);
	std::shared_ptr<PhysicsComponent> floorCollider = make_shared<PhysicsComponent>();
	floorCollider->Init(floorBoxSettings, EActivation::Activate);
	floor->AddComponent(MyComponent::ComponentType::Physics, floorCollider);
	floor->GetTransform().SetScale(10.f, .1f, 10.f);
	floor->GetTransform().SetPosition(0, -0.05f, 0);



	//Collider Debug Render Component配置
	std::shared_ptr<RenderComponent> debugColliderRender = std::make_shared<RenderComponent>();
	debugColliderRender->SetEffect(debugEffect);
	debugColliderRender->SetMaterial(debugMaterial);
	debugColliderRender->SetModel(ModelManager::Instance().GetModel("Capsule"));

	//Debug Collider Render ComponentをPlayerに追加
	player->AddComponent(MyComponent::ComponentType::DebugRender, debugColliderRender);
	//Set Ui Component to hunger component
	player->GetComponent<HungerComponent>(MyComponent::ComponentType::Hunger)->SetUIComponent(uiBar);
	// m_pUIElement->SetProvider(m_pPlayer.get(), UIFormat::FormatHunger);




}

void SceneGame::UnInit()
{
}

void SceneGame::Update(float tick)
{
	//===============Handle Input
	

	GetObj<DirLight>("DirLight")->Update(tick);
	PhysicsManager::Instance().Update(tick); // 物理システムの更新

	GetObj<GameObject>("CubeObject")->Update(tick);
	GetObj<GameObject>("Floor")->Update(tick);

	GetObj<ItemInstance>("AppleInstance")->Update(tick);
	GetObj<ItemInstance>("AppleInstance1")->Update(tick);

	GetObj<Player>("Player")->Update(tick);
	UIManager::GetInstance().Update(tick);
}

void SceneGame::Draw()
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

	GetObj<GameObject>("CubeObject")->Draw();
	GetObj<GameObject>("Floor")->Draw();
	GetObj<Player>("Player")->Draw();
	GetObj<ItemInstance>("AppleInstance")->Draw();
	GetObj<ItemInstance>("AppleInstance1")->Draw();

	//Ui描画
	UIRender* uiAim = GetObj<UIRender>("UiAim");
	uiAim->Draw();
	UIBar* uiBar = GetObj <UIBar>("UiBar");
	uiBar->Draw();

	//m_pUIElement->Draw();
	UIManager::GetInstance().Draw();
}
