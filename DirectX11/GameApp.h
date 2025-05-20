#pragma once
#include <memory>

#include "CameraBase.h"
#include "Cube.h"
#include "D3DApp.h"
#include "Shader.h"

class GameApp : public D3DApp
{

private:
    ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局
    ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
    ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
    ComPtr<ID3D11PixelShader> m_pPixelShader;	// 像素着色器

public:
    struct VertexPosColor
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
        static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
    };

private:
    bool InitEffect();
    bool InitResource();
    std::unique_ptr<Cube> cube;

    std::unique_ptr<PixelShader> pixelShader;
    std::unique_ptr<VertexShader> vertexShader;

    std::unique_ptr<CameraBase> camera;

public:
    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    ~GameApp();

    bool Init() override;
    void OnResize()override;
    void UpdateScene(float dt)override;
    void DrawScene()override;






};

