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


    static ComPtr<ID3D11DepthStencilState> DSSWriteStencil;		      
    static ComPtr<ID3D11DepthStencilState> DSSDrawWithStencil;	        
    static ComPtr<ID3D11DepthStencilState> DSSNoDoubleBlend;	      
    static ComPtr<ID3D11DepthStencilState> DSSNoDepthTest;		        
    static ComPtr<ID3D11DepthStencilState> DSSNoDepthWrite;		        
    static ComPtr<ID3D11DepthStencilState> DSSNoDepthTestWithStencil;	
    static ComPtr<ID3D11DepthStencilState> DSSNoDepthWriteWithStencil;	
};



