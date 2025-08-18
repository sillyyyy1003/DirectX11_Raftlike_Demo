#include "SceneGame.h"
#include <DirectXMath.h>
#include "d3dUtil.h"
#include "DayLight.h"
#include "GameApp.h"
#include "Geometry.h"
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
#include "UIManager.h"
#include "Skybox.h"
#include "TextureManager.h"
#include "UIInventorySlot.h"
#include "UIPlayerStatus.h"


namespace
{
	static constexpr DirectX::XMFLOAT4 DefaultLightColor = { 1.0f, 1.0f, 1.0, 1.0f }; // Ambient light color
	static constexpr float WaterWidth = 50.f;
	static constexpr float waterHeight = 1.f;

	//Collider Setting
	static constexpr DirectX::XMFLOAT3 HalfUnitScale = { 0.5f,0.5f,0.5f };	//Default Cube Size (length,height,width={1,1,1})
	static constexpr DirectX::XMFLOAT3 HalfFloorScale = { 5,0.1f,5 };

	static constexpr DirectX::XMFLOAT3 DefaultLightPosition = { 0,10,0 };	//Default Light Position


	static constexpr DirectX::XMFLOAT3 UIAimSize = { 32,32,1.f };
	static constexpr DirectX::XMFLOAT3 UiInventorySlotSize = { 64,64,1 };

	static constexpr float DefaultObjectScale = 0.05f;
}
void SceneGame::Init()
{
	//============Init Player
	Player* player = CreateObj<Player>("Player");
	player->Init("Assets/ConfigFile/PlayerConfig.json");	//json fileから読み込み
	m_pCurrentCamera = player->GetCameraController()->GetCamera();

	//============ Init light
	DirLight* light = CreateObj<DayLight>("DayLight");
	light->SetPosition(DefaultLightPosition);
	light->SetAmbient(DefaultLightColor);
	light->SetDiffuse({0.5,0.5,0.5,1});

	//===========Init Texture
	Texture* albedoTex = TextureManager::Instance().RegisterTexture("Food_Albedo","Assets/Texture/Foods_DefaultMaterial_AlbedoTransparency.png");
	Texture* normalTex = TextureManager::Instance().RegisterTexture("Food_Normal", "Assets/Texture/Foods_DefaultMaterial_Normal.png");
	Texture* metallicTex = TextureManager::Instance().RegisterTexture("Food_Metallic", "Assets/Texture/Foods_DefaultMaterial_MetallicSmoothness.png");
	Texture* uiAimTex = TextureManager::Instance().RegisterTexture("UI_Aim", "Assets/Texture/UI/UI_Aim_128x128.png");

	//Icon Texture
	TextureManager::Instance().RegisterTexture("Apple_Icon", "Assets/Texture/ObjectIcon/coca-leaves.png");
	TextureManager::Instance().RegisterTexture("Banana_Icon", "Assets/Texture/ObjectIcon/wood.png");
	// Inventory 背景読み込み
	Texture* uiInventorySlotBgTex = TextureManager::Instance().RegisterTexture("UI_InventorySlotBg", "Assets/Texture/UI/UI_Inventory_Block_256x256.png");
	// Inventory Cursor読み込み
	Texture* uiInventoryChosenTex = TextureManager::Instance().RegisterTexture("UI_InventoryChosenSlot", "Assets/Texture/UI/UI_Inventory_Chose_256x256.png");

	//Player status icon
	Texture* hpIconTex = TextureManager::Instance().RegisterTexture("UI_HpIcon", "Assets/Texture/UI/UI_Hp_Bar_Icon_128x128.png");
	Texture* hungerIconTex = TextureManager::Instance().RegisterTexture("UI_HungerIcon", "Assets/Texture/UI/UI_Hp_Bar_Food_128x128.png");



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

	// UI Aim Material
	Material* uiAimMaterial = CreateObj<Material>("UIAimMaterial");
	uiAimMaterial->SetTexture(Material::Albedo, uiAimTex);

	// UI Bar Material
	Material* uiBarMaterial = CreateObj<Material>("UiBarMaterial");
	uiBarMaterial->SetDiffuse({1,0,0,1});

	Material* uiBarBgMaterial = CreateObj<Material>("UiBarBgMaterial");
	uiBarBgMaterial->SetDiffuse({ 0,0,1.f,1.f });

	// UI Inventory Material
	Material* uiInventoryBgMaterial = CreateObj<Material>("UiInventoryBgMaterial");
	uiInventoryBgMaterial->SetDiffuse({ 105.f / 255,42.f / 255,0,1.f });	// Brown color for inventory background

	// 共通の背景Material
	Material* uiInventorySlotBgMaterial = CreateObj<Material>("UiInventorySlotBgMaterial");
	uiInventorySlotBgMaterial->SetTexture(Material::Albedo, uiInventorySlotBgTex);
	// UI Inventory Slot Material(Textureは切り替えて)
	Material* uiInventorySlotMaterial = CreateObj<Material>("UiInventorySlotMaterial");
	// Inventory cursor material
	Material* uiInventoryChosenSLotMaterial = CreateObj<Material>("UIInventoryChosenSlotMaterial");
	uiInventoryChosenSLotMaterial->SetTexture(Material::Albedo, uiInventoryChosenTex);

	// Player Status Material
	UIPlayerStatus::MaterialList hpMaterials(3);
	hpMaterials[UIPlayerStatus::MaterialType::Icon] = CreateObj<Material>("HpIconMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Icon]->SetTexture(Material::Albedo, hpIconTex);
	hpMaterials[UIPlayerStatus::MaterialType::Background] = CreateObj<Material>("HpBarBgMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Background]->SetDiffuse({ 0.2f,0.2f,0.2f,1.f });	// Dark gray for health bar background
	hpMaterials[UIPlayerStatus::MaterialType::Bar] = CreateObj<Material>("HpBarMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Bar]->SetDiffuse({ 1.f,0.f,0.f,1.f });	// Red for health bar


	UIPlayerStatus::MaterialList hungerMaterials(3);
	hungerMaterials[UIPlayerStatus::MaterialType::Icon] = CreateObj<Material>("HungerIconMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Icon]->SetTexture(Material::Albedo, hungerIconTex);
	hungerMaterials[UIPlayerStatus::MaterialType::Background] = CreateObj<Material>("HungerBarBgMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Background]->SetDiffuse({ 0.2f,0.2f,0.2f,1.f });	// Dark gray for hunger bar background
	hungerMaterials[UIPlayerStatus::MaterialType::Bar] = CreateObj<Material>("HungerBarMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Bar]->SetDiffuse({ 1.f,0.5f,0.f,1.f });	// Orange for hunger bar



	//===========Register food data
	std::shared_ptr<Food> apple = std::make_shared<Food>(20.f);
	ItemDataBase::Instance().RegisterItem("Apple", apple);
	std::shared_ptr<Food> banana = std::make_shared<Food>(10.f);
	ItemDataBase::Instance().RegisterItem("Banana", banana);


	//===========Init item

	std::shared_ptr<ItemInstance> appleInstance = make_shared<ItemInstance>();
	appleInstance->InitItem(ItemDataBase::Instance().GetItem("Apple"), 5);	 //Set apple item data
	std::shared_ptr<RenderComponent> appleRenderComponent = std::make_shared<RenderComponent>();	// Create apple render component
	appleRenderComponent->Init(foodMaterial, pbrEffect, ModelManager::Instance().GetModel("Food_Apple"));	  // Init apple render component
	appleInstance->AddComponent(MyComponent::ComponentType::Render, appleRenderComponent);	// Add apple render component to apple instance
	RegisterSceneObject(appleInstance);	// シーンに登録

	std::shared_ptr <ItemInstance> bananaInstance = make_shared<ItemInstance>();
	bananaInstance->InitItem(ItemDataBase::Instance().GetItem("Banana"), 2);	   //Set banana item data
	std::shared_ptr<RenderComponent> bananaRenderComponent = std::make_shared<RenderComponent>();   // Create banana render component
	bananaRenderComponent->Init(foodMaterial, pbrEffect, ModelManager::Instance().GetModel("Food_Banana")); // Init render component
	bananaInstance->AddComponent(MyComponent::ComponentType::Render, bananaRenderComponent);	   // Add banana render component to banana instance
	RegisterSceneObject(bananaInstance);	// シーンに登録


	GameObject* floor = CreateObj<GameObject>("Floor");
	std::shared_ptr<RenderComponent> floorRenderComponent = std::make_shared<RenderComponent>();
	floorRenderComponent->Init(floorMaterial, basicEffect, ModelManager::Instance().GetModel("Cube"));
	floor->AddComponent(MyComponent::ComponentType::Render, floorRenderComponent);
	

	//===========UI初期化
	UIRender* uiAim = CreateObj<UIRender>("UiAim");
	uiAim->Init(uiAimMaterial, uiBasicEffect, ModelManager::Instance().GetModel("Square"));
	uiAim->GetTransform().SetPosition({ 0,0,0.1f });
	uiAim->GetTransform().SetScale(UIAimSize);

	// Create UI Inventory
	UIFontSet* uiFontSet=GetObj<UIFontSet>("UIFontSet");
	UIBrush* uiBrush = GetObj<UIBrush>("UiBrush");
	UIInventory* uiInventory = CreateObj<UIInventory>("UiInventory");
	uiInventory->Init(player->GetInventory(), uiBasicEffect, uiInventoryBgMaterial, uiInventorySlotBgMaterial,uiInventorySlotMaterial, uiInventoryChosenSLotMaterial,ModelManager::Instance().GetModel("Square"),
		uiFontSet, "InventoryFont", uiBrush);
	uiInventory->LoadSizeAndPos("Assets/ConfigFile/UIConfig.json"); // Load position and size from config file
	uiInventory->SetPlayer(player); // Set player to inventory

	// Create UI Player Status
	UIPlayerStatus* uiPlayerStatus = CreateObj<UIPlayerStatus>("UiPlayerStatus");
	uiPlayerStatus->Init(hpMaterials, hungerMaterials, uiBasicEffect, ModelManager::Instance().GetModel("Square"));
	uiPlayerStatus->LoadPositionAndSize("Assets/ConfigFile/UIConfig.json"); // Load position and size from config file
	uiPlayerStatus->SetPlayer(player); // Set player to UI Player Status

	UIManager::Instance().ClearLayers();	// Clear existing UI layers

	UIManager::Instance().AddUiLayer("Aim", 3);
	UIManager::Instance().GetUILayer("Aim")->AddComponent(uiAim);

	UIManager::Instance().AddUiLayer("Inventory", 4);
	UIManager::Instance().GetUILayer("Inventory")->AddComponent(uiInventory);

	UIManager::Instance().AddUiLayer("PlayerStatus", 5);
	UIManager::Instance().GetUILayer("PlayerStatus")->AddComponent(uiPlayerStatus);

	//=====物理の初期化

	// Create the settings for the collision volume (the shape).

	// Create the shape

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.

	// Create the actual rigid body

	RefConst<Shape> defaultBoxSettings = ShapeFactory::Instance().GetOrCreateBox(RVec3(HalfUnitScale.x,HalfUnitScale.y,HalfUnitScale.z));
	

	std::shared_ptr<PhysicsComponent> appleCollider = make_shared<PhysicsComponent>();
	appleInstance->AddComponent(MyComponent::ComponentType::Physics, appleCollider);
	DirectX::XMFLOAT3 appleColliderSize = ModelManager::Instance().GetModel("Food_Apple")->GetModelSize();
	BodyCreationSettings appleBoxSettings(new BoxShape(RVec3(appleColliderSize.x * 0.5f * DefaultObjectScale, appleColliderSize.y * 0.5f * DefaultObjectScale, appleColliderSize.z * 0.5f * DefaultObjectScale)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM); //Init apple cube
	PhysicsManager::Instance().SetBodyCreationMass(1.f, appleBoxSettings);// Set the mass properties for the apple box
	appleCollider->Init(appleBoxSettings, EActivation::Activate);  //Create& Add
	appleInstance->GetTransform().SetScale(DefaultObjectScale, DefaultObjectScale, DefaultObjectScale);
	appleInstance->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->SetPosition(0, 3, 3);

	std::shared_ptr<PhysicsComponent> bananaCollider = make_shared<PhysicsComponent>();
	bananaInstance->AddComponent(MyComponent::ComponentType::Physics, bananaCollider);
	DirectX::XMFLOAT3 bananaColliderSize = ModelManager::Instance().GetModel("Food_Apple")->GetModelSize();
	BodyCreationSettings bananaBoxSettings(new BoxShape(RVec3(bananaColliderSize.x * 0.5f * DefaultObjectScale, bananaColliderSize.y * 0.5f * DefaultObjectScale, bananaColliderSize.z * 0.5f * DefaultObjectScale)), { 0,0,0, }, Quat::sIdentity(), EMotionType::Dynamic, Layers::ITEM); //Init apple cube
	PhysicsManager::Instance().SetBodyCreationMass(1.f, bananaBoxSettings);// Set the mass properties for the apple box
	bananaCollider->Init(bananaBoxSettings, EActivation::Activate);  //Create& Add
	bananaInstance->GetTransform().SetScale(DefaultObjectScale,DefaultObjectScale,DefaultObjectScale);
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


	//===========Set skybox camera
	GetObj<Skybox>("Skybox")->GetSkyboxEffect()->InitCamera(m_pCurrentCamera);

}

void SceneGame::UnInit()
{
	m_sceneObjects.clear(); // Clear all scene objects

}

void SceneGame::Update(float tick)
{
	//===============Camera Update


	//===============Handle Input
	if(KInput::IsKeyTrigger(VK_ESCAPE))
	{
		m_pSceneManager->SetCurrentScene("Title");
		return;
	}

	//===============Skybox Update
	

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
	UIManager::Instance().Update(tick);

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
	UIManager::Instance().Draw();

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
