#include "SceneGame.h"
#include <DirectXMath.h>
#include "d3dUtil.h"
#include "DayLight.h"
#include "DriftManager.h"
#include "GameApp.h"
#include "Geometry.h"
#include "ItemDataBase.h"
#include "KInput.h"
#include "MaterialManager.h"
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
	static constexpr DirectX::XMFLOAT4 DefaultLightDiffuse = { 0.5f,0.5f,0.5f,1.f }; // Default light diffuse color
	static constexpr DirectX::XMFLOAT4 DefaultLightColor = { 1.0f, 1.0f, 1.0, 1.0f }; // Ambient light color
	static constexpr float WaterWidth = 50.f;
	static constexpr float waterHeight = 1.f;

	//Collider Setting
	static constexpr DirectX::XMFLOAT3 HalfUnitScale = { 0.5f,0.5f,0.5f };	//Default Cube Size (length,height,width={1,1,1})
	static constexpr DirectX::XMFLOAT3 HalfFloorScale = { 5,0.1f,5 };

	static constexpr DirectX::XMFLOAT3 DefaultLightPosition = { 0,10,0 };	//Default Light Position

	static constexpr DirectX::XMFLOAT3 UIAimSize = { 32,32,1.f };

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
	TextureManager::Instance().LoadTextures("Assets/ConfigFile/Config.json");

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
	MaterialManager::Instance().RegisterMaterials("Assets/ConfigFile/Config.json");


	//===========Register food data
	ItemDataBase::Instance().LoadItemDataFromJsonFile("Assets/ConfigFile/ItemDataBase.json");

	//===========Init item
	std::shared_ptr<ItemInstance> appleInstance(ItemDataBase::Instance().CreateItemInstance("Apple",3));
	appleInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	appleInstance->SetPosition({ -3, 3, 0 });
	RegisterSceneObject(appleInstance);


	std::shared_ptr<ItemInstance> bananaInstance(ItemDataBase::Instance().CreateItemInstance("Banana",10));
	bananaInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	bananaInstance->SetPosition({ 3, 3, 3 });
	RegisterSceneObject(bananaInstance);


	std::shared_ptr<ItemInstance> coconutInstance (ItemDataBase::Instance().CreateItemInstance("Coconut"));
	coconutInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	coconutInstance->SetPosition({3, 3, 0});
	RegisterSceneObject(coconutInstance);

	GameObject* floor = CreateObj<GameObject>("Floor");
	std::shared_ptr<RenderComponent> floorRenderComponent = std::make_shared<RenderComponent>();
	floorRenderComponent->Init(MaterialManager::Instance().GetMaterial("FloorMaterial"), basicEffect, ModelManager::Instance().GetModel("Cube"));
	floor->AddComponent(MyComponent::ComponentType::Render, floorRenderComponent);

	// Drift Manager
	DriftManager* driftManager = CreateObj<DriftManager>("DriftManager");
	driftManager->Init(pbrEffect,player); // Initialize drift manager with PBR effect and player


	//===========UI初期化
	UIRender* uiAim = CreateObj<UIRender>("UiAim");
	uiAim->Init(MaterialManager::Instance().GetMaterial("UIAimMaterial"), uiBasicEffect, ModelManager::Instance().GetModel("Square"));
	uiAim->GetTransform().SetPosition({ 0,0,0.1f });
	uiAim->GetTransform().SetScale(UIAimSize);

	// Create UI Inventory
	UIFontSet* uiFontSet=GetObj<UIFontSet>("UIFontSet");
	UIBrush* uiBrush = GetObj<UIBrush>("UiBrush");
	UIInventory* uiInventory = CreateObj<UIInventory>("UiInventory");
	Material* uiInventoryBgMaterial = MaterialManager::Instance().GetMaterial("UiInventoryBgMaterial");
	Material* uiInventorySlotBgMaterial = MaterialManager::Instance().GetMaterial("UiInventorySlotBgMaterial");
	Material* uiInventorySlotMaterial = MaterialManager::Instance().GetMaterial("UiInventorySlotMaterial");
	Material* uiInventoryChosenSLotMaterial = MaterialManager::Instance().GetMaterial("UiInventoryChosenSlotMaterial");




	uiInventory->Init(player->GetInventory(), uiBasicEffect, uiInventoryBgMaterial, uiInventorySlotBgMaterial,uiInventorySlotMaterial, uiInventoryChosenSLotMaterial,ModelManager::Instance().GetModel("Square"),
		uiFontSet, "InventoryFont", uiBrush);
	uiInventory->LoadSizeAndPos("Assets/ConfigFile/UIConfig.json"); // Load position and size from config file
	uiInventory->SetPlayer(player); // Set player to inventory

	// Create UI Player Status
	UIPlayerStatus::MaterialList hpMaterials(3);
	hpMaterials[UIPlayerStatus::MaterialType::Icon] = MaterialManager::Instance().GetMaterial("HpIconMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Background] = MaterialManager::Instance().GetMaterial("HpBarBgMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Bar] = MaterialManager::Instance().GetMaterial("HpBarMaterial");
	UIPlayerStatus::MaterialList hungerMaterials(3);
	hungerMaterials[UIPlayerStatus::MaterialType::Icon] = MaterialManager::Instance().GetMaterial("HungerIconMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Background] = MaterialManager::Instance().GetMaterial("HungerBarBgMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Bar] = MaterialManager::Instance().GetMaterial("HungerBarMaterial");

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
	BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(HalfFloorScale.x,HalfFloorScale.y,HalfFloorScale.z)), Vec3().sZero(), Quat::sIdentity(), EMotionType::Dynamic, Layers::BOAT);
	std::shared_ptr<PhysicsComponent> floorCollider = make_shared<PhysicsComponent>();
	floorCollider->Init(floorBoxSettings, EActivation::Activate,floor);
	floor->AddComponent(MyComponent::ComponentType::Physics, floorCollider);
	floorCollider->SetGameObject(floor); // Set the GameObject for the PhysicsComponent
	floor->GetTransform().SetScale(HalfFloorScale * 2.f);

	// Init Buoyancy system
	BuoyancySystem* buoyancySystem = CreateObj<BuoyancySystem>("BuoyancySystem");
	buoyancySystem->Init(WaterWidth,waterHeight);


	//Collider Debug Render Component配置
	Material* debugMaterial = MaterialManager::Instance().GetMaterial("DebugMaterial");
	std::shared_ptr<RenderComponent> debugColliderRender = std::make_shared<RenderComponent>();
	debugColliderRender->Init(debugMaterial, debugEffect, ModelManager::Instance().GetModel("Capsule"));

	//Debug Collider Render ComponentをPlayerに追加
	player->AddComponent(MyComponent::ComponentType::DebugRender, debugColliderRender);


	//===========Set skybox camera
	GetObj<Skybox>("Skybox")->GetSkyboxEffect()->InitCamera(m_pCurrentCamera);

}

void SceneGame::UnInit()
{
	m_sceneObjects.clear(); // Clear all scene objects
	GetObj<DriftManager>("DriftManager")->UnInit(); 
	
	PhysicsManager::Instance().RemoveAllBodies(); 
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


	//===============Player Update
	GetObj<Player>("Player")->Update(tick);


	//===============Scene objects Update
	for(const auto& object:m_sceneObjects)
	{
		object->Update(tick);
	}

	//===============DriftManager Update
	GetObj<DriftManager>("DriftManager")->Update(tick); // Update drift manager


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

	GetObj<DriftManager>("DriftManager")->Draw(); // Draw drift manager items

	// Transparent Draw
	GetObj<Player>("Player")->Draw();

	// Ui描画
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
