//--------------------------------------------------------------------------------------
// File: Tutorial03.cpp
//
// This application displays a triangle using Direct3D 11
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729720.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"

// jm
#include <vector>

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE g_hInst                           = nullptr;
HWND g_hWnd                                 = nullptr;
D3D_DRIVER_TYPE g_driverType                = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL g_featureLevel            = D3D_FEATURE_LEVEL_11_0;
ID3D11Device *g_pd3dDevice                  = nullptr;
ID3D11Device1 *g_pd3dDevice1                = nullptr;
ID3D11DeviceContext *g_pImmediateContext    = nullptr;
ID3D11DeviceContext1 *g_pImmediateContext1  = nullptr;
IDXGISwapChain *g_pSwapChain                = nullptr;
IDXGISwapChain1 *g_pSwapChain1              = nullptr;
ID3D11RenderTargetView *g_pRenderTargetView = nullptr;
ID3D11VertexShader *g_pVertexShader         = nullptr;
ID3D11PixelShader *g_pPixelShader           = nullptr;
ID3D11InputLayout *g_pVertexLayout          = nullptr;
ID3D11Buffer *g_pVertexBuffer               = nullptr;

// jm
const int SRC = 0;
const int DEST = 1;
ID3D11Texture2D *g_color[2] = { nullptr };
ID3D11RenderTargetView *g_rtv[2] = { nullptr };
ID3D11Texture2D *g_depthStencil[2] = { nullptr };
ID3D11DepthStencilView *g_dsv[2] = { nullptr };
ID3D11Texture2D *g_staging[2] = { nullptr };
ID3D11Buffer *g_constants = nullptr;
ID3D11DepthStencilState *g_replaceDS = nullptr;
ID3D11DepthStencilState *g_decrDS = nullptr;
ID3D11DepthStencilState *g_equalDS = nullptr;
UINT width = 0;
UINT height = 0;
ID3D11Texture2D *pBackBuffer = nullptr;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine,
                    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (FAILED(InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(InitDevice()))
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm       = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = {0, 0, 512, 512}; // jm
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 3: Shaders",
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT,
                          CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr,
                          hInstance, nullptr);
    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(WCHAR *szFileName,
                              LPCSTR szEntryPoint,
                              LPCSTR szShaderModel,
                              ID3DBlob **ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob *pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
                            dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char *>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob)
        pErrorBlob->Release();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    width  = rc.right - rc.left;
    height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels,
                               numFeatureLevels, D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel,
                               &g_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry
            // without it
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags,
                                   &featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION,
                                   &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1 *dxgiFactory = nullptr;
    {
        IDXGIDevice *dxgiDevice = nullptr;
        hr                      = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice),
                                          reinterpret_cast<void **>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter *adapter = nullptr;
            hr                    = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1),
                                        reinterpret_cast<void **>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2 *dxgiFactory2 = nullptr;
    hr                          = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2),
                                     reinterpret_cast<void **>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1),
                                          reinterpret_cast<void **>(&g_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)g_pImmediateContext->QueryInterface(
                __uuidof(ID3D11DeviceContext1), reinterpret_cast<void **>(&g_pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width              = width;
        sd.Height             = height;
        sd.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count   = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount        = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr,
                                                  &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain),
                                               reinterpret_cast<void **>(&g_pSwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount                        = 1;
        sd.BufferDesc.Width                   = width;
        sd.BufferDesc.Height                  = height;
        sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator   = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow                       = g_hWnd;
        sd.SampleDesc.Count                   = 1;
        sd.SampleDesc.Quality                 = 0;
        sd.Windowed                           = TRUE;

        hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    hr                           = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                 reinterpret_cast<void **>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width    = (FLOAT)width;
    vp.Height   = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Compile the vertex shader
    ID3DBlob *pVSBlob = nullptr;
    hr                = CompileShaderFromFile(L"StencilBlit.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the "
                   L"directory that contains the FX file.",
                   L"Error", MB_OK);
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
                                          nullptr, &g_pVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                         pVSBlob->GetBufferSize(), &g_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

    // Compile the pixel shader
    ID3DBlob *pPSBlob = nullptr;
    hr                = CompileShaderFromFile(L"StencilBlit.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the "
                   L"directory that contains the FX file.",
                   L"Error", MB_OK);
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
                                         nullptr, &g_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    // Create vertex buffer
    SimpleVertex vertices[] = {
        XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(-0.5f, -0.5f, 0.5f),
    };
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = sizeof(SimpleVertex) * 3;
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr               = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // jm
    for (int idx = SRC; idx <= DEST; ++idx)
    {
        {
            D3D11_TEXTURE2D_DESC colorDesc;
            colorDesc.ArraySize = 1;
            colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
            colorDesc.CPUAccessFlags = 0;
            colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            colorDesc.Height = height;
            colorDesc.MipLevels = 1;
            colorDesc.MiscFlags = 0;
            colorDesc.SampleDesc.Count = 1;
            colorDesc.SampleDesc.Quality = 0;
            colorDesc.Usage = D3D11_USAGE_DEFAULT;
            colorDesc.Width = width;

            hr = g_pd3dDevice->CreateTexture2D(&colorDesc, nullptr, &g_color[idx]);
            if (FAILED(hr))
                return hr;
        }

        {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
            rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            rtvDesc.Texture2D.MipSlice = 0;
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

            hr = g_pd3dDevice->CreateRenderTargetView(g_color[idx], &rtvDesc, &g_rtv[idx]);
            if (FAILED(hr))
                return hr;
        }

        {
            D3D11_TEXTURE2D_DESC dsDesc;
            dsDesc.ArraySize = 1;
            dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            dsDesc.CPUAccessFlags = 0;
            dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
            dsDesc.Height = height;
            dsDesc.MipLevels = 1;
            dsDesc.MiscFlags = 0;
            dsDesc.SampleDesc.Count = 1;
            dsDesc.SampleDesc.Quality = 0;
            dsDesc.Usage = D3D11_USAGE_DEFAULT;
            dsDesc.Width = width;

            hr = g_pd3dDevice->CreateTexture2D(&dsDesc, nullptr, &g_depthStencil[idx]);
            if (FAILED(hr))
                return hr;
        }

        {
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
            dsvDesc.Flags = 0;
            dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            dsvDesc.Texture2D.MipSlice = 0;
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            hr = g_pd3dDevice->CreateDepthStencilView(g_depthStencil[idx], &dsvDesc, &g_dsv[idx]);
            if (FAILED(hr))
                return hr;
        }

        {
            D3D11_TEXTURE2D_DESC stagingDesc;
            stagingDesc.ArraySize = 1;
            stagingDesc.BindFlags = 0;
            stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            stagingDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
            stagingDesc.Height = height;
            stagingDesc.MipLevels = 1;
            stagingDesc.MiscFlags = 0;
            stagingDesc.SampleDesc.Count = 1;
            stagingDesc.SampleDesc.Quality = 0;
            stagingDesc.Usage = D3D11_USAGE_STAGING;
            stagingDesc.Width = width;

            hr = g_pd3dDevice->CreateTexture2D(&stagingDesc, nullptr, &g_staging[idx]);
            if (FAILED(hr))
                return hr;
        }
    }

    {
        D3D11_BUFFER_DESC constantsDesc;
        constantsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantsDesc.ByteWidth = 16;
        constantsDesc.CPUAccessFlags = 0;
        constantsDesc.MiscFlags = 0;
        constantsDesc.StructureByteStride = 0;
        constantsDesc.Usage = D3D11_USAGE_DEFAULT;

        hr = g_pd3dDevice->CreateBuffer(&constantsDesc, nullptr, &g_constants);
        if (FAILED(hr))
            return hr;
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        desc.DepthEnable = TRUE;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        desc.StencilEnable = TRUE;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        hr = g_pd3dDevice->CreateDepthStencilState(&desc, &g_replaceDS);
        if (FAILED(hr))
            return hr;
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.DepthEnable = TRUE;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.StencilEnable = TRUE;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        hr = g_pd3dDevice->CreateDepthStencilState(&desc, &g_decrDS);
        if (FAILED(hr))
            return hr;
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.DepthEnable = FALSE;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.StencilEnable = TRUE;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        hr = g_pd3dDevice->CreateDepthStencilState(&desc, &g_equalDS);
        if (FAILED(hr))
            return hr;
    }

    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_constants);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if (g_pImmediateContext)
        g_pImmediateContext->ClearState();

    // jm
    for (int idx = SRC; idx <= DEST; ++idx)
    {
        if (g_color[idx])
            g_color[idx]->Release();
        if (g_rtv[idx])
            g_rtv[idx]->Release();
        if (g_depthStencil[idx])
            g_depthStencil[idx]->Release();
        if (g_dsv[idx])
            g_dsv[idx]->Release();
        if (g_staging[idx])
            g_staging[idx]->Release();
    }

    if (g_constants)
        g_constants->Release();
    if (g_replaceDS)
        g_replaceDS->Release();
    if (g_decrDS)
        g_decrDS->Release();
    if (g_equalDS)
        g_equalDS->Release();
    if (pBackBuffer)
        pBackBuffer->Release();

    if (g_pVertexBuffer)
        g_pVertexBuffer->Release();
    if (g_pVertexLayout)
        g_pVertexLayout->Release();
    if (g_pVertexShader)
        g_pVertexShader->Release();
    if (g_pPixelShader)
        g_pPixelShader->Release();
    if (g_pRenderTargetView)
        g_pRenderTargetView->Release();
    if (g_pSwapChain1)
        g_pSwapChain1->Release();
    if (g_pSwapChain)
        g_pSwapChain->Release();
    if (g_pImmediateContext1)
        g_pImmediateContext1->Release();
    if (g_pImmediateContext)
        g_pImmediateContext->Release();
    if (g_pd3dDevice1)
        g_pd3dDevice1->Release();
    if (g_pd3dDevice)
        g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------

// jm
void drawQuad(float r, float g, float b, float depth)
{
    std::vector<SimpleVertex> vertices;
    vertices.push_back({ XMFLOAT3(-1.0f, 3.0f, depth) } );
    vertices.push_back({ XMFLOAT3(3.0f, -1.0f, depth) } );
    vertices.push_back({ XMFLOAT3(-1.0f, -1.0f, depth) } );

    UINT size = sizeof(SimpleVertex) * 3;

    g_pImmediateContext->UpdateSubresource(g_pVertexBuffer, 0, nullptr, vertices.data(), size, size);

    float color[4] = { r, g, b, 1.0f };

    g_pImmediateContext->UpdateSubresource(g_constants, 0, nullptr, color, 16, 16);

    g_pImmediateContext->Draw(3, 0);
}

void Render()
{
    // jm
    FLOAT black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_rtv[SRC], black);
    g_pImmediateContext->ClearRenderTargetView(g_rtv[DEST], black);
    g_pImmediateContext->ClearDepthStencilView(g_dsv[SRC], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    g_pImmediateContext->ClearDepthStencilView(g_dsv[DEST], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

    // Fill source with red, depth = 0.5, stencil = 7
    g_pImmediateContext->OMSetRenderTargets(1, &g_rtv[SRC], g_dsv[SRC]);
    g_pImmediateContext->OMSetDepthStencilState(g_replaceDS, 7);
    drawQuad(1.0f, 0.0f, 0.0f, 0.5f);

    // Fill dest with yellow, depth = 0.0, stencil = 1
    g_pImmediateContext->OMSetRenderTargets(1, &g_rtv[DEST], g_dsv[DEST]);
    g_pImmediateContext->OMSetDepthStencilState(g_replaceDS, 1);
    drawQuad(1.0f, 1.0f, 0.0f, 0.0f);

    // Copy the stencil data.
    g_pImmediateContext->CopyResource(g_staging[SRC], g_depthStencil[SRC]);
    g_pImmediateContext->CopyResource(g_staging[DEST], g_depthStencil[DEST]);

    D3D11_MAPPED_SUBRESOURCE mapSrc, mapDest;
    g_pImmediateContext->Map(g_staging[SRC], 0, D3D11_MAP_READ, 0, &mapSrc);
    g_pImmediateContext->Map(g_staging[DEST], 0, D3D11_MAP_WRITE, 0, &mapDest);

    for (UINT y = 0; y < height; ++y)
    {
        for (UINT x = 0; x < width; ++x)
        {
            ptrdiff_t offset = ((x * 4) + 3) + y * mapSrc.RowPitch;
            const uint8_t *src = reinterpret_cast<const uint8_t *>(mapSrc.pData) + offset;
            uint8_t *dest = reinterpret_cast<uint8_t *>(mapDest.pData) + offset;
            memcpy(dest, src, 1u);
        }
    }

    g_pImmediateContext->Unmap(g_staging[SRC], 0);
    g_pImmediateContext->Unmap(g_staging[DEST], 0);

    g_pImmediateContext->CopyResource(g_depthStencil[DEST], g_staging[DEST]);

    // Render blue where depth < 0, decrement on depth failure.
    g_pImmediateContext->OMSetDepthStencilState(g_decrDS, 0);
    drawQuad(0.0f, 0.0f, 1.0f, 0.0f);

    // Render green where stencil == 6.
    g_pImmediateContext->OMSetDepthStencilState(g_equalDS, 6);
    drawQuad(0.0f, 1.0f, 0.0f, 0.0f);

    // Present the information rendered to the back buffer to the front buffer (the screen)
    g_pImmediateContext->CopyResource(pBackBuffer, g_color[DEST]);
    g_pSwapChain->Present(0, 0);
}
