#pragma once
#include <memory>
#include "D3DApp.h"
#include "SignalBus.h"
#include "UIFontSet.h"


class GameApp : public D3DApp
{
private:
    std::shared_ptr<GameSignalBus> m_pGameSignalBus;    //ゲーム用

private:

    bool InitResource();
    void ApplyResolutionPreset(Event::ResolutionPreset _preset);
public:

	GameApp(HINSTANCE hInstance, const std::wstring& windowName);
    ~GameApp() override;

    bool Init() override;
    void OnResize()override;
    void UpdateScene(float dt)override;
    void DrawScene()override;




};

