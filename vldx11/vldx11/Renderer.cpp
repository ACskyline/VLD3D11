#include "Renderer.h"



Renderer::Renderer(uint32_t _width, uint32_t _height, XMVECTORF32 _clearColor, float _clearDepth, uint8_t _clearStencil) :
	width(_width), height(_height), frame(0), clearColor(_clearColor), clearDepth(_clearDepth), clearStencil(_clearStencil),
	SwapChain(nullptr), 
	d3d11Device(nullptr), 
	d3d11DevCon(nullptr), 
	renderTargetView(nullptr),
	depthStencilBuffer(nullptr),
	depthStencilView(nullptr)
{
}

Renderer::~Renderer()
{
	if (SwapChain!=nullptr) SwapChain->Release();
	if (d3d11Device != nullptr) d3d11Device->Release();
	if (d3d11DevCon != nullptr) d3d11DevCon->Release();
	if (renderTargetView != nullptr) renderTargetView->Release();
	if (depthStencilView != nullptr) depthStencilView->Release();
	if (depthStencilBuffer != nullptr) depthStencilBuffer->Release();
}

bool Renderer::InitRenderer(HWND hwnd)
{
	if (!InitD3D11App(hwnd)) return false;
	InitViewport();
}

bool Renderer::InitD3D11App(HWND hwnd)
{
	HRESULT hr;

	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;//double buffer
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);
	if (!CheckError(hr, nullptr)) return false;

	//Create our Default Render Target View
	if (!InitDefaultRenderTargetView()) return false;

	//Create our Depth Stencil View
	if (!InitDepthStencilView()) return false;

	//Set our Render Target
	SetDefaultRenderTarget();

	return true;
}

bool Renderer::InitDefaultRenderTargetView()
{
	HRESULT hr;

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create our Render Target
	hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);

	//Release our BackBuffer
	BackBuffer->Release();

	if (!CheckError(hr, nullptr)) return false;
}

bool Renderer::InitDepthStencilView()
{
	HRESULT hr;
	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	if (!CheckError(hr, nullptr)) return false;

	hr = d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (!CheckError(hr, nullptr)) return false;

	return true;
}

void Renderer::SetDefaultRenderTarget()
{
	currentRTV = renderTargetView;
	currentDSV = depthStencilView;
	d3d11DevCon->OMSetRenderTargets(1, &currentRTV, currentDSV);
}

void Renderer::SetRenderTarget(RenderTexture* renderTexture)
{
	if (renderTexture->HasRenderTargetView()) currentRTV = renderTexture->GetRenderTargetView();
	if (renderTexture->HasDepthStencilView()) currentDSV = renderTexture->GetDepthStencilView();
	d3d11DevCon->OMSetRenderTargets(1, &currentRTV, currentDSV);
}

void Renderer::InitViewport()
{
	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);
}

void Renderer::DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec)
{
	//Draw Call
	for (auto item = GlobalDrawableGrpVec.begin(); item != GlobalDrawableGrpVec.end(); item++)
	{
		(*item)->Draw(d3d11DevCon);
	}
}

void Renderer::DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec, Material* _pMat)
{
	//Draw Call
	for (auto item = GlobalDrawableGrpVec.begin(); item != GlobalDrawableGrpVec.end(); item++)
	{
		(*item)->Draw(d3d11DevCon, _pMat);
	}
}

void Renderer::DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec, Material* _pMat, FrameUniform* _pFrameUniform)
{
	//Draw Call
	for (auto item = GlobalDrawableGrpVec.begin(); item != GlobalDrawableGrpVec.end(); item++)
	{
		(*item)->Draw(d3d11DevCon, _pMat, _pFrameUniform);
	}
}

void Renderer::ClearCurrentRenderTargetDefault()
{
	//Clear our backbuffer to the updated color
	if(currentRTV) d3d11DevCon->ClearRenderTargetView(currentRTV, clearColor);
	if(currentDSV) d3d11DevCon->ClearDepthStencilView(currentDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Renderer::ClearCurrentRenderTarget(XMVECTORF32 _clearColor, float _clearDepth, uint8_t _clearStencil)
{
	//Clear our backbuffer to the updated color
	if (currentRTV) d3d11DevCon->ClearRenderTargetView(currentRTV, _clearColor);
	if (currentDSV) d3d11DevCon->ClearDepthStencilView(currentDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, _clearDepth, _clearStencil);
}