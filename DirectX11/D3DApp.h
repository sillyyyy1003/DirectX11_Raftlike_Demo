#pragma once

#include <wrl/client.h>
#include <string>
#include "WinMin.h"
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "CpuTimer.h"
#include <imgui/Include/imgui.h>
#include <imgui/Include/imgui_impl_dx11.h>
#include <imgui/Include/imgui_impl_win32.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "imgui.lib")
#pragma comment(lib, "DirectXTex.lib")


class D3DApp
{
public:
    D3DApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
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

    /// <summary>
    /// ImGui初期化
    /// </summary>
    /// <returns></returns>
    bool InitImGui();

    void CalculateFrameStats(); //Fps

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

    // Direct3D 11
    ComPtr<ID3D11Device> m_pd3dDevice;                    
    ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   
    ComPtr<IDXGISwapChain> m_pSwapChain;                 
    // Direct3D 11.1
    ComPtr<ID3D11Device1> m_pd3dDevice1;                  
    ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1; 
    ComPtr<IDXGISwapChain1> m_pSwapChain1;                
    // 常用资源
    ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        
    ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;  
    ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;  
    D3D11_VIEWPORT m_ScreenViewport;                     


    std::wstring m_MainWndCaption;                     
    int m_ClientWidth;                                  
    int m_ClientHeight;                                  
};

extern D3DApp* gD3D;