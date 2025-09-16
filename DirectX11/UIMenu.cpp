#include "UIMenu.h"
#include "AudioManager.h"
#include "SceneBase.hpp"
#include "UIManager.h"

namespace UIMenuConst
{
	static constexpr float BarScaler = 0.8f;

	// Z 坐标
	static constexpr float TextZ = 1.0f;
	static constexpr float BgZ = 0.5f;
	static constexpr float ButtonZ = 1.0f;

	// BGM
	static constexpr DirectX::XMFLOAT3 BgmTextPos = { -280, 140, TextZ };
	static constexpr DirectX::XMFLOAT3 BgmTextScale = { 120, 65, 1 };
	static constexpr DirectX::XMFLOAT3 BgmBarPos = { -120, 140, TextZ };
	static constexpr DirectX::XMFLOAT2 BgmBarSize = { 462 * BarScaler, 28 * BarScaler };
	static constexpr DirectX::XMFLOAT2 BgmBarThumb = { 24 * BarScaler, 24 * BarScaler };
	static constexpr DirectX::XMFLOAT3 BgmLeftArrowPos = { -180, 140, ButtonZ };
	static constexpr DirectX::XMFLOAT3 BgmRightArrowPos = { 310, 140, ButtonZ };
	static constexpr float ArrowWidth = 54.5f;
	static constexpr float ArrowHeight = 63.5f;

	// SE
	static constexpr DirectX::XMFLOAT3 SeTextPos = { -280, 65, TextZ };
	static constexpr DirectX::XMFLOAT3 SeTextScale = { 120, 65, 1 };
	static constexpr DirectX::XMFLOAT3 SeBarPos = { -120, 65, TextZ };
	static constexpr DirectX::XMFLOAT3 SeLeftArrowPos = { -180, 65, ButtonZ };
	static constexpr DirectX::XMFLOAT3 SeRightArrowPos = { 310, 65, ButtonZ };

	// Environment
	static constexpr DirectX::XMFLOAT3 EnvironTextPos = { -280, 0, TextZ };
	static constexpr DirectX::XMFLOAT3 EnvironTextScale = { 120, 65, 1 };
	static constexpr DirectX::XMFLOAT3 EnvironBarPos = { -120, -10, TextZ };
	static constexpr DirectX::XMFLOAT3 EnvironLeftArrowPos = { -180, -10, ButtonZ };
	static constexpr DirectX::XMFLOAT3 EnvironRightArrowPos = { 310, -10, ButtonZ };

	// Buttons
	static constexpr DirectX::XMFLOAT3 ResumeButtonPos = { 0, -135, ButtonZ };
	static constexpr DirectX::XMFLOAT3 BackToTitleButtonPos = { 0, -260, ButtonZ };
	static constexpr float ButtonWidth = 240;
	static constexpr float ButtonHeight = 87.2f;

	// Text Colors
	static constexpr D2D1_COLOR_F TextColor = { 0.7176f, 0.3882f, 0.0f, 1.0f };
}

UIMenu::UIMenu()
{
}

void UIMenu::Init(MenuResource& resource)
{
    using namespace UIMenuConst;

    //==============Menu Background
    m_menuBg = std::make_unique<UIRender>();
    m_menuBg->Init(resource.menuMat, resource.effect, resource.square);
    m_menuBg->GetTransform().SetPosition({ 0,0,BgZ });
    m_menuBg->GetTransform().SetScale({ 800,720,1 });

    //==============BGM
    m_bgmText = std::make_unique<UIText>();
    m_bgmText->Init(resource.fontSet, "UIMenuFont", resource.brush);
    m_bgmText->SetPosition(BgmTextPos);
    m_bgmText->SetScale(BgmTextScale);
    m_bgmText->SetStaticText("BGM");
    m_bgmText->SetTextColor(TextColor);

    m_bgmBar = std::make_unique<UIBar>();
    m_bgmBar->Init(resource.barBgMat, resource.barMat, resource.effect, resource.effect);
    m_bgmBar->SetScale(BgmBarSize, BgmBarThumb);
    m_bgmBar->SetPosition(BgmBarPos, { -2 * BarScaler, -4 * BarScaler });

    m_bgmLeftArrow = std::make_unique<UIButtonMove>();
    m_bgmLeftArrow->Init(resource.effect, resource.bgmLeftArrowMat, resource.square);
    m_bgmLeftArrow->SetButton(BgmLeftArrowPos, ArrowWidth, ArrowHeight);

    m_bgmRightArrow = std::make_unique<UIButtonMove>();
    m_bgmRightArrow->Init(resource.effect, resource.bgmRightArrowMat, resource.square);
    m_bgmRightArrow->InitMoveParam(0.005f, 5.f);
    m_bgmRightArrow->SetButton(BgmRightArrowPos, ArrowWidth, ArrowHeight);


    //==============SE
    m_seText = std::make_unique<UIText>();
    m_seText->Init(resource.fontSet, "UIMenuFont", resource.brush);
    m_seText->SetPosition(SeTextPos);
    m_seText->SetScale(SeTextScale);
    m_seText->SetStaticText("SE");
    m_seText->SetTextColor(TextColor);

    m_seBar = std::make_unique<UIBar>();
    m_seBar->Init(resource.barBgMat, resource.barMat, resource.effect, resource.effect);
    m_seBar->SetScale(BgmBarSize, BgmBarThumb);
    m_seBar->SetPosition(SeBarPos, { -2 * BarScaler, -4 * BarScaler });

    m_seLeftArrow = std::make_unique<UIButtonMove>();
    m_seLeftArrow->Init(resource.effect, resource.seLeftArrowMat, resource.square);
    m_seLeftArrow->InitMoveParam(0.005f, 5.f);
    m_seLeftArrow->SetButton(SeLeftArrowPos, ArrowWidth, ArrowHeight);


    m_seRightArrow = std::make_unique<UIButtonMove>();
    m_seRightArrow->Init(resource.effect, resource.seRightArrowMat, resource.square);
    m_seRightArrow->InitMoveParam(0.005f, 5.f);
    m_seRightArrow->SetButton(SeRightArrowPos, ArrowWidth, ArrowHeight);
   

    //==============Environment
    m_environmentText = std::make_unique<UIText>();
    m_environmentText->Init(resource.fontSet, "UIMenuFont", resource.brush);
    m_environmentText->SetPosition(EnvironTextPos);
    m_environmentText->SetScale(EnvironTextScale);
    m_environmentText->SetStaticText("Envir");
    m_environmentText->SetTextColor(TextColor);

    m_environmentBar = std::make_unique<UIBar>();
    m_environmentBar->Init(resource.barBgMat, resource.barMat, resource.effect, resource.effect);
    m_environmentBar->SetScale(BgmBarSize, BgmBarThumb);
    m_environmentBar->SetPosition(EnvironBarPos, { -2 * BarScaler, -4 * BarScaler });

    m_environmentLeftArrow = std::make_unique<UIButtonMove>();
    m_environmentLeftArrow->Init(resource.effect, resource.environmentLeftArrowMat, resource.square);
    m_environmentLeftArrow->SetButton(EnvironLeftArrowPos, ArrowWidth, ArrowHeight);

    m_environmentRightArrow = std::make_unique<UIButtonMove>();
    m_environmentRightArrow->Init(resource.effect, resource.environmentRightArrowMat, resource.square);
    m_environmentRightArrow->SetButton(EnvironRightArrowPos, ArrowWidth, ArrowHeight);

    //==============Buttons
    m_resumeButton = std::make_unique<UIButtonMove>();
    m_resumeButton->Init(resource.effect, resource.resumeMat, resource.square);
    m_resumeButton->SetButton(ResumeButtonPos, ButtonWidth, ButtonHeight);

    m_backToTitleButton = std::make_unique<UIButtonMove>();
    m_backToTitleButton->Init(resource.effect, resource.backToTitleMat, resource.square);
    m_backToTitleButton->SetButton(BackToTitleButtonPos, ButtonWidth, ButtonHeight);
 
    //==============AudioManager link
    AudioManager::Instance().SetUI(m_bgmBar.get(), m_seBar.get(), m_environmentBar.get());

    //==============Add To Layer
    UIManager::Instance().AddUiLayer("Menu", 30);
	UIManager::Instance().GetUILayer("Menu")->AddComponent(m_menuBg.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_bgmText.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_seText.get());
	UIManager::Instance().GetUILayer("Menu")->AddComponent(m_environmentText.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_bgmBar.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_seBar.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_environmentBar.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_bgmLeftArrow.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_bgmRightArrow.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_seLeftArrow.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_seRightArrow.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_environmentLeftArrow.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_environmentRightArrow.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_backToTitleButton.get());
    UIManager::Instance().GetUILayer("Menu")->AddComponent(m_resumeButton.get());

    SetActive(false);
}

void UIMenu::SetButton(SceneBase* scene)
{
    m_bgmLeftArrow->SetOnClick([]()
    {
	    AudioManager::Instance().AddVolume(AudioManager::AudioType::BGM, -0.1f);
        AudioManager::Instance().Play("SE_Button", false);
    });
    m_bgmLeftArrow->SetOnHover([this]() { m_bgmLeftArrow->ActiveMove(); });
    m_bgmLeftArrow->SetOnExit([this]() { m_bgmLeftArrow->DeActiveMove(); });

    m_bgmRightArrow->SetOnClick([]()
    {
	    AudioManager::Instance().AddVolume(AudioManager::AudioType::BGM, 0.1f);
        AudioManager::Instance().Play("SE_Button", false);
    });
    m_bgmRightArrow->SetOnHover([this]() { m_bgmRightArrow->ActiveMove(); });
    m_bgmRightArrow->SetOnExit([this]() { m_bgmRightArrow->DeActiveMove(); });


    m_seLeftArrow->SetOnClick([]() {
   
    AudioManager::Instance().AddVolume(AudioManager::AudioType::SE, -0.1f);
    AudioManager::Instance().Play("SE_Button", false);

    });

    m_seLeftArrow->SetOnHover([this]()
        {
            m_seLeftArrow->ActiveMove();
        });
    m_seLeftArrow->SetOnExit([this]() { m_seLeftArrow->DeActiveMove(); });

    m_seRightArrow->SetOnClick([]()
    {
	    AudioManager::Instance().AddVolume(AudioManager::AudioType::SE, 0.1f); AudioManager::Instance().Play("SE_Button", false);
    });
    m_seRightArrow->SetOnHover([this]()
    {
	    m_seRightArrow->ActiveMove();
    });
    m_seRightArrow->SetOnExit([this]() { m_seRightArrow->DeActiveMove(); });


    m_environmentLeftArrow->SetOnClick([]()
    {
	    AudioManager::Instance().AddVolume(AudioManager::AudioType::ENVIRONMENT, -0.1f);
        AudioManager::Instance().Play("SE_Button", false);

    });
    m_environmentLeftArrow->SetOnHover([this]()
        {
            m_environmentLeftArrow->ActiveMove();
        });
    m_environmentLeftArrow->SetOnExit([this]() { m_environmentLeftArrow->DeActiveMove(); });

    m_environmentRightArrow->SetOnClick([]()
    {
	    AudioManager::Instance().AddVolume(AudioManager::AudioType::ENVIRONMENT, 0.1f);
        AudioManager::Instance().Play("SE_Button", false);

    });
    m_environmentRightArrow->SetOnHover([this]() { m_environmentRightArrow->ActiveMove(); });
    m_environmentRightArrow->SetOnExit([this]() { m_environmentRightArrow->DeActiveMove(); });


    m_resumeButton->SetOnClick([this]()
    {
            AudioManager::Instance().Play("SE_Button", false);
            this->SetActive(false);
    });
    m_resumeButton->SetOnHover([this]()
        {
            m_resumeButton->ActiveMove();
        });
    m_resumeButton->SetOnExit([this]()
        {
            m_resumeButton->DeActiveMove();
        });

    m_backToTitleButton->SetOnClick([scene,this]()
    {
    	AudioManager::Instance().Play("SE_Button", false);
	    scene->SetCurrentScene("Title");
        this->SetActive(false);
    });
    m_backToTitleButton->SetOnHover([this]() { m_backToTitleButton->ActiveMove(); });
    m_backToTitleButton->SetOnExit([this]() { m_backToTitleButton->DeActiveMove(); });

}

void UIMenu::Update(float dt)
{
}

void UIMenu::Draw()
{

}

void UIMenu::SetActive(bool isActive)
{
    m_menuBg->SetActive(isActive);

    m_bgmText->SetActive(isActive);
    m_seText->SetActive(isActive);
    m_environmentText->SetActive(isActive);

    m_bgmBar->SetActive(isActive);
    m_seBar->SetActive(isActive);
    m_environmentBar->SetActive(isActive);

    m_bgmLeftArrow->SetActive(isActive);
    m_bgmRightArrow->SetActive(isActive);
    m_seLeftArrow->SetActive(isActive);
    m_seRightArrow->SetActive(isActive);
    m_environmentLeftArrow->SetActive(isActive);
    m_environmentRightArrow->SetActive(isActive);
    m_backToTitleButton->SetActive(isActive);
    m_resumeButton->SetActive(isActive);

	UIManager::Instance().EnableLayers(!isActive);// Disable all layers
	UIManager::Instance().GetUILayer("Menu")->SetEnable(isActive); // Enable menu layer
}
