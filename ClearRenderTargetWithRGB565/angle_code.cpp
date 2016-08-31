#include <windows.h>
#include <d3d11_1.h>

HRESULT CompileShaderFromFile(WCHAR *szFileName,
                              LPCSTR szEntryPoint,
                              LPCSTR szShaderModel,
                              ID3DBlob **ppBlobOut);

namespace
{
ID3D11VertexShader *vertexShader             = nullptr;
ID3D11PixelShader *pixelShader               = nullptr;
ID3D11RenderTargetView *renderTargetView     = nullptr;
ID3D11InputLayout *inputLayout               = nullptr;
ID3D11Buffer *vertexBuffer                   = nullptr;
ID3D11Texture2D *texture                     = nullptr;
ID3D11ShaderResourceView *shaderResourceView = nullptr;
ID3D11SamplerState *samplerState             = nullptr;
int windowWidth                              = 0;
int windowHeight                             = 0;

int cellSize                     = 128;
const size_t StreamingBufferSize = 1024 * 1024;
}

HRESULT angle_code_init(ID3D11Device *d3dDevice,
                        ID3D11DeviceContext *d3dContext,
                        int width,
                        int height)
{
    HRESULT hr = S_OK;

    // Compile the vertex shader
    ID3DBlob *pVSBlob = nullptr;
    hr                = CompileShaderFromFile(L"VS.hlsl", "main", "vs_5_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   "The FX file cannot be compiled.  Please run this executable from the directory "
                   "that contains the FX file.",
                   "Error", MB_OK);
        return hr;
    }

    // Create the vertex shader
    hr = d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
                                       nullptr, &vertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    if (FAILED(hr))
        return hr;

    // Compile the pixel shader
    ID3DBlob *pPSBlob = nullptr;
    hr                = CompileShaderFromFile(L"PS.hlsl", "main", "ps_5_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   "The FX file cannot be compiled.  Please run this executable from the directory "
                   "that contains the FX file.",
                   "Error", MB_OK);
        return hr;
    }

    // Create the pixel shader
    hr = d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
                                      nullptr, &pixelShader);
    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width    = (FLOAT)width;
    vp.Height   = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dContext->RSSetViewports(1, &vp);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = d3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                      pVSBlob->GetBufferSize(), &inputLayout);

    // Set the input layout
    d3dContext->IASetInputLayout(inputLayout);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth      = StreamingBufferSize;
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = d3dDevice->CreateBuffer(&bd, nullptr, &vertexBuffer);
    if (FAILED(hr))
        return hr;

    // Set primitive topology
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width              = cellSize;
    textureDesc.Height             = cellSize;
    textureDesc.MipLevels          = 1;
    textureDesc.ArraySize          = 1;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Format             = DXGI_FORMAT_B5G6R5_UNORM;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags     = 0;
    hr                             = d3dDevice->CreateTexture2D(&textureDesc, nullptr, &texture);
    if (FAILED(hr))
        return hr;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    rtvDesc.Format             = DXGI_FORMAT_B5G6R5_UNORM;
    rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    hr = d3dDevice->CreateRenderTargetView(texture, &rtvDesc, &renderTargetView);
    if (FAILED(hr))
        return hr;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format                    = DXGI_FORMAT_B5G6R5_UNORM;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels       = 1;
    hr = d3dDevice->CreateShaderResourceView(texture, &srvDesc, &shaderResourceView);
    if (FAILED(hr))
        return hr;

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.MaxLOD         = 1000.0f;
    samplerDesc.MinLOD         = -1000.0f;
    hr                         = d3dDevice->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(hr))
        return hr;

    pVSBlob->Release();
    pPSBlob->Release();

    windowWidth  = width;
    windowHeight = height;

    return S_OK;
}

void angle_code_deinit()
{
    if (vertexShader)
        vertexShader->Release();
    if (pixelShader)
        pixelShader->Release();
    if (renderTargetView)
        renderTargetView->Release();
    if (inputLayout)
        inputLayout->Release();
    if (vertexBuffer)
        vertexBuffer->Release();
    if (shaderResourceView)
        shaderResourceView->Release();
    if (texture)
        texture->Release();
    if (samplerState)
        samplerState->Release();
}

void angle_code_render(ID3D11DeviceContext *d3dContext)
{
    d3dContext->VSSetShader(vertexShader, nullptr, 0);
    d3dContext->PSSetShader(pixelShader, nullptr, 0);
    d3dContext->PSSetShaderResources(0, 1, &shaderResourceView);
    d3dContext->PSSetSamplers(0, 1, &samplerState);

    float clearValues[4] = { 0.0, 0.0, 1.0, 1.0 };
    d3dContext->ClearRenderTargetView(renderTargetView, clearValues);

    int minX = 0;
    int minY = 0;
    int maxX = cellSize;
    int maxY = cellSize;

    auto l = [](int v, int s) {
        return static_cast<float>(v) / static_cast<float>(s) * 2.0f - 1.0f;
    };

    float minXf = l(minX, windowWidth);
    float maxXf = l(maxX, windowWidth);
    float minYf = l(minY, windowHeight);
    float maxYf = l(maxY, windowHeight);

    const float vertices[] = {
        minXf, maxYf, minXf, minYf, maxXf, minYf,

        minXf, maxYf, maxXf, minYf, maxXf, maxYf,
    };

    UINT dataSize = sizeof(float) * 6 * 2;

    D3D11_MAPPED_SUBRESOURCE mapInfo;

    if (dataSize >= StreamingBufferSize)
    {
        d3dContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapInfo);
    }
    else
    {
        d3dContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapInfo);
    }

    unsigned char *writePointer = static_cast<unsigned char *>(mapInfo.pData);
    memcpy(&writePointer[0], vertices, dataSize);

    d3dContext->Unmap(vertexBuffer, 0);

    // Set vertex buffer
    UINT stride = sizeof(float) * 2;
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Render a triangle
    d3dContext->Draw(6, 0);
}
