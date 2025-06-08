#pragma once
#include <wrl/client.h>
#include <string>
#include "WinMin.h"
#include <d3d11_1.h>
#include <d2d1.h>
#include <DirectXMath.h>
#include <dwrite.h>
#include "CpuTimer.h"
#include "Assets/Lib/imgui/Include/imgui.h"
#include "Assets/Lib/imgui/Include/imgui_impl_dx11.h"
#include "Assets/Lib/imgui/Include/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "imgui.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "DirectXTex.lib")

#define WIN_WIDTH  (1280)
#define WIN_HEIGHT (720)

class RenderTarget;
class DepthStencil;

class D3DApp
{
public:
    D3DApp(HINSTANCE hInstance, const std::wstring& windowName);
    virtual ~D3DApp();

    HINSTANCE AppInst()const;                  
    HWND      MainWnd()const;                   
    float     AspectRatio()const;               

    int Run();          //MainLoop                   

    virtual bool Init();                      
    virtual void OnResize();                  
    virtual void UpdateScene(float dt) = 0;    
    virtual void DrawScene() = 0;               
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    ID3D11Device* GetDevice() const { return m_pd3dDevice.Get(); };
    ID3D11DeviceContext* GetContext() const { return m_pd3dImmediateContext.Get(); };
    IDXGISwapChain* GetSwapChain() const { return m_pSwapChain.Get(); };




protected:
    /// <summary>
    /// Init Main Window
    /// </summary>
    /// <returns>成功かどうか？</returns>
    bool InitMainWindow();

    /// <summary>
    /// DirectX3D初期化
    /// </summary>
    /// <returns>成功かどうか？</returns> // Direct3D初始化
    bool InitDirect3D();

    /// @brief ImGui初期化
    bool InitImGui();

    bool InitDirectX2D();

    void CalculateFrameStats(); //Fps

    /// @brief Windowサイズ調整
    /// @param width 
    /// @param height 
    void SetWindowSize(int width, int height);

protected:

    HINSTANCE m_hAppInst;       
    HWND      m_hMainWnd;        
    bool      m_AppPaused;       
    bool      m_Minimized;      
    bool      m_Maximized;       
    bool      m_Resizing;        
    bool      m_Enable4xMsaa;    
    UINT      m_4xMsaaQuality;  

    CpuTimer m_Timer;            // Timer

    template <class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    // Direct2D
    ComPtr<ID2D1Factory> m_pd2dFactory;							// D2D工厂
    ComPtr<ID2D1RenderTarget> m_pd2dRenderTarget;				// D2D渲染目标
    ComPtr<IDWriteFactory> m_pdwriteFactory;					// DWrite工厂

    // Direct3D 11
    ComPtr<ID3D11Device> m_pd3dDevice;                    
    ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   
    ComPtr<IDXGISwapChain> m_pSwapChain;

    // Direct3D 11.1
    ComPtr<ID3D11Device1> m_pd3dDevice1;                  
    ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1; 
    ComPtr<IDXGISwapChain1> m_pSwapChain1;                

    ComPtr<ID3D11Texture2D>        m_pDepthStencilBuffer;                
    ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;                  
    ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;                  
    D3D11_VIEWPORT                 m_ScreenViewport;                                        


    std::wstring m_MainWndCaption;                     
    int m_ClientWidth;                                  
    int m_ClientHeight;

    FILE* fp = nullptr;



public:
    /// @brief サンプラーステート設定
	/// @param _state 
    static void SetSamplerState(ComPtr<ID3D11SamplerState> _state);

    /// @brief ブレンドステートを設定
    /// @param _state ブレンドステート名
    static void SetBlendState(ComPtr<ID3D11BlendState> _state);

    /// @brief Set Rasterizer State
    /// @param _rsState 
    static void SetCullingMode(ComPtr<ID3D11RasterizerState> _rsState);

};

extern D3DApp* gD3D;