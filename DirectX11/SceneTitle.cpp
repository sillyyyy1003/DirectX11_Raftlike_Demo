#include "SceneTitle.h"

#include "AudioManager.h"
#include "ModelManager.h"
#include "UIBasicEffect.h"
#include "UIButtonMove.h"
#include "UIGlossEffect.h"
#include "UIManager.h"
#include "d3dUtil.h"
#include "FirstPersonCamera.h"
#include "GameApp.h"
#include "KInput.h"
#include "RenderState.h"
#include "Skybox.h"
#include "SkyboxEffect.h"
#include "GameObject.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "Water.h"
#include "WaterEffect.h"

namespace 
{

	static constexpr DirectX::XMFLOAT3 TitleLogoPosition = { 200,120,0.1f };
	static constexpr DirectX::XMFLOAT3 LogoSize = { 512,83,1.f };
	static constexpr float TitleScale = 1.8f;

	static constexpr float RotateSpeed = 0.02f;
	static constexpr DirectX::XMFLOAT3 WaterSize = { 100,1,100 };
	static constexpr DirectX::XMFLOAT3 WaterPos = { 0,-5,0};

}

void SceneTitle::Init()
{
	PixelShader* uiElementPS = GetObj<PixelShader>("UIElementPS");
	VertexShader* uiElementVS = GetObj<VertexShader>("UIElementVS");
	VertexShader* waterVS = GetObj<VertexShader>("WaterVS");
	PixelShader* waterPS = GetObj<PixelShader>("WaterPS");

	//==========Init Effect
	UIBasicEffect* uiBasicEffect = CreateObj<UIBasicEffect>("UiBasicEffect");
	uiBasicEffect->InitPixelShader(uiElementPS);
	uiBasicEffect->InitVertexShader(uiElementVS);

	//==========Load Texture
	Texture* buttonStartTex = CreateObj<Texture>("ButtonStartTex");
	buttonStartTex->Create("Assets/Texture/UI/UI_Button_Start_300x109.png");
	Texture* buttonOptionTex = CreateObj<Texture>("ButtonOptionTex");
	buttonOptionTex->Create("Assets/Texture/UI/UI_Button_Option_300x109.png");
	Texture* buttonExitTex = CreateObj<Texture>("ButtonExitTex");
	buttonExitTex->Create("Assets/Texture/UI/UI_Button_Exit_300x109.png");
	Texture* titleLogoTex = CreateObj<Texture>("TitleLogoTex");
	titleLogoTex->Create("Assets/Texture/UI/UI_Title_Logo_512x104.png");

	//==========Init Material
	Material* startMat = CreateObj<Material>("StartBgMaterial");
	startMat->SetTexture(Material::Albedo, buttonStartTex);

	Material* optionMat = CreateObj<Material>("OptionBgMaterial");
	optionMat->SetTexture(Material::Albedo, buttonOptionTex);
	
	Material* exitMat = CreateObj<Material>("ExitBgMaterial");
	exitMat->SetTexture(Material::Albedo, buttonExitTex);

	Material* titleLogoMat = CreateObj<Material>("TitleLogoMaterial");
	titleLogoMat->SetTexture(Material::Albedo, titleLogoTex);

	//===========Read scene config;
	std::ifstream ifs("Assets/ConfigFile/SceneConfig.json");
	assert(ifs.is_open());
	nlohmann::json j;
	ifs >> j;

	//===========Init Button
	UIButtonMove* startButton = CreateObj<UIButtonMove>("StartButton");
	startButton->Init(uiBasicEffect, startMat, ModelManager::Instance().GetModel("Square"));
	startButton->LoadButtonConfig(j["Title"]["UI"], "StartButton");

	UIButtonMove* optionButton = CreateObj<UIButtonMove>("OptionButton");
	optionButton->Init(uiBasicEffect, optionMat, ModelManager::Instance().GetModel("Square"));
	optionButton->LoadButtonConfig(j["Title"]["UI"], "OptionButton");

	UIButtonMove* exitButton = CreateObj<UIButtonMove>("exitButton");
	exitButton->Init(uiBasicEffect, exitMat, ModelManager::Instance().GetModel("Square"));
	exitButton->LoadButtonConfig(j["Title"]["UI"], "ExitButton");

	UIRender* titleLogo = CreateObj<UIRender>("TitleLogo");
	titleLogo->SetEffect(uiBasicEffect);
	titleLogo->SetMaterial(titleLogoMat);
	titleLogo->SetModel(ModelManager::Instance().GetModel("Square"));
	titleLogo->GetTransform().SetPosition(TitleLogoPosition);
	titleLogo->GetTransform().SetScale(LogoSize * TitleScale);

	//===========Register button to ui manager
	UIManager::Instance().ClearLayers();
	UIManager::Instance().AddUiLayer("Button", 1);
	UIManager::Instance().GetUILayer("Button")->AddComponent(startButton);
	UIManager::Instance().GetUILayer("Button")->AddComponent(optionButton);
	UIManager::Instance().GetUILayer("Button")->AddComponent(exitButton);

	UIManager::Instance().AddUiLayer("TitleLogo", 2);
	UIManager::Instance().GetUILayer("TitleLogo")->AddComponent(titleLogo);

	//===========Set Button Event
	startButton->SetOnClick([this]()
		{
			AudioManager::Instance().Play("Button", false);
			SetCurrentScene("Game");
		});
	startButton->SetOnHover([startButton]()
		{
			startButton->ActiveMove();
		});
	startButton->SetOnExit([startButton]()
		{
			startButton->DeActiveMove();
		});

	optionButton->SetOnClick([this]()
		{
			AudioManager::Instance().Play("Button", false);
			SetCurrentScene("Option");
		});
	optionButton->SetOnHover([optionButton, startButton]()
		{
			optionButton->ActiveMove();
			startButton->DeActiveMove();
		});
	optionButton->SetOnExit([optionButton]()
		{
			optionButton->DeActiveMove();
		});

	exitButton->SetOnClick([this]()
		{
			PostQuitMessage(0);
		});
	exitButton->SetOnHover([exitButton,startButton]()
		{
			exitButton->ActiveMove();
			startButton->DeActiveMove();
		});
	exitButton->SetOnExit([exitButton]()
		{
			exitButton->DeActiveMove();
		});

	startButton->ActiveMove();

	//=======Init Camera
	FirstPersonCamera* camera = CreateObj<FirstPersonCamera>("Camera");
	camera->SetPosition({ 0,0,-10 });
	camera->SetTarget({ 0,0,0 });
	GetObj<Skybox>("Skybox")->GetSkyboxEffect()->InitCamera(camera);
	m_pCurrentCamera = camera;

	//=======Init Water
	Texture* waterNormalMap = TextureManager::Instance().GetTexture("WaterNormalMap");
	Material* waterMaterial = MaterialManager::Instance().GetMaterial("WaterMaterial");
	DirLight* light = GetObj<DirLight>("SystemLight");
	// init water effect
	WaterEffect* waterEffect = CreateObj<WaterEffect>("WaterEffect");
	waterEffect->Init(waterVS, waterPS, m_pCurrentCamera, waterNormalMap, light);
	// init water mesh
	PlaneMesh* waterMesh = CreateObj<PlaneMesh>("WaterMesh");
	const uint waterSlice = 50;	// mesh slice which leads to 51x51 
	waterMesh->Init(waterSlice, 1);
	// create render component
	std::shared_ptr<RenderComponent> waterRender = std::make_shared<RenderComponent>();
	waterRender->Init(waterMaterial, waterEffect, waterMesh);
	// create water
	Water* water = CreateObj<Water>("Water");
	water->LoadFromConfig(j["Title"], "Water");
	water->AddComponent<RenderComponent>(MyComponent::ComponentType::Render, waterRender);
	water->GetTransform().SetPosition(WaterPos);
	water->GetTransform().SetScale( WaterSize);

	AudioManager::Instance().StopBgms();
	AudioManager::Instance().Play("BGM1", true);
	AudioManager::Instance().Play("WaveBackGround", true);
}

void SceneTitle::UnInit()
{
}

void SceneTitle::Update(float tick)
{

	// Handle Input
	if(KInput::IsKeyTrigger(VK_ESCAPE))
	{
		m_pSceneManager->RemoveSubScene();
		return;
	}


	// Camera Update
	m_pCurrentCamera->m_transform.Rotate({ 0,tick * RotateSpeed,0 });

	GetObj<Skybox>("Skybox")->Update(tick);

	// Update water param
	GetObj<Water>("Water")->GetTransform().Rotate({0,tick * RotateSpeed,0});	// rotate with camera
	GetObj<Water>("Water")->Update(tick);

	// Button Update
	UIManager::Instance().Update(tick);

}

void SceneTitle::Draw()
{
	// Draw skybox
	GetObj<Skybox>("Skybox")->Draw();

	// Draw Water
	GetObj<Water>("Water")->Draw();

	// Draw Buttons
	GameApp::SetDepthStencilState(RenderStates::DSSNoDepthTest);
	UIManager::Instance().Draw();
}
