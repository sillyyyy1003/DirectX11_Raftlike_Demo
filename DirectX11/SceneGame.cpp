#include "SceneGame.h"
#include <DirectXMath.h>

#include "AudioManager.h"
#include "CraftSystem.h"
#include "d3dUtil.h"
#include "DayLight.h"
#include "DriftManager.h"
#include "GameApp.h"
#include "ItemDataBase.h"
#include "ItemInstances.h"
#include "KInput.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "PBREffect.h"
#include "PhysicsManager.h"
#include "Player.h"
#include "PostProcessMonochromeEffect.h"
#include "RenderState.h"
#include "SceneManager.h"
#include "ShapeFactory.h"
#include "SkyboxEffect.h"
#include "UIBasicEffect.h"
#include "UIManager.h"
#include "Skybox.h"
#include "TextureManager.h"
#include "UIButtonMove.h"
#include "UIInventorySlot.h"
#include "UIPlayerStatus.h"
#include "WaterEffect.h"

namespace
{
	static constexpr DirectX::XMFLOAT4 DefaultLightDiffuse = { 0.5f,0.5f,0.5f,1.f }; // Default light diffuse color
	static constexpr DirectX::XMFLOAT4 DefaultLightColor = { 1.0f, 1.0f, 1.0, 1.0f }; // Ambient light color
	static constexpr float WaterWidth = 200.f;
	static constexpr float waterHeight = 20.f;

	//Collider Setting
	static constexpr DirectX::XMFLOAT3 HalfFloorScale = { 5,0.1f,5 };
	static constexpr DirectX::XMFLOAT3 DefaultLightPosition = { 0,10,0 };	//Default Light Position
	static constexpr DirectX::XMFLOAT3 UIAimSize = { 32,32,1.f };

}
void SceneGame::Init()
{
	//============Load Scene data
	std::ifstream ifs("Assets/ConfigFile/SceneConfig.json");
	assert(ifs.is_open());
	nlohmann::json j;
	ifs >> j;

	//============Init Player
	Player* player = CreateObj<Player>("Player");
	player->Init("Assets/ConfigFile/PlayerConfig.json");	//json fileから読み込み
	m_pCurrentCamera = player->GetCameraController()->GetCamera();
	player->SetPosition({ 0,1,0 });
	dynamic_cast<SceneManager*>(m_pSceneManager)->SetCurrentCamera(m_pCurrentCamera);

	//============ Init light
	DirLight* light = CreateObj<DayLight>("DayLight");
	light->SetPosition(DefaultLightPosition);
	light->SetAmbient(DefaultLightColor);
	light->SetDiffuse({0.5,0.5,0.5,1});

	//============Get Shader
	VertexShader* basicPosNormalTexVS = GetObj<VertexShader>("BasicPosNormalTexVS");
	PixelShader* basicBlinnPhongPS = GetObj<PixelShader>("BasicBlinnPhongPS");
	PixelShader* pbrPS = GetObj<PixelShader>("PBRPS");
	VertexShader* pbrVS = GetObj<VertexShader>("PBRVS");
	PixelShader* uiElementPS = GetObj<PixelShader>("UIElementPS");
	VertexShader* uiElementVS = GetObj<VertexShader>("UIElementVS");
	VertexShader* waterVS = GetObj<VertexShader>("WaterVS");
	PixelShader* waterPS = GetObj<PixelShader>("WaterPS");

	PixelShader* monoChromePS = CreateObj<PixelShader>("MonoChromePS");
	monoChromePS->Load("Assets/Shader/PS_Monochrome.cso");

	// Post Effect
	MonoChrome* monoChrome = CreateObj<MonoChrome>("MonoChrome");
	monoChrome->Init(monoChromePS);

	// register monochrome effect on player dead
	player->AddDeathListener([monoChrome](bool isDead)
		{
			monoChrome->SetActive(isDead);
		}
	);


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

	Texture* waterNormalMap = TextureManager::Instance().GetTexture("WaterNormalMap");
	WaterEffect* waterEffect = CreateObj<WaterEffect>("WaterEffect");
	waterEffect->Init(waterVS, waterPS, m_pCurrentCamera, waterNormalMap,light);


	//===========Register food data
	ItemDataBase::Instance().LoadItemDataFromJsonFile("Assets/ConfigFile/ItemDataBase.json");


	//===========Init item
	std::shared_ptr<ItemInstance> appleInstance(ItemDataBase::Instance().CreateItemInstanceToWorldWithPhysics("Apple", 3, -1, Layers::ITEM));
	appleInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	appleInstance->SetPosition({ -3, 0.5, 0 });
	RegisterSceneObject(appleInstance);
	DebugLog::Log("AppleInstance : bodyIndex:{}",appleInstance->GetComponent<PhysicsComponent>(MyComponent::ComponentType::Physics)->GetBodyID().GetIndex());

	std::shared_ptr<ItemInstance> bananaInstance(ItemDataBase::Instance().CreateItemInstanceToWorldWithPhysics("Banana",10, -1, Layers::ITEM));
	bananaInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	bananaInstance->SetPosition({ 3, 0.5, 3 });
	RegisterSceneObject(bananaInstance);

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
	// death text only activate when player is dead
	player->AddDeathListener([uiAim](bool isDead)
		{
			uiAim->SetActive(!isDead);	//if player revive, isDead=false
		}
	);

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
	//uiInventory->LoadSizeAndPos("Assets/ConfigFile/UIConfig.json"); // Load position and size from config file
	uiInventory->LoadSizeAndPos(j["Game"]["UI"],"UIInventory");
	uiInventory->SetPlayer(player); // Set player to inventory
	// Hide inventory when player is dead
	player->AddDeathListener([uiInventory](bool isDead)
		{
			uiInventory->SetActive(!isDead);
		}
	);


	// Create UI Player Status
	UIPlayerStatus::MaterialList hpMaterials(3);
	hpMaterials[UIPlayerStatus::MaterialType::Icon] = MaterialManager::Instance().GetMaterial("HpIconMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Background] = MaterialManager::Instance().GetMaterial("HpBarBgMaterial");
	hpMaterials[UIPlayerStatus::MaterialType::Bar] = MaterialManager::Instance().GetMaterial("HpBarMaterial");

	UIPlayerStatus::MaterialList hungerMaterials(3);
	hungerMaterials[UIPlayerStatus::MaterialType::Icon] = MaterialManager::Instance().GetMaterial("HungerIconMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Background] = MaterialManager::Instance().GetMaterial("HungerBarBgMaterial");
	hungerMaterials[UIPlayerStatus::MaterialType::Bar] = MaterialManager::Instance().GetMaterial("HungerBarMaterial");

	UIPlayerStatus::MaterialList thirstMaterials(3);
	thirstMaterials[UIPlayerStatus::MaterialType::Icon] = MaterialManager::Instance().GetMaterial("ThirstIconMaterial");
	thirstMaterials[UIPlayerStatus::MaterialType::Background] = MaterialManager::Instance().GetMaterial("ThirstBarBgMaterial");
	thirstMaterials[UIPlayerStatus::MaterialType::Bar] = MaterialManager::Instance().GetMaterial("ThirstBarMaterial");


	UIPlayerStatus* uiPlayerStatus = CreateObj<UIPlayerStatus>("UiPlayerStatus");
	uiPlayerStatus->Init(hpMaterials, hungerMaterials, thirstMaterials, uiBasicEffect, ModelManager::Instance().GetModel("Square"));
	uiPlayerStatus->LoadPositionAndSize(j["Game"]["UI"], "UIPlayerStatus");
	uiPlayerStatus->SetPlayer(player); // Set player to UI Player Status

	// Hide PlayerStatus when player is dead
	player->AddDeathListener([uiPlayerStatus](bool isDead)
		{
			uiPlayerStatus->SetActive(!isDead);
		}
	);


	// Create Death UI
	UIText* deathText = CreateObj<UIText>("DeathText");
	deathText->Init(uiFontSet, "DeathFont", uiBrush);
	deathText->SetPosition({ 0,100,1.0 });
	deathText->SetScale({800,100,1});
	deathText->SetStaticText("You Are Incapacitated!");
	deathText->SetActive(false);
	// death text only activate when player is dead
	player->AddDeathListener([deathText](bool isDead)
		{
			deathText->SetActive(isDead);
		}
	);

	// Create Death Button

	UIButtonMove* backToTitleButton = CreateObj<UIButtonMove>("backToTitleButton");
	Material* backToTitleMat = MaterialManager::Instance().GetMaterial("UiBackToTitleButton");
	backToTitleButton->Init(uiBasicEffect, backToTitleMat, ModelManager::Instance().GetModel("Square"));
	// Load button interactive param
	//backToTitleButton->LoadButtonConfig("Assets/ConfigFile/UIConfig.json", "BackToTitleButton");
	backToTitleButton->LoadButtonConfig(j["Game"]["UI"], "BackToTitleButton");
	player->AddDeathListener([backToTitleButton](bool isDead)
		{
			backToTitleButton->SetActive(isDead);
		}
	);
	backToTitleButton->SetActive(false);

	UIButtonMove* reviveButton = CreateObj<UIButtonMove>("reviveButton");
	Material* reviveButtonMat = MaterialManager::Instance().GetMaterial("UiReviveButton");
	reviveButton->Init(uiBasicEffect, reviveButtonMat, ModelManager::Instance().GetModel("Square"));
	 // set button interactive param
	//reviveButton->LoadButtonConfig("Assets/ConfigFile/UIConfig.json", "ReviveButton");
	reviveButton->LoadButtonConfig(j["Game"]["UI"], "ReviveButton");
	player->AddDeathListener([reviveButton](bool isDead)
		{
			reviveButton->SetActive(isDead);
			if (isDead)reviveButton->ActiveMove();	//if is active, make revive button move
		}
	);
	reviveButton->SetActive(false);
	UIManager::Instance().ClearLayers();	// Clear existing UI layers

	UIManager::Instance().AddUiLayer("Aim", 1);
	UIManager::Instance().GetUILayer("Aim")->AddComponent(uiAim);

	UIManager::Instance().AddUiLayer("Button", 2);
	UIManager::Instance().GetUILayer("Button")->AddComponent(backToTitleButton);
	UIManager::Instance().GetUILayer("Button")->AddComponent(reviveButton);

	UIManager::Instance().AddUiLayer("Player", 3);
	UIManager::Instance().GetUILayer("Player")->AddComponent(uiPlayerStatus);
	UIManager::Instance().GetUILayer("Player")->AddComponent(uiInventory);	// note that ui inventory is has lower priority than craft system panel

	UIManager::Instance().AddUiLayer("Message", 4);
	UIManager::Instance().GetUILayer("Message")->AddComponent(deathText);
	UIManager::Instance().AddUiLayer("CraftSystem", 5);
	




	//=====Set Button event

	backToTitleButton->SetOnClick([this]()
		{
			SetCurrentScene("Title");	//if clicked back to title scene
		});
	backToTitleButton->SetOnHover([backToTitleButton, reviveButton]()
		{
			backToTitleButton->ActiveMove();
			reviveButton->DeActiveMove();
		});
	backToTitleButton->SetOnExit([backToTitleButton]()
		{
			backToTitleButton->DeActiveMove();
		});


	reviveButton->SetOnClick([player]()
		{
			player->Revive();//if clicked player revive
		});
	reviveButton->SetOnHover([backToTitleButton, reviveButton]()
		{
			reviveButton->ActiveMove();
			backToTitleButton->DeActiveMove();
		});
	reviveButton->SetOnExit([reviveButton]()
		{
			reviveButton->DeActiveMove();
		});


	//=====物理の初期化

	// Create the settings for the collision volume (the shape).
	// Create the shape
	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
	// Create the actual body
	BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(HalfFloorScale.x,HalfFloorScale.y,HalfFloorScale.z)), Vec3().sZero(), Quat::sIdentity(), EMotionType::Dynamic, Layers::BOAT);
	std::shared_ptr<PhysicsComponent> floorCollider = make_shared<PhysicsComponent>();
	floorCollider->Init(floorBoxSettings, EActivation::Activate,floor);
	floor->AddComponent(MyComponent::ComponentType::Physics, floorCollider);
	floorCollider->SetGameObject(floor); // Set the GameObject for the PhysicsComponent
	floor->GetTransform().SetScale(HalfFloorScale * 2.f);


	// Init Buoyancy system
	BuoyancySystem* buoyancySystem = CreateObj<BuoyancySystem>("BuoyancySystem");
	Material* waterMaterial = MaterialManager::Instance().GetMaterial("WaterMaterial");
	buoyancySystem->Init(WaterWidth, waterHeight, waterMaterial, waterEffect);

	//Collider Debug Render Component配置
	Material* debugMaterial = MaterialManager::Instance().GetMaterial("DebugMaterial");
	std::shared_ptr<RenderComponent> debugColliderRender = std::make_shared<RenderComponent>();
	debugColliderRender->Init(debugMaterial, debugEffect, ModelManager::Instance().GetModel("Capsule"));

	//Debug Collider Render ComponentをPlayerに追加
	player->AddComponent(MyComponent::ComponentType::DebugRender, debugColliderRender);

	//===========Set skybox camera
	GetObj<Skybox>("Skybox")->GetSkyboxEffect()->InitCamera(m_pCurrentCamera);

	//===========Hide cursor
#ifdef NDEBUG
	ShowCursor(FALSE);
#endif
	//===========Set Sound
	AudioManager::Instance().StopBgms();
	AudioManager::Instance().Play("BGM2", true);


	//test
	std::shared_ptr<ItemInstance> bananaInBag = ItemDataBase::Instance().CreateItemInstance("Banana", 10);
	player->GetInventory()->Insert(bananaInBag.get());

	std::shared_ptr<ItemInstance> cup = ItemDataBase::Instance().CreateItemInstance("BasicCup", 1);
	player->GetInventory()->Insert(cup.get());

	CraftRecipe* recipe = CreateObj<CraftRecipe>("Recipe");
	recipe->Init("Apple", { { "Banana",4 } });

	CraftSystem* system = CreateObj<CraftSystem>("CraftSystem");
	system->Init(player->GetInventory());

}

void SceneGame::UnInit()
{
	m_sceneObjects.clear(); // Clear all scene objects
	GetObj<DriftManager>("DriftManager")->UnInit(); 
	
	PhysicsManager::Instance().RemoveAllBodies();
	ShowCursor(TRUE);
	
}

void SceneGame::Update(float tick)
{
	if(m_isChangeScene)
	{
		SceneBase::SetCurrentScene(m_sceneName.c_str());
		return;
	}

	
	//===============Camera Update


	//===============Handle Input

	if(KInput::IsKeyTrigger('K'))
	{
		GetObj<Player>("Player")->Kill();
	}


	//===============Skybox Update
	

	//===============Physics Update
	GetObj<BuoyancySystem>("BuoyancySystem")->PreUpdate(tick);
	PhysicsManager::Instance().Update(tick); // 物理システムの更新


	//===============Light Update
	GetObj<DayLight>("DayLight")->Update(tick);

	//===============Player Update
	GetObj<Player>("Player")->Update(tick);

	//===============Object Update
	GetObj<GameObject>("Floor")->Update(tick);

	//===============Scene objects Update
	for(const auto& object:m_sceneObjects)
	{
		object->Update(tick);
	}

	//===============DriftManager Update
	GetObj<DriftManager>("DriftManager")->Update(tick); // Update drift manager

	//===============PostProcess Update
	GetObj<MonoChrome>("MonoChrome")->Update(tick);

	//===============UI Update
	UIManager::Instance().Update(tick);


	//===============Clear all inactive game objects
	DeleteInactiveSceneObject();
}

void SceneGame::Draw()
{
	GetObj<MonoChrome>("MonoChrome")->PreDraw();

	GetObj<Skybox>("Skybox")->Draw();

	//============Draw GameObjects
	GameApp::SetDepthStencilState(RenderStates::DSSLessEqual);
	GetObj<GameObject>("Floor")->Draw();

	for (const auto& object : m_sceneObjects)
	{
		object->Draw();
	}

	GetObj<DriftManager>("DriftManager")->Draw(); // Draw drift manager items

	// Transparent Draw
	GetObj<BuoyancySystem>("BuoyancySystem")->Draw();
	GetObj<Player>("Player")->Draw();

	GetObj<MonoChrome>("MonoChrome")->DrawRenderTarget();
	

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

void SceneGame::SetCurrentScene(const char* sceneName)
{
	m_isChangeScene = true;
	m_sceneName = sceneName;
}
