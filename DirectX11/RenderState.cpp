#include "RenderState.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace Microsoft::WRL;

ComPtr<ID3D11RasterizerState> RenderStates::RSNoCull = nullptr;
ComPtr<ID3D11RasterizerState> RenderStates::RSWireframe = nullptr;
ComPtr<ID3D11RasterizerState> RenderStates::RSCullClockWise = nullptr;
ComPtr<ID3D11RasterizerState> RenderStates::RSShadow = nullptr;


ComPtr<ID3D11SamplerState> RenderStates::SSAnisotropicWrap = nullptr;
ComPtr<ID3D11SamplerState> RenderStates::SSLinearWrap = nullptr;

ComPtr<ID3D11BlendState> RenderStates::BSAlphaToCoverage = nullptr;
ComPtr<ID3D11BlendState> RenderStates::BSNoColorWrite = nullptr;
ComPtr<ID3D11BlendState> RenderStates::BSTransparent = nullptr;
ComPtr<ID3D11BlendState> RenderStates::BSAdditive = nullptr;

ComPtr<ID3D11DepthStencilState> RenderStates::DSSWriteStencil = nullptr;
ComPtr<ID3D11DepthStencilState> RenderStates::DSSDrawWithStencil = nullptr;
ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDoubleBlend = nullptr;
ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthTest = nullptr;
ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthWrite = nullptr;
ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthTestWithStencil = nullptr;
ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthWriteWithStencil = nullptr;

bool RenderStates::IsInit()
{
    return RSWireframe != nullptr;
}

void RenderStates::InitAll(ID3D11Device* device)
{
    
    if (IsInit())
        return;

    CD3D11_RASTERIZER_DESC rasterizerDesc(CD3D11_DEFAULT{});

    // 线框模式
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    HR(device->CreateRasterizerState(&rasterizerDesc, RSWireframe.GetAddressOf()));

    // 无背面剔除模式
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    HR(device->CreateRasterizerState(&rasterizerDesc, RSNoCull.GetAddressOf()));

    // 顺时针剔除模式
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = true;
    HR(device->CreateRasterizerState(&rasterizerDesc, RSCullClockWise.GetAddressOf()));

    // 深度偏移模式
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 1.0f;
    HR(device->CreateRasterizerState(&rasterizerDesc, RSShadow.GetAddressOf()));


    // ******************
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));


    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR(device->CreateSamplerState(&sampDesc, SSLinearWrap.GetAddressOf()));

    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MaxAnisotropy = 4;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR(device->CreateSamplerState(&sampDesc, SSAnisotropicWrap.GetAddressOf()));


    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    auto& rtDesc = blendDesc.RenderTarget[0];
    // Alpha-To-Coverage
    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.IndependentBlendEnable = false;
    rtDesc.BlendEnable = false;
    rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    HR(device->CreateBlendState(&blendDesc, BSAlphaToCoverage.GetAddressOf()));

    // Alpha blend
    // Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
    // Alpha = SrcAlpha
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    rtDesc.BlendEnable = true;
    rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

    HR(device->CreateBlendState(&blendDesc, BSTransparent.GetAddressOf()));

    // Add
    // Color = SrcColor + DestColor
    // Alpha = SrcAlpha
    rtDesc.SrcBlend = D3D11_BLEND_ONE;
    rtDesc.DestBlend = D3D11_BLEND_ONE;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

    HR(device->CreateBlendState(&blendDesc, BSAdditive.GetAddressOf()));

    //
    // Color = DestColor
    // Alpha = DestAlpha
    rtDesc.BlendEnable = false;
    rtDesc.SrcBlend = D3D11_BLEND_ZERO;
    rtDesc.DestBlend = D3D11_BLEND_ONE;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask = 0;
    HR(device->CreateBlendState(&blendDesc, BSNoColorWrite.GetAddressOf()));

 
    D3D11_DEPTH_STENCIL_DESC dsDesc;

    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    HR(device->CreateDepthStencilState(&dsDesc, DSSWriteStencil.GetAddressOf()));


    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    HR(device->CreateDepthStencilState(&dsDesc, DSSDrawWithStencil.GetAddressOf()));


    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    HR(device->CreateDepthStencilState(&dsDesc, DSSNoDoubleBlend.GetAddressOf()));


    dsDesc.DepthEnable = false;
    dsDesc.StencilEnable = false;

    HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthTest.GetAddressOf()));



    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthTestWithStencil.GetAddressOf()));

    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false;

    HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthWrite.GetAddressOf()));

    /*
     *?行深度??，但不写入深度?的状?
     *若?制非透明物体?，?使用默?状?
     *?制透明物体?，使用?状?可以有效?保混合状?的?行
	 *并且?保?前的非透明物体可以阻??后的一切物体
     *??足模板?条件的区域才?行?制
     */
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
  
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthWriteWithStencil.GetAddressOf()));

  
    D3D11SetDebugObjectName(RSCullClockWise.Get(), "RSCullClockWise");
    D3D11SetDebugObjectName(RSNoCull.Get(), "RSNoCull");
    D3D11SetDebugObjectName(RSWireframe.Get(), "RSWireframe");

    D3D11SetDebugObjectName(SSAnisotropicWrap.Get(), "SSAnisotropicWrap");
    D3D11SetDebugObjectName(SSLinearWrap.Get(), "SSLinearWrap");

    D3D11SetDebugObjectName(BSAlphaToCoverage.Get(), "BSAlphaToCoverage");
    D3D11SetDebugObjectName(BSNoColorWrite.Get(), "BSNoColorWrite");
    D3D11SetDebugObjectName(BSTransparent.Get(), "BSTransparent");
    D3D11SetDebugObjectName(BSAdditive.Get(), "BSAdditive");

    D3D11SetDebugObjectName(DSSWriteStencil.Get(), "DSSWriteStencil");
    D3D11SetDebugObjectName(DSSDrawWithStencil.Get(), "DSSDrawWithStencil");
    D3D11SetDebugObjectName(DSSNoDoubleBlend.Get(), "DSSNoDoubleBlend");
    D3D11SetDebugObjectName(DSSNoDepthTest.Get(), "DSSNoDepthTest");
    D3D11SetDebugObjectName(DSSNoDepthWrite.Get(), "DSSNoDepthWrite");
    D3D11SetDebugObjectName(DSSNoDepthTestWithStencil.Get(), "DSSNoDepthTestWithStencil");
    D3D11SetDebugObjectName(DSSNoDepthWriteWithStencil.Get(), "DSSNoDepthWriteWithStencil");
}