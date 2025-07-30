#include "SceneTitle.h"
#include "ModelManager.h"
#include "UIBasicEffect.h"
#include "UIButtonMove.h"
#include "UIGlossEffect.h"
#include "UIManager.h"
#include "d3dUtil.h"
#include "KInput.h"

namespace 
{
	static constexpr DirectX::XMFLOAT3 StartButtonPosition = { 450,-50,0.5f };
	static constexpr DirectX::XMFLOAT3 OptionButtonPosition = { 450,-160,0.5f };
	static constexpr DirectX::XMFLOAT3 ExitButtonPosition = { 450,-270,0.5f };
	static constexpr DirectX::XMFLOAT2 ButtonSize = { 300,109 };
	static constexpr DirectX::XMFLOAT3 TitleLogoPosition = { 200,120,0.1f };
	static constexpr DirectX::XMFLOAT3 LogoSize = { 512,83,1.f };
	static constexpr float ButtonAmplitude = 5.f;
	static constexpr float ButtonMoveSpeed = 3.f;
	static constexpr float ButtonScale = 0.8f;
	static constexpr float TitleScale =1.8f;

}

void SceneTitle::Init()
{
	PixelShader* uiElementPS = GetObj<PixelShader>("UIElementPS");
	VertexShader* uiElementVS = GetObj<VertexShader>("UIElementVS");
	PixelShader* uiGlossPS = GetObj<PixelShader>("UIGlossPS");
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

	//===========Init Button
	UIButtonMove* startButton = CreateObj<UIButtonMove>("StartButton");
	startButton->Init(uiBasicEffect, startMat, ModelManager::Instance().GetModel("Square"));
	startButton->SetButton(StartButtonPosition, ButtonSize.x * ButtonScale, ButtonSize.y * ButtonScale);	// Set button size & position
	startButton->InitMoveParam(ButtonMoveSpeed, ButtonAmplitude);	// set button interactive param


	UIButtonMove* optionButton = CreateObj<UIButtonMove>("OptionButton");
	optionButton->Init(uiBasicEffect, optionMat, ModelManager::Instance().GetModel("Square"));
	optionButton->SetButton(OptionButtonPosition, ButtonSize.x * ButtonScale, ButtonSize.y * ButtonScale);  // Set button size & position
	optionButton->InitMoveParam(ButtonMoveSpeed, ButtonAmplitude);// set button interactive param



	UIButtonMove* exitButton = CreateObj<UIButtonMove>("exitButton");
	exitButton->Init(uiBasicEffect, exitMat, ModelManager::Instance().GetModel("Square"));
	exitButton->SetButton(ExitButtonPosition, ButtonSize.x * ButtonScale, ButtonSize.y * ButtonScale);  // Set button size & position
	exitButton->InitMoveParam(ButtonMoveSpeed, ButtonAmplitude);	 // set button interactive param

	UIRender* titleLogo = CreateObj<UIRender>("TitleLogo");
	titleLogo->SetEffect(uiBasicEffect);
	titleLogo->SetMaterial(titleLogoMat);
	titleLogo->SetModel(ModelManager::Instance().GetModel("Square"));
	titleLogo->GetTransform().SetPosition(TitleLogoPosition);
	titleLogo->GetTransform().SetScale(LogoSize * TitleScale);

	//===========Register button to ui manager
	UIManager::GetInstance().AddUiLayer("Button", 1);
	UIManager::GetInstance().GetUILayer("Button")->AddComponent(startButton);
	UIManager::GetInstance().GetUILayer("Button")->AddComponent(optionButton);
	UIManager::GetInstance().GetUILayer("Button")->AddComponent(exitButton);

	//===========Set Button Event
	startButton->SetOnClick([this]()
		{
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

	// Button Update
	UIManager::GetInstance().Update(tick);

	// todo: Get another floating boat in sky box as background

}

void SceneTitle::Draw()
{
	// Draw Title
	GetObj<UIRender>("TitleLogo")->Draw();
	// Draw Buttons
	UIManager::GetInstance().Draw();
}
