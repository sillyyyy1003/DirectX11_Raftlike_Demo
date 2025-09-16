#pragma once
#include "WinMin.h"
#include <wrl/client.h>
#include <d3d11_1.h>

class RenderStates
{
public:
    template <class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    static bool IsInit();

    static void InitAll(ID3D11Device* device);

public:
    static ComPtr<ID3D11RasterizerState> RSWireframe;		      
    static ComPtr<ID3D11RasterizerState> RSNoCull;			          
    static ComPtr<ID3D11RasterizerState> RSCullClockWise;
    static ComPtr<ID3D11RasterizerState> RSShadow;				

    static ComPtr<ID3D11SamplerState> SSLinearWrap;			           
    static ComPtr<ID3D11SamplerState> SSAnisotropicWrap;		    

    static ComPtr<ID3D11BlendState> BSNoColorWrite;		               
    static ComPtr<ID3D11BlendState> BSTransparent;		               
    static ComPtr<ID3D11BlendState> BSAlphaToCoverage;	                
    static ComPtr<ID3D11BlendState> BSAdditive;			                


    static ComPtr<ID3D11DepthStencilState> DSSEqual;					// 深度/模板状态：仅允许绘制深度值相等的像素
    static ComPtr<ID3D11DepthStencilState> DSSLessEqual;                // 深度/模板状态：用于传统方式天空盒绘制
    static ComPtr<ID3D11DepthStencilState> DSSGreaterEqual;             // 深度/模板状态：用于反向Z绘制
    static ComPtr<ID3D11DepthStencilState> DSSNoDepthWrite;             // 深度/模板状态：仅测试，但不写入深度值
    static ComPtr<ID3D11DepthStencilState> DSSNoDepthTest;              // 深度/模板状态：关闭深度测试
    static ComPtr<ID3D11DepthStencilState> DSSWriteStencil;		        // 深度/模板状态：无深度测试，写入模板值
    static ComPtr<ID3D11DepthStencilState> DSSEqualStencil;	            // 深度/模板状态：反向Z，检测模板值
};



