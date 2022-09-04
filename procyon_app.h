#ifndef PROCYON_APP_H
#define PROCYON_APP_H

#endif // PROCYON_APP_H
#ifdef PROCYON_APP_IMPL

// platform detection
#if defined(_WIN32)
    #define PAPP_WIN32
#else
    #error unsupported platform
#endif

// common macros
#if !defined(PROCYON_DEBUG) && !defined(NDEBUG)
    #define PROCYON_DEBUG
#endif

#ifndef PROCYON_ASSERT
    #ifdef PROCYON_DEBUG
        #include <assert.h>
        #define PROCYON_ASSERT(exp) assert(exp)
    #else
        #define PROCYON_ASSERT(exp) ((void)0)
    #endif
#endif

#ifdef __cplusplus
    #define P_STRUCT(type) type
#else
    #define P_STRUCT(type) (type)
#endif

#define PROCYON_MSG_ASSERT(exp, msg) PROCYON_ASSERT(((void)(msg),exp))

// platform includes and defines
#ifdef PAPP_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <d3d11.h>
    #include <dxgi1_2.h>

    #pragma comment (lib, "gdi32.lib")
    #pragma comment (lib, "user32.lib")
    #pragma comment (lib, "dxguid.lib")
    #pragma comment (lib, "d3d11.lib")
    #pragma comment (lib, "dxgi.lib")
#endif

// common includes
#include <stdbool.h>

// platform specific declarations
#ifdef PAPP_WIN32
typedef struct papp_win32_t
{
    HWND wnd;
} papp_win32_t;

typedef struct papp_d3d11_t
{
    ID3D11Device *dev;
    ID3D11DeviceContext *devcon;
    IDXGISwapChain1 *swap_chain;

    ID3D11Buffer *vbuffer;
    ID3D11Buffer *ibuffer;

    ID3D11InputLayout* layout;
    ID3D11VertexShader* vshader;
    ID3D11PixelShader* pshader;

    ID3D11ShaderResourceView* texture_view;

    ID3D11SamplerState* sampler;
    ID3D11BlendState* blend_state;
    ID3D11RasterizerState* rasterizer_state;
    ID3D11DepthStencilState* depth_stencil_state;

    ID3D11RenderTargetView *render_target_view;
    ID3D11DepthStencilView *depth_stencil_view;

    D3D11_VIEWPORT viewport;
} papp_d3d11_t;

typedef struct papp_d3d11_vertex_t
{
    float position[2];
    float uv[2];
    float color[3];
} papp_d3d11_vertex_t;
#endif

// common declarations
typedef struct papp_t
{
    int width, height;
    bool should_quit;
    #ifdef PAPP_WIN32
        papp_win32_t win32;
        papp_d3d11_t d3d11;
    #endif
} papp_t;
static papp_t papp = {0};

// platform specific macros
#ifdef PAPP_WIN32
    #define PAPP_HR_ASSERT(hr) PROCYON_ASSERT(SUCCEEDED(hr))

    #ifdef __cplusplus
        #define PAPP_WIN32_REFIID(com) __uuidof(com)
    #else
        #define PAPP_WIN32_REFIID(com) &IID_##com
    #endif
#endif

// WIN32
static void papp_win32_fatal_error(const char* message)
{
    MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
    ExitProcess(0);
}

static LRESULT CALLBACK papp_win32_window_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
            papp.should_quit = true;
            return 0;
        default:
            return DefWindowProcW(wnd, msg, wparam, lparam);
    }
}

static void papp_win32_process_incoming_messages()
{
    MSG msg;
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        switch(msg.message)
        {
            case WM_QUIT:
                papp.should_quit = true;
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
                break;
        }
    }
}

static papp_win32_create_window()
{
    HINSTANCE hinstance = GetModuleHandleA(NULL);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = papp_win32_window_proc;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"procyon_app_window_class";

    ATOM atom = RegisterClassExW(&wc);
    PROCYON_MSG_ASSERT(atom, "Failed to register window class");

    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    // WS_EX_NOREDIRECTIONBITMAP flag here is needed to fix ugly bug with Windows 10
    // when window is resized and DXGI swap chain uses FLIP presentation model
    // DO NOT use it if you choose to use non-FLIP presentation model
    DWORD exstyle = WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP;
    DWORD style = WS_OVERLAPPEDWINDOW;

    // uncomment in case you want fixed size window
    // style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    // RECT rect = { 0, 0, 1280, 720 };
    // AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    // width = rect.right - rect.left;
    // height = rect.bottom - rect.top;

    HWND window_handle = CreateWindowExW(
        exstyle, wc.lpszClassName, L"Nie patrz mi sie na tytul", style,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, wc.hInstance, NULL);
    PROCYON_MSG_ASSERT(window_handle, "Failed to create window");

    papp.win32.wnd = window_handle;
    papp.width = width;
    papp.height = height;
}

ULONG papp_D3D11InfoQueue_Release(ID3D11InfoQueue * This)
{
#ifdef __cplusplus
    return This->Release();
#else
    return This->lpVtbl->Release(This);
#endif
}

ULONG papp_D3D11RenderTargetView_Release(ID3D11RenderTargetView * This)
{
#ifdef __cplusplus
    return This->Release();
#else
    return This->lpVtbl->Release(This);
#endif
}

ULONG papp_D3D11DepthStencilView_Release(ID3D11DepthStencilView * This)
{
#ifdef __cplusplus
    return This->Release();
#else
    return This->lpVtbl->Release(This);
#endif
}

ULONG papp_DXGIFactory_Release(IDXGIFactory * This)
{
#ifdef __cplusplus
    return This->Release();
#else
    return This->lpVtbl->Release(This);
#endif
}

ULONG papp_DXGIFactory2_Release(IDXGIFactory2 * This)
{
#ifdef __cplusplus
    return This->Release();
#else
    return This->lpVtbl->Release(This);
#endif
}

ULONG papp_D3D11Texture2D_Release(ID3D11Texture2D * This)
{
#ifdef __cplusplus
    return This->Release();
#else
    return This->lpVtbl->Release(This);
#endif
}

HRESULT papp_D3D11Device_CreateShaderResourceView(ID3D11Device * This, ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView)
{
#ifdef __cplusplus
    return This->CreateShaderResourceView(pResource, pDesc, ppSRView);
#else
    return This->lpVtbl->CreateShaderResourceView(This, pResource, pDesc, ppSRView);
#endif
}

HRESULT papp_D3D11Device_QueryInterface(ID3D11Device * This, REFIID riid, void **ppvObject)
{
#ifdef __cplusplus
    return This->QueryInterface(riid, ppvObject);
#else
    return This->lpVtbl->QueryInterface(This, riid, ppvObject);
#endif
}

HRESULT papp_D3D11InfoQueue_SetBreakOnSeverity(ID3D11InfoQueue * This, D3D11_MESSAGE_SEVERITY Severity, BOOL bEnable)
{
#ifdef __cplusplus
    return This->SetBreakOnSeverity(Severity, bEnable);
#else
    return This->lpVtbl->SetBreakOnSeverity(This, Severity, bEnable);
#endif
}

HRESULT papp_DXGIFactory2_CreateSwapChainForHwnd(IDXGIFactory2 * This, IUnknown *pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1 *pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain)
{
#ifdef __cplusplus
    return This->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
#else
    return This->lpVtbl->CreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
#endif
}

HRESULT papp_DXGISwapChain1_GetParent(IDXGISwapChain1 * This, REFIID riid, void **ppParent)
{
#ifdef __cplusplus
    return This->GetParent(riid, ppParent);
#else
    return This->lpVtbl->GetParent(This, riid, ppParent);
#endif
}

HRESULT papp_DXGIFactory_MakeWindowAssociation(IDXGIFactory * This, HWND WindowHandle, UINT Flags)
{
#ifdef __cplusplus
    return This->MakeWindowAssociation(WindowHandle, Flags);
#else
    return This->lpVtbl->MakeWindowAssociation(This, WindowHandle, Flags);
#endif
}

HRESULT papp_D3D11Device_CreateBuffer(ID3D11Device * This, const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer)
{
#ifdef __cplusplus
    return This->CreateBuffer(pDesc, pInitialData, ppBuffer);
#else
    return This->lpVtbl->CreateBuffer(This, pDesc, pInitialData, ppBuffer);
#endif
}

HRESULT papp_D3D11Device_CreateVertexShader(ID3D11Device * This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader)
{
#ifdef __cplusplus
    return This->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
#else
    return This->lpVtbl->CreateVertexShader(This, pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
#endif
}

HRESULT papp_D3D11Device_CreatePixelShader(ID3D11Device * This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader)
{
#ifdef __cplusplus
    return This->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
#else
    return This->lpVtbl->CreatePixelShader(This, pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
#endif
}

HRESULT papp_D3D11Device_CreateInputLayout(ID3D11Device * This, const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature,SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout)
{
#ifdef __cplusplus
    return This->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
#else
    return This->lpVtbl->CreateInputLayout(This, pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
#endif
}

HRESULT papp_D3D11Device_CreateSamplerState(ID3D11Device * This, const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState)
{
#ifdef __cplusplus
    return This->CreateSamplerState(pSamplerDesc, ppSamplerState);
#else
    return This->lpVtbl->CreateSamplerState(This, pSamplerDesc, ppSamplerState);
#endif
}

HRESULT papp_D3D11Device_CreateBlendState(ID3D11Device * This, const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState)
{
#ifdef __cplusplus
    return This->CreateBlendState(pBlendStateDesc, ppBlendState);
#else
    return This->lpVtbl->CreateBlendState(This, pBlendStateDesc, ppBlendState);
#endif
}

HRESULT papp_D3D11Device_CreateRasterizerState(ID3D11Device * This, const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState)
{
#ifdef __cplusplus
    return This->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
#else
    return This->lpVtbl->CreateRasterizerState(This, pRasterizerDesc, ppRasterizerState);
#endif
}

HRESULT papp_D3D11Device_CreateDepthStencilState(ID3D11Device * This, const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState)
{
#ifdef __cplusplus
    return This->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
#else
    return This->lpVtbl->CreateDepthStencilState(This, pDepthStencilDesc, ppDepthStencilState);
#endif
}

void papp_D3D11DeviceContext_ClearState(ID3D11DeviceContext * This)
{
#ifdef __cplusplus
    return This->ClearState();
#else
    This->lpVtbl->ClearState(This);
#endif
}

HRESULT papp_DXGISwapChain1_ResizeBuffers(IDXGISwapChain1 * This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
#ifdef __cplusplus
    return This->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
#else
    return This->lpVtbl->ResizeBuffers(This, BufferCount, Width, Height, NewFormat, SwapChainFlags);
#endif
}

HRESULT papp_DXGISwapChain1_GetBuffer(IDXGISwapChain1 * This, UINT Buffer, REFIID riid, void **ppSurface)
{
#ifdef __cplusplus
    return This->GetBuffer(Buffer, riid, ppSurface);
#else
    return This->lpVtbl->GetBuffer(This, Buffer, riid, ppSurface);
#endif
}

HRESULT papp_D3D11Device_CreateRenderTargetView(ID3D11Device * This, ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView)
{
#ifdef __cplusplus
    return This->CreateRenderTargetView(pResource, pDesc, ppRTView);
#else
    return This->lpVtbl->CreateRenderTargetView(This, pResource, pDesc, ppRTView);
#endif
}

HRESULT papp_D3D11Device_CreateTexture2D(ID3D11Device * This, const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D)
{
#ifdef __cplusplus
    return This->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
#else
    return This->lpVtbl->CreateTexture2D(This, pDesc, pInitialData, ppTexture2D);
#endif
}

HRESULT papp_D3D11Device_CreateDepthStencilView(ID3D11Device * This, ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView)
{
#ifdef __cplusplus
    return This->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
#else
    return This->lpVtbl->CreateDepthStencilView(This, pResource, pDesc, ppDepthStencilView);
#endif
}

void papp_D3D11DeviceContext_ClearRenderTargetView(ID3D11DeviceContext * This, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4])
{
#ifdef __cplusplus
    return This->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
#else
    This->lpVtbl->ClearRenderTargetView(This, pRenderTargetView, ColorRGBA);
#endif
}

void papp_D3D11DeviceContext_ClearDepthStencilView(ID3D11DeviceContext * This, ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
{
#ifdef __cplusplus
    return This->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
#else
    This->lpVtbl->ClearDepthStencilView(This, pDepthStencilView, ClearFlags, Depth, Stencil);
#endif
}

void papp_D3D11DeviceContext_IASetInputLayout(ID3D11DeviceContext * This, ID3D11InputLayout *pInputLayout)
{
#ifdef __cplusplus
    return This->IASetInputLayout(pInputLayout);
#else
    This->lpVtbl->IASetInputLayout(This, pInputLayout);
#endif
}

void papp_D3D11DeviceContext_IASetPrimitiveTopology(ID3D11DeviceContext * This, D3D11_PRIMITIVE_TOPOLOGY Topology)
{
#ifdef __cplusplus
    return This->IASetPrimitiveTopology(Topology);
#else
    This->lpVtbl->IASetPrimitiveTopology(This, Topology);
#endif
}

void papp_D3D11DeviceContext_IASetVertexBuffers(ID3D11DeviceContext * This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets)
{
#ifdef __cplusplus
    return This->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
#else
    This->lpVtbl->IASetVertexBuffers(This, StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
#endif
}

void papp_D3D11DeviceContext_IASetIndexBuffer(ID3D11DeviceContext * This, ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
#ifdef __cplusplus
    return This->IASetIndexBuffer(pIndexBuffer, Format, Offset);
#else
    This->lpVtbl->IASetIndexBuffer(This, pIndexBuffer, Format, Offset);
#endif
}

void papp_D3D11DeviceContext_VSSetShader(ID3D11DeviceContext * This, ID3D11VertexShader *pVertexShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
#ifdef __cplusplus
    return This->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
#else
    This->lpVtbl->VSSetShader(This, pVertexShader, ppClassInstances, NumClassInstances);
#endif
}

void papp_D3D11DeviceContext_RSSetViewports(ID3D11DeviceContext * This, UINT NumViewports, const D3D11_VIEWPORT *pViewports)
{
#ifdef __cplusplus
    return This->RSSetViewports(NumViewports, pViewports);
#else
    This->lpVtbl->RSSetViewports(This, NumViewports, pViewports);
#endif
}

void papp_D3D11DeviceContext_RSSetState(ID3D11DeviceContext * This, ID3D11RasterizerState *pRasterizerState)
{
#ifdef __cplusplus
    return This->RSSetState(pRasterizerState);
#else
    This->lpVtbl->RSSetState(This, pRasterizerState);
#endif
}

void papp_D3D11DeviceContext_PSSetSamplers(ID3D11DeviceContext * This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers)
{
#ifdef __cplusplus
    return This->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
#else
    This->lpVtbl->PSSetSamplers(This, StartSlot, NumSamplers, ppSamplers);
#endif
}

void papp_D3D11DeviceContext_PSSetShaderResources(ID3D11DeviceContext * This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
#ifdef __cplusplus
    return This->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
#else
    This->lpVtbl->PSSetShaderResources(This, StartSlot, NumViews, ppShaderResourceViews);
#endif
}

void papp_D3D11DeviceContext_PSSetShader(ID3D11DeviceContext * This, ID3D11PixelShader *pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
{
#ifdef __cplusplus
    return This->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
#else
    This->lpVtbl->PSSetShader(This, pPixelShader, ppClassInstances, NumClassInstances);
#endif
}

void papp_D3D11DeviceContext_OMSetBlendState(ID3D11DeviceContext * This, ID3D11BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask)
{
#ifdef __cplusplus
    return This->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
#else
    This->lpVtbl->OMSetBlendState(This, pBlendState, BlendFactor, SampleMask);
#endif
}

void papp_D3D11DeviceContext_OMSetDepthStencilState(ID3D11DeviceContext *This, ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef)
{
#ifdef __cplusplus
    return This->OMSetDepthStencilState(pDepthStencilState, StencilRef);
#else
    This->lpVtbl->OMSetDepthStencilState(This, pDepthStencilState, StencilRef);
#endif
}

void papp_D3D11DeviceContext_DrawIndexed(ID3D11DeviceContext * This, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
#ifdef __cplusplus
    return This->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
#else
    This->lpVtbl->DrawIndexed(This, IndexCount, StartIndexLocation, BaseVertexLocation);
#endif
}

HRESULT papp_DXGISwapChain1_Present(IDXGISwapChain1 * This, UINT SyncInterval, UINT Flags)
{
#ifdef __cplusplus
    return This->Present(SyncInterval, Flags);
#else
    return This->lpVtbl->Present(This, SyncInterval, Flags);
#endif
}

void papp_D3D11DeviceContext_OMSetRenderTargets(ID3D11DeviceContext * This, UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView)
{
#ifdef __cplusplus
    return This->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
#else
    This->lpVtbl->OMSetRenderTargets(This, NumViews, ppRenderTargetViews, pDepthStencilView);
#endif
}

void papp_D3D11DeviceContext_Draw(ID3D11DeviceContext * This, UINT VertexCount, UINT StartVertexLocation)
{
#ifdef __cplusplus
    return This->Draw(VertexCount, StartVertexLocation);
#else
    This->lpVtbl->Draw(This, VertexCount, StartVertexLocation);
#endif
}

void papp_d3d11_init()
{
    HRESULT hr;

    // device and device context
    {
        UINT flags = 0;
        #ifdef PROCYON_DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
        hr = D3D11CreateDevice(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, levels, ARRAYSIZE(levels),
            D3D11_SDK_VERSION, &papp.d3d11.dev, NULL, &papp.d3d11.devcon);
        PAPP_HR_ASSERT(hr);
    }

    #ifdef PROCYON_DEBUG
    {
        ID3D11InfoQueue* info;
        papp_D3D11Device_QueryInterface(papp.d3d11.dev, &IID_ID3D11InfoQueue, &info);
        papp_D3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        papp_D3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        papp_D3D11InfoQueue_Release(info);
    }
    #endif

    // swap chain
    {
	    IDXGIFactory2* factory;
	    hr = CreateDXGIFactory(PAPP_WIN32_REFIID(IDXGIFactory2), (LPVOID*)&factory);
	    PAPP_HR_ASSERT(hr);

        DXGI_SWAP_CHAIN_DESC1 desc = {0};
        desc.Width = 0;
        desc.Height = 0;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        // FLIP presentation model does not allow MSAA framebuffer
        // if you want MSAA then you'll need to render offscreen and manually
        // resolve to non-MSAA framebuffer
        desc.SampleDesc = P_STRUCT(DXGI_SAMPLE_DESC){1, 0};
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_NONE;
        // use more efficient FLIP presentation model
        // Windows 10 allows to use DXGI_SWAP_EFFECT_FLIP_DISCARD
        // for Windows 8 compatibility use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
        // for Windows 7 compatibility use DXGI_SWAP_EFFECT_DISCARD
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        hr = papp_DXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown *)papp.d3d11.dev, papp.win32.wnd, &desc, NULL, NULL, &papp.d3d11.swap_chain);
        PAPP_HR_ASSERT(hr);

	    papp_DXGIFactory2_Release(factory);
    }

    {
	    IDXGIFactory* factory;
        papp_DXGISwapChain1_GetParent(papp.d3d11.swap_chain, PAPP_WIN32_REFIID(IDXGIFactory), (LPVOID *)&factory);
        papp_DXGIFactory_MakeWindowAssociation(factory, papp.win32.wnd, DXGI_MWA_NO_ALT_ENTER);
        papp_DXGIFactory_Release(factory);
    }

    // vertex buffer
    {
        struct papp_d3d11_vertex_t data[] =
        {
            { { -0.5f, +0.5f }, {  0.0f, 25.0f }, { 1, 0, 0 } },
            { { +0.5f, +0.5f }, { 25.0f, 25.0f }, { 0, 1, 0 } },
            { { +0.5f, -0.5f }, { 25.0f,  0.0f }, { 0, 0, 1 } },
            { { -0.5f, -0.5f }, {  0.0f,  0.0f }, { 1, 1, 1 } },
        };

        D3D11_BUFFER_DESC desc = {0};
        desc.ByteWidth = sizeof(data);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initial = {0};
        initial.pSysMem = data;
        papp_D3D11Device_CreateBuffer(papp.d3d11.dev, &desc, &initial, &papp.d3d11.vbuffer);
    }

    // index buffer
    {
        unsigned short data[] =
        {
            0, 1, 3,
            1, 2, 3
        };

        D3D11_BUFFER_DESC desc = {0};
        desc.ByteWidth = sizeof(data);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initial = {0};
        initial.pSysMem = data;
        papp_D3D11Device_CreateBuffer(papp.d3d11.dev, &desc, &initial, &papp.d3d11.ibuffer);
    }

    // layout
    // vshader
    // pshader
    {
        // these must match vertex shader input layout (VS_INPUT in vertex shader source below)
        D3D11_INPUT_ELEMENT_DESC desc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(papp_d3d11_vertex_t, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(papp_d3d11_vertex_t, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(papp_d3d11_vertex_t, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        #include "d3d11_vshader.h"
        #include "d3d11_pshader.h"

        papp_D3D11Device_CreateVertexShader(papp.d3d11.dev, d3d11_vshader, sizeof(d3d11_vshader), NULL, &papp.d3d11.vshader);
        papp_D3D11Device_CreatePixelShader(papp.d3d11.dev, d3d11_pshader, sizeof(d3d11_pshader), NULL, &papp.d3d11.pshader);
        papp_D3D11Device_CreateInputLayout(papp.d3d11.dev, desc, ARRAYSIZE(desc), d3d11_vshader, sizeof(d3d11_vshader), &papp.d3d11.layout);
    }

    // texture view
    {
        unsigned int pixels[] =
        {
            0x00000000, 0xffffffff,
            0xffffffff, 0x00000000,
        };
        UINT width = 2;
        UINT height = 2;

        D3D11_TEXTURE2D_DESC desc = {0};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc = P_STRUCT(DXGI_SAMPLE_DESC){1, 0};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA data = {0};
        data.pSysMem = pixels;
        data.SysMemPitch = width * sizeof(unsigned int);

        ID3D11Texture2D* texture;
        papp_D3D11Device_CreateTexture2D(papp.d3d11.dev, &desc, &data, &texture);
        papp_D3D11Device_CreateShaderResourceView(papp.d3d11.dev, (ID3D11Resource*)texture, NULL, &papp.d3d11.texture_view);
        papp_D3D11Texture2D_Release(texture);
    }

    // sampler
    {
        D3D11_SAMPLER_DESC desc = {0};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        papp_D3D11Device_CreateSamplerState(papp.d3d11.dev, &desc, &papp.d3d11.sampler);
    }

    // blend state
    {
        D3D11_BLEND_DESC desc = {0};
        D3D11_RENDER_TARGET_BLEND_DESC *target_desc = &desc.RenderTarget[0];
        target_desc->BlendEnable = TRUE;
        target_desc->SrcBlend = D3D11_BLEND_SRC_ALPHA;
        target_desc->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        target_desc->BlendOp = D3D11_BLEND_OP_ADD;
        target_desc->SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        target_desc->DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        target_desc->BlendOpAlpha = D3D11_BLEND_OP_ADD;
        target_desc->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        papp_D3D11Device_CreateBlendState(papp.d3d11.dev, &desc, &papp.d3d11.blend_state);
    }

    // rasterizer state
    {
        D3D11_RASTERIZER_DESC desc = {0};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;

        papp_D3D11Device_CreateRasterizerState(papp.d3d11.dev, &desc, &papp.d3d11.rasterizer_state);
    }

    // depth stencil state
    {
        D3D11_DEPTH_STENCIL_DESC desc = {0};
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = FALSE;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        // desc.FrontFace = ...
        // desc.BackFace = ...

        papp_D3D11Device_CreateDepthStencilState(papp.d3d11.dev, &desc, &papp.d3d11.depth_stencil_state);
    }
}

void papp_win32_init()
{
    papp_win32_create_window();
    papp_d3d11_init();
    ShowWindow(papp.win32.wnd, SW_SHOWDEFAULT);
}

void papp_d3d11_prepare_frame()
{
    RECT rect;
    GetClientRect(papp.win32.wnd, &rect);
    int new_width = rect.right - rect.left;
    int new_height = rect.bottom - rect.top;

    if (papp.d3d11.render_target_view == NULL ||
        new_width != papp.width ||
        new_height != papp.height)
    {
        if (papp.d3d11.render_target_view)
        {
            papp_D3D11DeviceContext_ClearState(papp.d3d11.devcon);
            papp_D3D11RenderTargetView_Release(papp.d3d11.render_target_view);
            papp_D3D11DepthStencilView_Release(papp.d3d11.depth_stencil_view);
            papp.d3d11.render_target_view = NULL;
        }

        if (new_width != 0 && new_height != 0)
        {
            HRESULT hr = papp_DXGISwapChain1_ResizeBuffers(papp.d3d11.swap_chain, 0, new_width, new_height, DXGI_FORMAT_UNKNOWN, 0);
            if (FAILED(hr))
            {
                papp_win32_fatal_error("Failed to resize swap chain!");
            }

            ID3D11Texture2D *backbuffer;
            papp_DXGISwapChain1_GetBuffer(papp.d3d11.swap_chain, 0, PAPP_WIN32_REFIID(ID3D11Texture2D), (LPVOID *)&backbuffer);
            papp_D3D11Device_CreateRenderTargetView(papp.d3d11.dev, (ID3D11Resource *)backbuffer, NULL, &papp.d3d11.render_target_view);
            papp_D3D11Texture2D_Release(backbuffer);

            D3D11_TEXTURE2D_DESC depth_desc = {0};
            depth_desc.Width = new_width;
            depth_desc.Height = new_height;
            depth_desc.MipLevels = 1;
            depth_desc.ArraySize = 1;
            depth_desc.Format = DXGI_FORMAT_D32_FLOAT; // or use DXGI_FORMAT_D32_FLOAT_S8X24_UINT if you need stencil
            depth_desc.SampleDesc = P_STRUCT(DXGI_SAMPLE_DESC){1, 0};
            depth_desc.Usage = D3D11_USAGE_DEFAULT;
            depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

            ID3D11Texture2D *depth;
            papp_D3D11Device_CreateTexture2D(papp.d3d11.dev, &depth_desc, NULL, &depth);
            papp_D3D11Device_CreateDepthStencilView(papp.d3d11.dev, (ID3D11Resource *)depth, NULL, &papp.d3d11.depth_stencil_view);
            papp_D3D11Texture2D_Release(depth);
        }

        papp.width = new_width;
        papp.height = new_height;
        papp.d3d11.viewport.TopLeftX = 0;
        papp.d3d11.viewport.TopLeftY = 0;
        papp.d3d11.viewport.Width = (FLOAT)new_width;
        papp.d3d11.viewport.Height = (FLOAT)new_height;
        papp.d3d11.viewport.MinDepth = 0;
        papp.d3d11.viewport.MaxDepth = 1;
    }

}

void papp_d3d11_clear(float r, float g, float b, float a)
{
    FLOAT color[] = { r, g, b, a };
    papp_D3D11DeviceContext_ClearRenderTargetView(papp.d3d11.devcon, papp.d3d11.render_target_view, color);
    papp_D3D11DeviceContext_ClearDepthStencilView(papp.d3d11.devcon, papp.d3d11.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void papp_render_stuff()
{
    papp_D3D11DeviceContext_IASetInputLayout(papp.d3d11.devcon, papp.d3d11.layout);
    papp_D3D11DeviceContext_IASetPrimitiveTopology(papp.d3d11.devcon, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    UINT stride = sizeof(papp_d3d11_vertex_t);
    UINT offset = 0;
    papp_D3D11DeviceContext_IASetVertexBuffers(papp.d3d11.devcon, 0, 1, &papp.d3d11.vbuffer, &stride, &offset);
    papp_D3D11DeviceContext_IASetIndexBuffer(papp.d3d11.devcon, papp.d3d11.ibuffer, DXGI_FORMAT_R16_UINT, 0);

    papp_D3D11DeviceContext_VSSetShader(papp.d3d11.devcon, papp.d3d11.vshader, NULL, 0);

    papp_D3D11DeviceContext_RSSetViewports(papp.d3d11.devcon, 1, &papp.d3d11.viewport);
    papp_D3D11DeviceContext_RSSetState(papp.d3d11.devcon, papp.d3d11.rasterizer_state);

    papp_D3D11DeviceContext_PSSetSamplers(papp.d3d11.devcon, 0, 1, &papp.d3d11.sampler);
    papp_D3D11DeviceContext_PSSetShaderResources(papp.d3d11.devcon, 0, 1, &papp.d3d11.texture_view);
    papp_D3D11DeviceContext_PSSetShader(papp.d3d11.devcon, papp.d3d11.pshader, NULL, 0);

    papp_D3D11DeviceContext_OMSetBlendState(papp.d3d11.devcon, papp.d3d11.blend_state, NULL, ~0U);
    papp_D3D11DeviceContext_OMSetDepthStencilState(papp.d3d11.devcon, papp.d3d11.depth_stencil_state, 0);
    papp_D3D11DeviceContext_OMSetRenderTargets(papp.d3d11.devcon, 1, &papp.d3d11.render_target_view, papp.d3d11.depth_stencil_view);

    // ID3D11DeviceContext_Draw(papp.d3d11.context, 3, 0);
    papp_D3D11DeviceContext_DrawIndexed(papp.d3d11.devcon, 6, 0, 0);
}

void papp_d3d11_flip()
{
    BOOL vsync = TRUE;
    HRESULT hr = papp_DXGISwapChain1_Present(papp.d3d11.swap_chain, vsync ? 1 : 0, 0);
    if (hr == DXGI_STATUS_OCCLUDED)
    {
        if (vsync)
            Sleep(10);
    }
    else if (FAILED(hr))
    {
        papp_win32_fatal_error("Failed to present swap chain! Device lost?");
    }

    papp_d3d11_prepare_frame();
}
#endif // PROCYON_APP_IMPL