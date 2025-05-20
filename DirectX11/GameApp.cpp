#include "GameApp.h"

#include "Cube.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "FirstPersonCamera.h"
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosColor::inputLayout[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
    : D3DApp(hInstance, windowName, initWidth, initHeight)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
    if (!D3DApp::Init())
        return false;

    if (!InitEffect())
        return false;

    if (!InitResource())
        return false;

    return true;
}

void GameApp::OnResize()
{
    D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
    //GameUpdate
	//Game Render
    static float  phi = 0.0f, theta = 0.0f;
    phi += 0.3f * dt, theta += 0.37f * dt;
   

}

void GameApp::DrawScene()
{
    assert(m_pd3dImmediateContext);
    assert(m_pSwapChain);

    static float black[4] = { 0.1f, 0.1f, 0.2f, 1.0f };	// RGBA = (0,0,0,255)
    m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black);
    m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

   
    //GameRender
    
    //WorldMatrix
    DirectX::XMFLOAT4X4 mat[3];
    static float  phi = 0.0f, theta = 0.0f;
    phi += 0.3f * 0.002f, theta += 0.37f * 0.002f;
    XMMATRIX W = XMMatrixRotationX(phi) * XMMatrixRotationY(theta);
    XMStoreFloat4x4(&mat[0], W);
    

    //ViewMatrix
    mat[1] = camera->GetViewXMF();
    mat[2] = camera->GetProjXMF();

	vertexShader->WriteBuffer(0, mat);

    vertexShader->Bind();
    pixelShader->Bind();

    cube->Draw();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitEffect()
{
    //Create Shader
    vertexShader = std::make_unique<VertexShader>();
	pixelShader = std::make_unique<PixelShader>();

    vertexShader->Load("Assets/Shader/VS_Geometry.cso");
    pixelShader->Load("Assets/Shader/PS_Geometry.cso");

    return true;
}

bool GameApp::InitResource()
{
    cube = std::make_unique<Cube>();
    cube->Init("null");

    camera = std::make_unique<FirstPersonCamera>();
    camera->SetPos({ 0,0,-10 });
    return true;
}