#include "SceneGame.h"
#include <DirectXMath.h>
#include "AudioManager.h"
#include "CraftSystem.h"
#include "d3dUtil.h"
#include "DayLight.h"
#include "DriftManager.h"
#include "GameApp.h"
#include "HookInstance.h"
#include "ItemDataBase.h"
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
#include "UICharge.h"
#include "UICraftCategoryPanel.h"
#include "UICraftDetailPanel.h"
#include "UIInventory.h"
#include "UIMenu.h"
#include "UIPlayerStatus.h"
#include "UIChargeEffect.h"
#include "Water.h"
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
	static constexpr DirectX::XMFLOAT3 UIAimSize = {40,40,1.f };
	static constexpr DirectX::XMFLOAT2 UIChargeSize = {50, 50};

	static constexpr DirectX::XMFLOAT4 WaterBoxColor= { 0.027f, 0.016f, 0.337f, 1.0f };	// Dark blue water box
	static constexpr DirectX::XMFLOAT3 WaterBoxPos = { 0,-waterHeight / 2.f - 0.1f,0 };	// water face is -0.1f;

	static constexpr DirectX::XMFLOAT3 DeathTextPos={ 0,100,1.0 };
	static constexpr DirectX::XMFLOAT3 DeathTextScale = { 800,100,1 };

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
	PixelShader* uiRangePS = GetObj<PixelShader>("UIRangePS");
	PixelShader* monoChromePS = GetObj<PixelShader>("MonoChromePS");


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

	UIBasicEffect* uiBasicEffect = GetObj<UIBasicEffect>("UiBasicEffect");
	uiBasicEffect->InitPixelShader(uiElementPS);
	uiBasicEffect->InitVertexShader(uiElementVS);

	BasicEffect* debugEffect = CreateObj<BasicEffect>("DebugEffect");
	debugEffect->InitPixelShader(basicBlinnPhongPS);
	debugEffect->InitVertexShader(basicPosNormalTexVS);
	debugEffect->InitCamera(player->GetCameraController()->GetCamera());
	debugEffect->InitEffectDirLight(light);

	UIChargeEffect* uiChargeEffect = CreateObj<UIChargeEffect>("UiRangeEffect");
	uiChargeEffect->InitPixelShader(uiRangePS);
	uiChargeEffect->InitVertexShader(uiElementVS);

	Texture* waterNormalMap = TextureManager::Instance().GetTexture("WaterNormalMap");
	WaterEffect* waterEffect = CreateObj<WaterEffect>("WaterEffect");
	waterEffect->Init(waterVS, waterPS, m_pCurrentCamera, waterNormalMap,light);


	//=========== Register Item data
	ItemDataBase::Instance().LoadItemDataFromJsonFile("Assets/ConfigFile/ItemDataBase.json");

	//=========== Drift Manager
	DriftManager* driftManager = CreateObj<DriftManager>("DriftManager");
	driftManager->Init(pbrEffect, player); // Initialize drift manager with PBR effect and player

	//=========== Init item
	std::shared_ptr<ItemInstance> appleInstance(ItemDataBase::Instance().CreateItemInstanceToWorldWithPhysics("Apple", 3,  Layers::ITEM));
	appleInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	appleInstance->SetPosition({ -3, 0.5, 0 });
	RegisterSceneObject(appleInstance);

	std::shared_ptr<ItemInstance> bananaInstance(ItemDataBase::Instance().CreateItemInstanceToWorldWithPhysics("Banana",10, Layers::ITEM));
	bananaInstance->GetComponent<RenderComponent>(MyComponent::ComponentType::Render)->SetEffect(pbrEffect);
	bananaInstance->SetPosition({ 3, 0.5, 3 });
	RegisterSceneObject(bananaInstance);

	GameObject* floor = CreateObj<GameObject>("Floor");
	std::shared_ptr<RenderComponent> floorRenderComponent = std::make_shared<RenderComponent>();
	floorRenderComponent->Init(MaterialManager::Instance().GetMaterial("FloorMaterial"), basicEffect, ModelManager::Instance().GetModel("Cube"));
	floor->AddComponent(MyComponent::ComponentType::Render, floorRenderComponent);

	GameObject* underWaterBox = CreateObj<GameObject>("UnderWaterBox");
	std::shared_ptr<RenderComponent> underWaterBoxRenderComponent = std::make_shared<RenderComponent>();
	Material* underWaterBoxMat = CreateObj<Material>("UnderSea");
	underWaterBoxMat->SetDiffuse(WaterBoxColor);
	underWaterBoxRenderComponent->Init(underWaterBoxMat, basicEffect, ModelManager::Instance().GetModel("Environment_WaterBox"));
	underWaterBox->AddComponent(MyComponent::ComponentType::Render, underWaterBoxRenderComponent);
	underWaterBox->GetTransform().SetScale({ WaterWidth/2.f,waterHeight/2.f,WaterWidth/2.f}); //water box basic size{2,2,2} 
	underWaterBox->GetTransform().SetPosition(WaterBoxPos);


	//=====物理の初期化

	// Create the settings for the collision volume (the shape).
	// Create the shape
	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
	// Create the actual body
	BodyCreationSettings floorBoxSettings(new BoxShape(RVec3(HalfFloorScale.x, HalfFloorScale.y, HalfFloorScale.z)), Vec3().sZero(), Quat::sIdentity(), EMotionType::Dynamic, Layers::BOAT);
	std::shared_ptr<PhysicsComponent> floorCollider = make_shared<PhysicsComponent>();
	floor->AddComponent(MyComponent::ComponentType::Physics, floorCollider);
	floorCollider->Init(floorBoxSettings, EActivation::Activate, floor);
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

	// Charge ui
	UICharge* uiCharge = CreateObj<UICharge>("UiCharge");
	Material* uiChargeMat = MaterialManager::Instance().GetMaterial("UIChargeMaterial");
	uiCharge->Init(uiChargeMat, uiChargeEffect, ModelManager::Instance().GetModel("Square"));
	uiCharge->SetPosition({ 0,0,0.2f });
	uiCharge->SetScale(UIChargeSize);
	player->SetUiCharge(uiCharge);	// Set player charger ui

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
	deathText->SetPosition(DeathTextPos);
	deathText->SetScale(DeathTextScale);
	deathText->SetStaticText("You Are Incapacitated!");
	deathText->SetTextAlignment(UIText::TextAlign::Center);
	deathText->SetActive(false);
	// death text only activate when player is dead
	player->AddDeathListener([deathText](bool isDead)
		{
			deathText->SetActive(isDead);
		}
	);

	// Create Death Button

	UIButtonMove* backToTitleButton = CreateObj<UIButtonMove>("backToTitleButton");
	Material* backToTitleMat = MaterialManager::Instance().GetMaterial("UiBackToTitleButtonMaterial");
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
	Material* reviveButtonMat = MaterialManager::Instance().GetMaterial("UiReviveButtonMaterial");
	reviveButton->Init(uiBasicEffect, reviveButtonMat, ModelManager::Instance().GetModel("Square"));
	 // set button interactive param
	reviveButton->LoadButtonConfig(j["Game"]["UI"], "ReviveButton");
	player->AddDeathListener([reviveButton](bool isDead)
		{
			reviveButton->SetActive(isDead);
			if (isDead)reviveButton->ActiveMove();	//if is active, make revive button move
		}
	);
	reviveButton->SetActive(false);

	UIButtonMove* configButton = CreateObj<UIButtonMove>("ConfigButton");
	Material* configButtonMaterial= MaterialManager::Instance().GetMaterial("UiConfigButtonMaterial");
	configButton->Init(uiBasicEffect, configButtonMaterial, ModelManager::Instance().GetModel("Square"));
	configButton->LoadButtonConfig(j["Game"]["UI"], "ConfigButton");

	// Craft system
	CraftRecipe* recipe = CreateObj<CraftRecipe>("Recipe");
	recipe->Init("Apple", { { "Banana",4 } });

	CraftSystem::Instance().Init(player->GetInventory());
	CraftSystem::Instance().LoadRecipes("Assets/ConfigFile/CraftRecipes.json");
	CraftSystem::Instance().LoadCraftData("Assets/ConfigFile/CraftSystemData.json");
	

	//UI Craft panel // Third layer detail
	UICraftDetailPanel* craftPanel = CreateObj<UICraftDetailPanel>("UICraftDetailPanel");
	craftPanel->InitFonts(uiFontSet,uiBrush,"CraftPanelTitleFont","CraftPanelDescriptionFont","CraftPanelRequiresFont","CraftPanelIngredientNameFont","CraftPanelNumberFont");
	Material* panelBgMat = MaterialManager::Instance().GetMaterial("CraftPanelBackgroundMaterial");
	Material* panelButtonMat = MaterialManager::Instance().GetMaterial("CraftPanelButtonMaterial");
	Material* panelIconMat = CreateObj<Material>("PanelIconMaterial");
	Material* panelIconBgMat = CreateObj<Material>("PanelIconBgMaterial");
	craftPanel->InitRender(panelBgMat,panelButtonMat, uiInventorySlotBgMaterial,panelIconMat,uiBasicEffect,ModelManager::Instance().GetModel("Square"));
	craftPanel->LoadPanelConfig(j["Game"]["UI"], "CraftPanel");

	// Second layer category detail panel
	UICraftCategoryDetailPanel* craftCategoryDetailPanel = CreateObj<UICraftCategoryDetailPanel>("UICraftCategoryDetailPanel");

	Material* categoryDetailPanelMaterial = CreateObj<Material>("CategoryDetailPanelMaterial");
	craftCategoryDetailPanel->Init(uiFontSet, uiBrush, "CraftCategoryPanelFont", "CraftCategoryPanelSlotFont", panelBgMat, categoryDetailPanelMaterial, uiBasicEffect, ModelManager::Instance().GetModel("Square"));
	craftCategoryDetailPanel->InitPosAndSize({ -845,400,1.0 }, 260.f, { 240,50 }, { 64,64 });
	craftCategoryDetailPanel->SetDetailPanel(craftPanel);


	// UI Craft Category
	Material* categoryPanelBgRenderMat = MaterialManager::Instance().GetMaterial("UiCraftCategoryPanelMaterial");
	Material* categoryPanelBgMat = MaterialManager::Instance().GetMaterial("UiCraftCategoryPanelIconBackgroundMaterial");
	Material* categoryIconMat = MaterialManager::Instance().GetMaterial("UiCraftCategoryPanelIconMaterial");

	UICraftCategoryPanel* categoryPanel = CreateObj<UICraftCategoryPanel>("UICraftCategoryPanel");
	categoryPanel->Init(categoryPanelBgRenderMat,categoryPanelBgMat,categoryIconMat,uiBasicEffect, ModelManager::Instance().GetModel("Square"));
	categoryPanel->LoadSizeAndPos(j["Game"]["UI"], "CraftCategoryPanel");
	categoryPanel->SetPanels(craftCategoryDetailPanel, craftPanel);

	


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
			ShowCursor(FALSE);
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


	configButton->SetOnClick([this, configButton]
		{
			AudioManager::Instance().Play("SE_Button", false);
			GetObj<UIMenu>("UIMenu")->SetActive(true);	// Enable Menu
			configButton->DeActiveMove();
		});
	configButton->SetOnHover([configButton]
		{
			configButton->ActiveMove();
		});
	configButton->SetOnExit([configButton]
		{
			configButton->DeActiveMove();
		});
	player->AddDeathListener([configButton](bool isDead)
		{
			configButton->SetActive(!isDead);
		}
	);

	//===========Register ui layers
	UIManager::Instance().AddUiLayer("Aim", 1);
	UIManager::Instance().GetUILayer("Aim")->AddComponent(uiAim);
	UIManager::Instance().GetUILayer("Aim")->AddComponent(uiCharge);

	UIManager::Instance().AddUiLayer("Button", 2);
	UIManager::Instance().GetUILayer("Button")->AddComponent(backToTitleButton);
	UIManager::Instance().GetUILayer("Button")->AddComponent(reviveButton);
	UIManager::Instance().GetUILayer("Button")->AddComponent(deathText);
	UIManager::Instance().GetUILayer("Button")->AddComponent(configButton);

	UIManager::Instance().AddUiLayer("Player", 3);
	UIManager::Instance().GetUILayer("Player")->AddComponent(uiPlayerStatus);
	UIManager::Instance().GetUILayer("Player")->AddComponent(uiInventory);	// note that ui inventory is has lower priority than craft system panel

	UIManager::Instance().AddUiLayer("CraftPanel", 4);
	UIManager::Instance().GetUILayer("CraftPanel")->AddComponent(categoryPanel);
	UIManager::Instance().GetUILayer("CraftPanel")->AddComponent(craftCategoryDetailPanel);
	UIManager::Instance().GetUILayer("CraftPanel")->AddComponent(craftPanel);

	// Menu Setup
	GetObj<UIMenu>("UIMenu")->SetButton(this);

	
	//===========Set skybox camera
	GetObj<Skybox>("Skybox")->GetSkyboxEffect()->InitCamera(m_pCurrentCamera);

	//===========Hide cursor
#ifdef NDEBUG
	ShowCursor(FALSE);
#endif
	//===========Set Sound
	AudioManager::Instance().StopBgms();
	AudioManager::Instance().Play("BGM_Game", true);

	std::shared_ptr<ItemInstance> hook = ItemDataBase::Instance().CreateItemInstance("NormalHook", 1);
	// Set player item effect
	player->GetInventory()->SetItemEffectPtr(pbrEffect);

	// Insert hook to player inventory
	player->GetInventory()->Insert(hook.get());

	player->GetInventory()->UpdateItemOfPlayer(player);
}

void SceneGame::UnInit()
{
	m_sceneObjects.clear(); // Clear all scene objects
	GetObj<DriftManager>("DriftManager")->UnInit(); 

	CraftSystem::Instance().UnInit();	// Clear all recipes
	
}

void SceneGame::Update(float tick)
{
	//===============Scene Change
	if(m_isChangeScene)
	{
		SceneBase::SetCurrentScene(m_sceneName.c_str());
		return;
	}

	//===============Handle Input
	if (KInput::IsKeyRelease(VK_LCONTROL))
	{
		if (!m_isShowCursor)
		{
			ShowCursor(TRUE);	// Change cursor status
			GetObj<Player>("Player")->LockCursor(false);	// Unlock cursor
			m_isShowCursor = !m_isShowCursor;	
		}
		else
		{
			ShowCursor(FALSE);// Change cursor status
			GetObj<Player>("Player")->LockCursor(true);	// Lock cursor
			m_isShowCursor = !m_isShowCursor;
		}
	}

	


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
	GetObj<GameObject>("UnderWaterBox")->Draw();

	// Transparent Draw
	GetObj<BuoyancySystem>("BuoyancySystem")->Draw();
	GetObj<Player>("Player")->Draw();


	GetObj<MonoChrome>("MonoChrome")->DrawRenderTarget();
	



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

	// Remove layers in this scene
	UIManager::Instance().RemoveLayer("Aim");
	UIManager::Instance().RemoveLayer("Button");
	UIManager::Instance().RemoveLayer("Player");
	UIManager::Instance().RemoveLayer("CraftPanel");
}

