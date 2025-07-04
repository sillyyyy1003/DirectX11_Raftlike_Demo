//-------------------------------------------------------------------------------------
// DirectXTexUtil.cpp
//
// DirectX Texture Library - Utilities
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
//-------------------------------------------------------------------------------------

#include "DirectXTexP.h"

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
static_assert(XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT == DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT, "Xbox mismatch detected");
static_assert(XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT == DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT, "Xbox mismatch detected");
static_assert(XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT == DXGI_FORMAT_D16_UNORM_S8_UINT, "Xbox mismatch detected");
static_assert(XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS == DXGI_FORMAT_R16_UNORM_X8_TYPELESS, "Xbox mismatch detected");
static_assert(XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT == DXGI_FORMAT_X16_TYPELESS_G8_UINT, "Xbox mismatch detected");
static_assert(XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM == DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, "Xbox mismatch detected");
static_assert(XBOX_DXGI_FORMAT_R4G4_UNORM == DXGI_FORMAT_R4G4_UNORM, "Xbox mismatch detected");
#endif

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
static_assert(WIN10_DXGI_FORMAT_P208 == DXGI_FORMAT_P208, "Windows SDK mismatch detected");
static_assert(WIN10_DXGI_FORMAT_V208 == DXGI_FORMAT_V208, "Windows SDK mismatch detected");
static_assert(WIN10_DXGI_FORMAT_V408 == DXGI_FORMAT_V408, "Windows SDK mismatch detected");
#endif

#if defined(NTDDI_WIN11_GE) && !defined(_GAMING_XBOX)
static_assert(WIN11_DXGI_FORMAT_A4B4G4R4_UNORM == DXGI_FORMAT_A4B4G4R4_UNORM, "Windows SDK mismatch detected");
#endif

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace
{
#ifdef _WIN32
    //-------------------------------------------------------------------------------------
    // WIC Pixel Format Translation Data
    //-------------------------------------------------------------------------------------
    struct WICTranslate
    {
        const GUID& wic;
        DXGI_FORMAT format;
        bool        srgb;

        constexpr WICTranslate(const GUID& wg, DXGI_FORMAT fmt, bool isrgb) noexcept :
            wic(wg),
            format(fmt),
            srgb(isrgb) {}
    };

    constexpr WICTranslate g_WICFormats[] =
    {
        { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT,         false },

        { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT,         false },
        { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM,         true },

        { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM,             true },
        { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM,             true }, // DXGI 1.1
        { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM,             true }, // DXGI 1.1

        { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, true }, // DXGI 1.1
        { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM,          true },

        { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM,             true },
        { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM,               true },

        { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT,                  false },
        { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT,                  false },
        { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM,                  true },
        { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM,                   true },

        { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM,                   false },

        { GUID_WICPixelFormatBlackWhite,            DXGI_FORMAT_R1_UNORM,                   false },
    };

    bool g_WIC2 = false;
    IWICImagingFactory* g_Factory = nullptr;

    BOOL WINAPI InitializeWICFactory(PINIT_ONCE, PVOID, PVOID *ifactory) noexcept
    {
    #if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory2),
            ifactory
        );

        if (SUCCEEDED(hr))
        {
            // WIC2 is available on Windows 10, Windows 8.x, and Windows 7 SP1 with KB 2670838 installed
            g_WIC2 = true;
            return TRUE;
        }
        else
        {
            g_WIC2 = false;

            hr = CoCreateInstance(
                CLSID_WICImagingFactory1,
                nullptr,
                CLSCTX_INPROC_SERVER,
                __uuidof(IWICImagingFactory),
                ifactory
            );
            return SUCCEEDED(hr) ? TRUE : FALSE;
        }
    #else
        g_WIC2 = false;

        return SUCCEEDED(CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory),
            ifactory)) ? TRUE : FALSE;
    #endif
    }

#else // !WIN32
    inline void * _aligned_malloc(size_t size, size_t alignment)
    {
        size = (size + alignment - 1) & ~(alignment - 1);
        return std::aligned_alloc(alignment, size);
    }

#define _aligned_free free
#endif
}


#ifdef _WIN32
//=====================================================================================
// WIC Utilities
//=====================================================================================

_Use_decl_annotations_
DXGI_FORMAT DirectX::Internal::WICToDXGI(const GUID& guid) noexcept
{
    for (size_t i = 0; i < std::size(g_WICFormats); ++i)
    {
        if (memcmp(&g_WICFormats[i].wic, &guid, sizeof(GUID)) == 0)
            return g_WICFormats[i].format;
    }

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
    if (g_WIC2)
    {
        if (memcmp(&GUID_WICPixelFormat96bppRGBFloat, &guid, sizeof(GUID)) == 0)
            return DXGI_FORMAT_R32G32B32_FLOAT;
    }
#endif

    return DXGI_FORMAT_UNKNOWN;
}

_Use_decl_annotations_
bool DirectX::Internal::DXGIToWIC(DXGI_FORMAT format, GUID& guid, bool ignoreRGBvsBGR) noexcept
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        if (ignoreRGBvsBGR)
        {
            // If we are not doing conversion so don't really care about BGR vs RGB color-order,
            // we can use the canonical WIC 32bppBGRA format which avoids an extra format conversion when using the WIC scaler
            memcpy(&guid, &GUID_WICPixelFormat32bppBGRA, sizeof(GUID));
        }
        else
        {
            memcpy(&guid, &GUID_WICPixelFormat32bppRGBA, sizeof(GUID));
        }
        return true;

    case DXGI_FORMAT_D32_FLOAT:
        memcpy(&guid, &GUID_WICPixelFormat32bppGrayFloat, sizeof(GUID));
        return true;

    case DXGI_FORMAT_D16_UNORM:
        memcpy(&guid, &GUID_WICPixelFormat16bppGray, sizeof(GUID));
        return true;

    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        memcpy(&guid, &GUID_WICPixelFormat32bppBGRA, sizeof(GUID));
        return true;

    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        memcpy(&guid, &GUID_WICPixelFormat32bppBGR, sizeof(GUID));
        return true;

    #if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
    case DXGI_FORMAT_R32G32B32_FLOAT:
        if (g_WIC2)
        {
            memcpy(&guid, &GUID_WICPixelFormat96bppRGBFloat, sizeof(GUID));
            return true;
        }
        break;
    #endif

    default:
        for (size_t i = 0; i < std::size(g_WICFormats); ++i)
        {
            if (g_WICFormats[i].format == format)
            {
                memcpy(&guid, &g_WICFormats[i].wic, sizeof(GUID));
                return true;
            }
        }
        break;
    }

    memset(&guid, 0, sizeof(GUID));
    return false;
}

TEX_FILTER_FLAGS DirectX::Internal::CheckWICColorSpace(_In_ const GUID& sourceGUID, _In_ const GUID& targetGUID) noexcept
{
    TEX_FILTER_FLAGS srgb = TEX_FILTER_DEFAULT;

    for (size_t i = 0; i < std::size(g_WICFormats); ++i)
    {
        if (memcmp(&g_WICFormats[i].wic, &sourceGUID, sizeof(GUID)) == 0)
        {
            if (g_WICFormats[i].srgb)
                srgb |= TEX_FILTER_SRGB_IN;
        }

        if (memcmp(&g_WICFormats[i].wic, &targetGUID, sizeof(GUID)) == 0)
        {
            if (g_WICFormats[i].srgb)
                srgb |= TEX_FILTER_SRGB_OUT;
        }
    }

    if ((srgb & (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT)) == (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT))
    {
        srgb &= ~(TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT);
    }

    return srgb;
}


//-------------------------------------------------------------------------------------
// Public helper function to get common WIC codec GUIDs
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
REFGUID DirectX::GetWICCodec(WICCodecs codec) noexcept
{
    switch (codec)
    {
    case WIC_CODEC_BMP:
        return GUID_ContainerFormatBmp;

    case WIC_CODEC_JPEG:
        return GUID_ContainerFormatJpeg;

    case WIC_CODEC_PNG:
        return GUID_ContainerFormatPng;

    case WIC_CODEC_TIFF:
        return GUID_ContainerFormatTiff;

    case WIC_CODEC_GIF:
        return GUID_ContainerFormatGif;

    case WIC_CODEC_WMP:
        return GUID_ContainerFormatWmp;

    case WIC_CODEC_ICO:
        return GUID_ContainerFormatIco;

#ifdef NTDDI_WIN10_RS4
    case WIC_CODEC_HEIF:
        // This requires installing https://aka.ms/heif
        return GUID_ContainerFormatHeif;
#endif

    default:
        return GUID_NULL;
    }
}


//-------------------------------------------------------------------------------------
// Singleton function for WIC factory
//-------------------------------------------------------------------------------------
IWICImagingFactory* DirectX::GetWICFactory(bool& iswic2) noexcept
{
    if (g_Factory)
    {
        iswic2 = g_WIC2;
        return g_Factory;
    }

    static INIT_ONCE s_initOnce = INIT_ONCE_STATIC_INIT;

    if (!InitOnceExecuteOnce(&s_initOnce,
        InitializeWICFactory,
        nullptr,
        reinterpret_cast<LPVOID*>(&g_Factory)))
    {
        return nullptr;
    }

    iswic2 = g_WIC2;
    return g_Factory;
}


//-------------------------------------------------------------------------------------
// Optional initializer for WIC factory
//-------------------------------------------------------------------------------------
void DirectX::SetWICFactory(_In_opt_ IWICImagingFactory* pWIC) noexcept
{
    if (pWIC == g_Factory)
        return;

    bool iswic2 = false;
    if (pWIC)
    {
    #if(_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
        ComPtr<IWICImagingFactory2> wic2;
        HRESULT hr = pWIC->QueryInterface(IID_PPV_ARGS(wic2.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            iswic2 = true;
        }
    #endif
        pWIC->AddRef();
    }

    g_WIC2 = iswic2;
    std::swap(pWIC, g_Factory);
    if (pWIC)
        pWIC->Release();
}
#endif // WIN32


//=====================================================================================
// DXGI Format Utilities
//=====================================================================================

//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::IsPacked(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_YUY2: // 4:2:2 8-bit
    case DXGI_FORMAT_Y210: // 4:2:2 10-bit
    case DXGI_FORMAT_Y216: // 4:2:2 16-bit
        return true;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::IsVideo(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_YUY2:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
    case DXGI_FORMAT_NV11:
        // These video formats can be used with the 3D pipeline through special view mappings

    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
        // These are limited use video formats not usable in any way by the 3D pipeline

    case WIN10_DXGI_FORMAT_P208:
    case WIN10_DXGI_FORMAT_V208:
    case WIN10_DXGI_FORMAT_V408:
        // These video formats are for JPEG Hardware decode (DXGI 1.4)
        return true;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::IsPlanar(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_NV12:      // 4:2:0 8-bit
    case DXGI_FORMAT_P010:      // 4:2:0 10-bit
    case DXGI_FORMAT_P016:      // 4:2:0 16-bit
    case DXGI_FORMAT_420_OPAQUE:// 4:2:0 8-bit
    case DXGI_FORMAT_NV11:      // 4:1:1 8-bit

    case WIN10_DXGI_FORMAT_P208: // 4:2:2 8-bit
    case WIN10_DXGI_FORMAT_V208: // 4:4:0 8-bit
    case WIN10_DXGI_FORMAT_V408: // 4:4:4 8-bit
        // These are JPEG Hardware decode formats (DXGI 1.4)

    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        // These are Xbox One platform specific types
        return true;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::IsDepthStencil(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_D16_UNORM:
    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        return true;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::IsBGR(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        return true;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::IsTypeless(DXGI_FORMAT fmt, bool partialTypeless) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC7_TYPELESS:
        return true;

    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        return partialTypeless;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::HasAlpha(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        return true;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
// Returns bits-per-pixel for a given DXGI format, or 0 on failure
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
size_t DirectX::BitsPerPixel(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_YUY2:
    case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        return 32;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
    case WIN10_DXGI_FORMAT_V408:
        return 24;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case WIN10_DXGI_FORMAT_P208:
    case WIN10_DXGI_FORMAT_V208:
    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        return 16;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_NV11:
        return 12;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case XBOX_DXGI_FORMAT_R4G4_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    default:
        return 0;
    }
}


//-------------------------------------------------------------------------------------
// Returns bits-per-color-channel for a given DXGI format, or 0 on failure
// For mixed formats, it returns the largest color-depth in the format
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
size_t DirectX::BitsPerColor(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        return 32;

    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return 24;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_Y216:
    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        return 16;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        return 14;

    case DXGI_FORMAT_R11G11B10_FLOAT:
        return 11;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_Y210:
    case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        return 10;

    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_YUY2:
    case DXGI_FORMAT_NV11:
    case WIN10_DXGI_FORMAT_P208:
    case WIN10_DXGI_FORMAT_V208:
    case WIN10_DXGI_FORMAT_V408:
        return 8;

    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 7;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B5G6R5_UNORM:
        return 6;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return 5;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case XBOX_DXGI_FORMAT_R4G4_UNORM:
    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        return 4;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
        // Palettized formats return 0 for this function

    default:
        return 0;
    }
}


//-------------------------------------------------------------------------------------
// Computes the image row pitch in bytes, and the slice ptich (size in bytes of the image)
// based on DXGI format, width, and height
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::ComputePitch(DXGI_FORMAT fmt, size_t width, size_t height,
    size_t& rowPitch, size_t& slicePitch, CP_FLAGS flags) noexcept
{
    uint64_t pitch = 0;
    uint64_t slice = 0;

    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        assert(IsCompressed(fmt));
        {
            if (flags & CP_FLAGS_BAD_DXTN_TAILS)
            {
                const size_t nbw = width >> 2;
                const size_t nbh = height >> 2;
                pitch = std::max<uint64_t>(1u, uint64_t(nbw) * 8u);
                slice = std::max<uint64_t>(1u, pitch * uint64_t(nbh));
            }
            else
            {
                const uint64_t nbw = std::max<uint64_t>(1u, (uint64_t(width) + 3u) / 4u);
                const uint64_t nbh = std::max<uint64_t>(1u, (uint64_t(height) + 3u) / 4u);
                pitch = nbw * 8u;
                slice = pitch * nbh;
            }
        }
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        assert(IsCompressed(fmt));
        {
            if (flags & CP_FLAGS_BAD_DXTN_TAILS)
            {
                const size_t nbw = width >> 2;
                const size_t nbh = height >> 2;
                pitch = std::max<uint64_t>(1u, uint64_t(nbw) * 16u);
                slice = std::max<uint64_t>(1u, pitch * uint64_t(nbh));
            }
            else
            {
                const uint64_t nbw = std::max<uint64_t>(1u, (uint64_t(width) + 3u) / 4u);
                const uint64_t nbh = std::max<uint64_t>(1u, (uint64_t(height) + 3u) / 4u);
                pitch = nbw * 16u;
                slice = pitch * nbh;
            }
        }
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_YUY2:
        assert(IsPacked(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 4u;
        slice = pitch * uint64_t(height);
        break;

    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        assert(IsPacked(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 8u;
        slice = pitch * uint64_t(height);
        break;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
        if ((height % 2) != 0)
        {
            // Requires a height alignment of 2.
            return E_INVALIDARG;
        }
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 2u;
        slice = pitch * (uint64_t(height) + ((uint64_t(height) + 1u) >> 1));
        break;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        if ((height % 2) != 0)
        {
            // Requires a height alignment of 2.
            return E_INVALIDARG;
        }

        #if (__cplusplus >= 201703L)
            [[fallthrough]];
        #elif defined(__clang__)
            [[clang::fallthrough]];
        #elif defined(_MSC_VER)
            __fallthrough;
        #endif

    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 4u;
        slice = pitch * (uint64_t(height) + ((uint64_t(height) + 1u) >> 1));
        break;

    case DXGI_FORMAT_NV11:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 3u) >> 2) * 4u;
        slice = pitch * uint64_t(height) * 2u;
        break;

    case WIN10_DXGI_FORMAT_P208:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 2u;
        slice = pitch * uint64_t(height) * 2u;
        break;

    case WIN10_DXGI_FORMAT_V208:
        if ((height % 2) != 0)
        {
            // Requires a height alignment of 2.
            return E_INVALIDARG;
        }
        assert(IsPlanar(fmt));
        pitch = uint64_t(width);
        slice = pitch * (uint64_t(height) + (((uint64_t(height) + 1u) >> 1) * 2u));
        break;

    case WIN10_DXGI_FORMAT_V408:
        assert(IsPlanar(fmt));
        pitch = uint64_t(width);
        slice = pitch * (uint64_t(height) + (uint64_t(height >> 1) * 4u));
        break;

    default:
        assert(!IsCompressed(fmt) && !IsPacked(fmt) && !IsPlanar(fmt));
        {
            size_t bpp;

            if (flags & CP_FLAGS_24BPP)
                bpp = 24;
            else if (flags & CP_FLAGS_16BPP)
                bpp = 16;
            else if (flags & CP_FLAGS_8BPP)
                bpp = 8;
            else
                bpp = BitsPerPixel(fmt);

            if (!bpp)
                return E_INVALIDARG;

            if (flags & (CP_FLAGS_LEGACY_DWORD | CP_FLAGS_PARAGRAPH | CP_FLAGS_YMM | CP_FLAGS_ZMM | CP_FLAGS_PAGE4K))
            {
                if (flags & CP_FLAGS_PAGE4K)
                {
                    pitch = ((uint64_t(width) * bpp + 32767u) / 32768u) * 4096u;
                    slice = pitch * uint64_t(height);
                }
                else if (flags & CP_FLAGS_ZMM)
                {
                    pitch = ((uint64_t(width) * bpp + 511u) / 512u) * 64u;
                    slice = pitch * uint64_t(height);
                }
                else if (flags & CP_FLAGS_YMM)
                {
                    pitch = ((uint64_t(width) * bpp + 255u) / 256u) * 32u;
                    slice = pitch * uint64_t(height);
                }
                else if (flags & CP_FLAGS_PARAGRAPH)
                {
                    pitch = ((uint64_t(width) * bpp + 127u) / 128u) * 16u;
                    slice = pitch * uint64_t(height);
                }
                else // DWORD alignment
                {
                    // Special computation for some incorrectly created DDS files based on
                    // legacy DirectDraw assumptions about pitch alignment
                    pitch = ((uint64_t(width) * bpp + 31u) / 32u) * sizeof(uint32_t);
                    slice = pitch * uint64_t(height);
                }
            }
            else
            {
                // Default byte alignment
                pitch = (uint64_t(width) * bpp + 7u) / 8u;
                slice = pitch * uint64_t(height);
            }
        }
        break;
    }

#if defined(_M_IX86) || defined(_M_ARM) || defined(_M_HYBRID_X86_ARM64)
    static_assert(sizeof(size_t) == 4, "Not a 32-bit platform!");
    if (pitch > UINT32_MAX || slice > UINT32_MAX)
    {
        rowPitch = slicePitch = 0;
        return HRESULT_E_ARITHMETIC_OVERFLOW;
    }
#else
    static_assert(sizeof(size_t) == 8, "Not a 64-bit platform!");
#endif

    rowPitch = static_cast<size_t>(pitch);
    slicePitch = static_cast<size_t>(slice);

    return S_OK;
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
size_t DirectX::ComputeScanlines(DXGI_FORMAT fmt, size_t height) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        assert(IsCompressed(fmt));
        return std::max<size_t>(1, (height + 3) / 4);

    case DXGI_FORMAT_NV11:
    case WIN10_DXGI_FORMAT_P208:
        assert(IsPlanar(fmt));
        return height * 2;

    case WIN10_DXGI_FORMAT_V208:
        assert(IsPlanar(fmt));
        return height + (((height + 1) >> 1) * 2);

    case WIN10_DXGI_FORMAT_V408:
        assert(IsPlanar(fmt));
        return height + ((height >> 1) * 4);

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_420_OPAQUE:
    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        assert(IsPlanar(fmt));
        return height + ((height + 1) >> 1);

    default:
        assert(IsValid(fmt));
        assert(!IsCompressed(fmt) && !IsPlanar(fmt));
        return height;
    }
}


//-------------------------------------------------------------------------------------
// Converts to an SRGB equivalent type if available
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
DXGI_FORMAT DirectX::MakeSRGB(DXGI_FORMAT fmt) noexcept
{
    switch (fmt)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    case DXGI_FORMAT_BC1_UNORM:
        return DXGI_FORMAT_BC1_UNORM_SRGB;

    case DXGI_FORMAT_BC2_UNORM:
        return DXGI_FORMAT_BC2_UNORM_SRGB;

    case DXGI_FORMAT_BC3_UNORM:
        return DXGI_FORMAT_BC3_UNORM_SRGB;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

    case DXGI_FORMAT_BC7_UNORM:
        return DXGI_FORMAT_BC7_UNORM_SRGB;

    default:
        return fmt;
    }
}


//-------------------------------------------------------------------------------------
// Converts to an non-SRGB equivalent type
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
DXGI_FORMAT DirectX::MakeLinear(DXGI_FORMAT fmt) noexcept
{
    switch (fmt)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_UNORM;

    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_UNORM;

    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_UNORM;

    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM;

    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_UNORM;

    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_UNORM;

    default:
        return fmt;
    }
}


//-------------------------------------------------------------------------------------
// Converts to a format to an equivalent TYPELESS format if available
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
DXGI_FORMAT DirectX::MakeTypeless(DXGI_FORMAT fmt) noexcept
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_TYPELESS;

    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return DXGI_FORMAT_R32G32B32_TYPELESS;

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_TYPELESS;

    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
        return DXGI_FORMAT_R32G32_TYPELESS;

    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        return DXGI_FORMAT_R10G10B10A2_TYPELESS;

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_TYPELESS;

    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
        return DXGI_FORMAT_R16G16_TYPELESS;

    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        return DXGI_FORMAT_R32_TYPELESS;

    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        return DXGI_FORMAT_R8G8_TYPELESS;

    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        return DXGI_FORMAT_R16_TYPELESS;

    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case XBOX_DXGI_FORMAT_R4G4_UNORM:
        return DXGI_FORMAT_R8_TYPELESS;

    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_TYPELESS;

    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_TYPELESS;

    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_TYPELESS;

    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return DXGI_FORMAT_BC4_TYPELESS;

    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return DXGI_FORMAT_BC5_TYPELESS;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_TYPELESS;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_TYPELESS;

    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        return DXGI_FORMAT_BC6H_TYPELESS;

    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_TYPELESS;

    default:
        return fmt;
    }
}


//-------------------------------------------------------------------------------------
// Converts to a TYPELESS format to an equivalent UNORM format if available
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
DXGI_FORMAT DirectX::MakeTypelessUNORM(DXGI_FORMAT fmt) noexcept
{
    switch (fmt)
    {
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return DXGI_FORMAT_R16G16B16A16_UNORM;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        return DXGI_FORMAT_R10G10B10A2_UNORM;

    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    case DXGI_FORMAT_R16G16_TYPELESS:
        return DXGI_FORMAT_R16G16_UNORM;

    case DXGI_FORMAT_R8G8_TYPELESS:
        return DXGI_FORMAT_R8G8_UNORM;

    case DXGI_FORMAT_R16_TYPELESS:
        return DXGI_FORMAT_R16_UNORM;

    case DXGI_FORMAT_R8_TYPELESS:
        return DXGI_FORMAT_R8_UNORM;

    case DXGI_FORMAT_BC1_TYPELESS:
        return DXGI_FORMAT_BC1_UNORM;

    case DXGI_FORMAT_BC2_TYPELESS:
        return DXGI_FORMAT_BC2_UNORM;

    case DXGI_FORMAT_BC3_TYPELESS:
        return DXGI_FORMAT_BC3_UNORM;

    case DXGI_FORMAT_BC4_TYPELESS:
        return DXGI_FORMAT_BC4_UNORM;

    case DXGI_FORMAT_BC5_TYPELESS:
        return DXGI_FORMAT_BC5_UNORM;

    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        return DXGI_FORMAT_B8G8R8A8_UNORM;

    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        return DXGI_FORMAT_B8G8R8X8_UNORM;

    case DXGI_FORMAT_BC7_TYPELESS:
        return DXGI_FORMAT_BC7_UNORM;

    default:
        return fmt;
    }
}


//-------------------------------------------------------------------------------------
// Converts to a TYPELESS format to an equivalent FLOAT format if available
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
DXGI_FORMAT DirectX::MakeTypelessFLOAT(DXGI_FORMAT fmt) noexcept
{
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
        return DXGI_FORMAT_R32G32B32_FLOAT;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

    case DXGI_FORMAT_R32G32_TYPELESS:
        return DXGI_FORMAT_R32G32_FLOAT;

    case DXGI_FORMAT_R16G16_TYPELESS:
        return DXGI_FORMAT_R16G16_FLOAT;

    case DXGI_FORMAT_R32_TYPELESS:
        return DXGI_FORMAT_R32_FLOAT;

    case DXGI_FORMAT_R16_TYPELESS:
        return DXGI_FORMAT_R16_FLOAT;

    default:
        return fmt;
    }
}


//=====================================================================================
// TexMetadata
//=====================================================================================

size_t TexMetadata::ComputeIndex(size_t mip, size_t item, size_t slice) const noexcept
{
    if (mip >= mipLevels)
        return size_t(-1);

    switch (dimension)
    {
    case TEX_DIMENSION_TEXTURE1D:
    case TEX_DIMENSION_TEXTURE2D:
        if (slice > 0)
            return size_t(-1);

        if (item >= arraySize)
            return size_t(-1);

        return (item*(mipLevels)+mip);

    case TEX_DIMENSION_TEXTURE3D:
        if (item > 0)
        {
            // No support for arrays of volumes
            return size_t(-1);
        }
        else
        {
            size_t index = 0;
            size_t d = depth;

            for (size_t level = 0; level < mip; ++level)
            {
                index += d;
                if (d > 1)
                    d >>= 1;
            }

            if (slice >= d)
                return size_t(-1);

            index += slice;

            return index;
        }

    default:
        return size_t(-1);
    }
}

// Equivalent to D3D11CacluateSubresource: MipSlice + ArraySlice * MipLevels
uint32_t TexMetadata::CalculateSubresource(size_t mip, size_t item) const noexcept
{
    uint32_t result = uint32_t(-1);

    if (mip < mipLevels)
    {
        switch (dimension)
        {
        case TEX_DIMENSION_TEXTURE1D:
        case TEX_DIMENSION_TEXTURE2D:
            if (item < arraySize)
            {
                return static_cast<uint32_t>(mip + item*mipLevels);
            }
            break;

        case TEX_DIMENSION_TEXTURE3D:
            // No support for arrays of volumes
            if (item == 0)
            {
                result = static_cast<uint32_t>(mip);
            }
            break;

        default:
            break;
        }
    }

    return result;
}

// Equivalent to D3D12CacluateSubresource: MipSlice + ArraySlice * MipLevels + PlaneSlice * MipLevels * ArraySize
uint32_t TexMetadata::CalculateSubresource(size_t mip, size_t item, size_t plane) const noexcept
{
    uint32_t result = uint32_t(-1);

    if (mip < mipLevels)
    {
        switch (dimension)
        {
        case TEX_DIMENSION_TEXTURE1D:
        case TEX_DIMENSION_TEXTURE2D:
            if (item < arraySize)
            {
                return static_cast<uint32_t>(mip + item*mipLevels + plane*mipLevels*arraySize);
            }
            break;

        case TEX_DIMENSION_TEXTURE3D:
            // No support for arrays of volumes
            if (item == 0)
            {
                result = static_cast<uint32_t>(mip + plane*mipLevels);
            }
            break;

        default:
            break;
        }
    }

    return result;
}


//=====================================================================================
// Blob - Bitmap image container
//=====================================================================================

Blob& Blob::operator= (Blob&& moveFrom) noexcept
{
    if (this != &moveFrom)
    {
        Release();

        m_buffer = moveFrom.m_buffer;
        m_size = moveFrom.m_size;

        moveFrom.m_buffer = nullptr;
        moveFrom.m_size = 0;
    }
    return *this;
}

void Blob::Release() noexcept
{
    if (m_buffer)
    {
        _aligned_free(m_buffer);
        m_buffer = nullptr;
    }

    m_size = 0;
}

_Use_decl_annotations_
HRESULT Blob::Initialize(size_t size) noexcept
{
    if (!size)
        return E_INVALIDARG;

    Release();

    m_buffer = _aligned_malloc(size, 16);
    if (!m_buffer)
    {
        Release();
        return E_OUTOFMEMORY;
    }

    m_size = size;

    return S_OK;
}

HRESULT Blob::Trim(size_t size) noexcept
{
    if (!size)
        return E_INVALIDARG;

    if (!m_buffer)
        return E_UNEXPECTED;

    if (size > m_size)
        return E_INVALIDARG;

    m_size = size;

    return S_OK;
}

HRESULT Blob::Resize(size_t size) noexcept
{
    if (!size)
        return E_INVALIDARG;

    if (!m_buffer || !m_size)
        return E_UNEXPECTED;

    void *tbuffer = _aligned_malloc(size, 16);
    if (!tbuffer)
        return E_OUTOFMEMORY;

    memcpy(tbuffer, m_buffer, std::min(m_size, size));

    Release();

    m_buffer = tbuffer;
    m_size = size;

    return S_OK;
}
