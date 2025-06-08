#include "GameApp.h"
#include "Cube.h"
#include "d3dUtil.h"
#include "DebugLog.h"
#include "DirLight.h"
#include "DXTrace.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "RenderState.h"
#include "SceneManager.h"

using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName)
    : D3DApp(hInstance, windowName)
{
}

GameApp::~GameApp()
{
    //UnInit game contents
    SceneManager::Get()->UnInit();
   
}

bool GameApp::Init()
{

#ifdef _DEBUG
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
    assert(m_pdwriteFactory);

	// 释放D2D的相关资源
    //m_pColorBrush.Reset();
    m_pd2dRenderTarget.Reset();

    D3DApp::OnResize();

    // 为D2D创建DXGI表面渲染目标
    //ComPtr<IDXGISurface> surface;
    //HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf()));
    //D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
    //D2D1_RENDER_TARGET_TYPE_DEFAULT,
    //D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
    //hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
    //surface.Reset();

    //if (hr == S_OK)
    //{
    //    // 创建固定颜色刷和文本格式
    //    //HR(m_pd2dRenderTarget->CreateSolidColorBrush(
    //       /* D2D1::ColorF(D2D1::ColorF::White),
    //        m_pColorBrush.GetAddressOf()));*/
    //    //HR(m_pdwriteFactory->CreateTextFormat(L"宋体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
    //      /*   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,20, L"zh-cn",m_pTextFormat.GetAddressOf()));*/
    //}
    //else
    //{
    //    assert(m_pd2dRenderTarget);
    //}
}

void GameApp::UpdateScene(float dt)
{
    //GameUpdate
    SceneManager::Get()->_update(dt);

}

void GameApp::DrawScene()
{
    assert(m_pd3dImmediateContext);
    assert(m_pSwapChain);

    static float color[4] = { 0.4f, 0.4f, 0.8f, 1.0f };	// RGBA = (0,0,0,255)
    m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&color));
    m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //GameRender=================================================

    SceneManager::Get()->_draw();
    //GameRender=================================================

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    HR(m_pSwapChain->Present(0, 0));
}



bool GameApp::InitResource()
{
    // ======RenderState
    RenderStates::InitAll(m_pd3dDevice.Get());

    // ======Game内容初期化
    SceneManager::Get()->Init();

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
        SetWindowSize(1920, 1080);
    	break;
    case Event::ResolutionPreset::R_720p:
        SetWindowSize(1280 , 720);
        break;
    case Event::ResolutionPreset::R_540p:
        SetWindowSize(960, 540);
        break;
    }
}
