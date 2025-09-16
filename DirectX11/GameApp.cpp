#include "GameApp.h"

#include "AudioManager.h"
#include "Cube.h"
#include "d3dUtil.h"
#include "DebugLog.h"
#include "DirLight.h"
#include "DXTrace.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "RenderState.h"
#include "SceneManager.h"
#include "Sprite.h"

using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName)
    : D3DApp(hInstance, windowName)
{
}

GameApp::~GameApp()
{
    // UnInit game contents
    SceneManager::Get()->UnInit();

    //=====Audioの初期化
    AudioManager::Instance().UnInit(); // オーディオマネージャーの終了処理

	// UnInit Sprite
    Sprite::Uninit();
}

bool GameApp::Init()
{

#if defined(_DEBUG) || defined(DEBUG)
    // 標準出力の割り当て
    AllocConsole();
    freopen_s(&fp, "CON", "w", stdout);
    DebugLog::Log("Game Init Completed");
#endif

    if (!D3DApp::Init())
        return false;

    if (!InitResource())
        return false;

    return true;
}

void GameApp::OnResize()
{
    assert(m_pd2dFactory);
    assert(m_pDWriteFactory);

    // 释放D2D的相关资源
    m_pd2dRenderTarget.Reset();

    D3DApp::OnResize();

    ComPtr<IDXGISurface> surface;
    HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
    surface.Reset();

    if (hr == S_OK)
    {

    }
    else
    {
        assert(m_pd2dRenderTarget);
    }
    m_pd2dRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}


void GameApp::UpdateScene(float dt)
{
    // GameUpdate
    SceneManager::Get()->_update(dt);
}

void GameApp::DrawScene()
{
    assert(m_pd3dImmediateContext);
    assert(m_pSwapChain);

    static float color[4] = { 0.4f, 0.4f, 0.8f, 1.0f };	// RGBA = (0,0,0,255)

    m_pDefRenderTarget->Clear(color);
    m_pDefDepthStencil->Clear();

    //GameRender=================================================
    SceneManager::Get()->_draw();

    //GameRender=================================================

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    HR(m_pSwapChain->Present(0, 0));
}

POINT GameApp::GetCursorPosition()
{
    POINT cursor;
    GetCursorPos(&cursor);
	ScreenToClient(gD3D->MainWnd(), &cursor);
    return cursor;
}


bool GameApp::InitResource()
{
    // ======RenderState
    RenderStates::InitAll(m_pd3dDevice.Get());

    // ======Game内容初期化
	SceneManager::Get()->InitD2DResource(); // D2Dの初期化

    
    SceneManager::Get()->Init();    // Material/Effect/GameObjectの初期化

    // ======Init Sprite
    Sprite::Init();

    // ======events subscription
    m_pGameSignalBus = std::make_shared<GameSignalBus>();
    // ウィンドウ調整
    m_pGameSignalBus->OnResolutionChangeRequest.Connect(
        [this](Event::ResolutionPreset preset)
        {
            this->ApplyResolutionPreset(preset);
        });


    SceneManager::Get()->SetSignalBus(m_pGameSignalBus.get());
    return true;

}

void GameApp::ApplyResolutionPreset(Event::ResolutionPreset _preset)
{
    switch(_preset)
    {
	    default:
    case Event::ResolutionPreset::R_1080p:
        SetWindowSize(Event::R_1080.x,Event::R_1080.y);
    	break;
    case Event::ResolutionPreset::R_720p:
        SetWindowSize(Event::R_720.x, Event::R_720.y);
        break;
    case Event::ResolutionPreset::R_540p:
        SetWindowSize(Event::R_540.x, Event::R_540.y);
        break;
    }
}
