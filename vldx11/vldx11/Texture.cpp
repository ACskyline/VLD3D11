#include "Texture.h"

Texture::Texture(TextureType _type, wstring _fileName) :
	type(_type), fileName(_fileName)
{
}

Texture::Texture(TextureType _type) : type(_type)
{
}

Texture::~Texture()
{
	if (textureRes != nullptr) textureRes->Release();
	if (textureResView != nullptr) textureResView->Release();
	if (samplerState != nullptr) samplerState->Release();
}

bool Texture::InitTexture(ID3D11Device* d3d11Device)
{
	HRESULT hr;
	hr = CreateWICTextureFromFile(d3d11Device, fileName.c_str(), (ID3D11Resource**)(&(textureRes)), &textureResView);
	//hr = CreateWICTextureFromFileEx(d3d11Device, 
	//								fileName.c_str(), 
	//								0, 
	//								D3D11_USAGE_DEFAULT, 
	//								D3D11_BIND_SHADER_RESOURCE, 
	//								0, 
	//								0, 
	//								WIC_LOADER_IGNORE_SRGB, 
	//								(ID3D11Resource**)(&(textureRes)), 
	//								&textureResView);
	if (!CheckError(hr, nullptr)) return false;

	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);//ZeroMemory is bad, CD3D11 contains all helper class

	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//default
	//sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.MinLOD = -FLT_MAX;//default
	//sampDesc.MinLOD = FLT_MAX;//default
	//sampDesc.MipLODBias = 0.f;//default
	//sampDesc.MaxAnisotropy = 1;//default
	//sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;//default
	//sampDesc.BorderColor[0] = 1.f;//default
	//sampDesc.BorderColor[1] = 1.f;//default
	//sampDesc.BorderColor[2] = 1.f;//default
	//sampDesc.BorderColor[3] = 1.f;//default

	hr = d3d11Device->CreateSamplerState(&sampDesc, &samplerState);
	if (!CheckError(hr, nullptr)) return false;

	return true;
}

void Texture::UseTextureData(uint32_t slot, ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetShaderResources(slot, 1, &textureResView);
	d3d11DevCon->PSSetSamplers(slot, 1, &samplerState);
}

void Texture::Debug()
{
	ID3D11Resource* textureResTemp;
	textureResView->GetResource(&textureResTemp);
	ID3D11Texture2D* texture2D = (ID3D11Texture2D*)textureResTemp;
	D3D11_TEXTURE2D_DESC texture2DDesc;

	if(texture2D!=nullptr) texture2D->GetDesc(&texture2DDesc);

	printf("Texture Debug: %d,%d,%d,%d,%d\n",
		texture2DDesc.Format,
		texture2DDesc.ArraySize,
		texture2DDesc.MipLevels,
		texture2DDesc.Width,
		texture2DDesc.Width);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// RENDER TEXTURE
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

RenderTexture::RenderTexture(RenderTextureType _typeRT, int _width, int _height) :
	Texture(Texture::TextureType::Default), 
	typeRT(_typeRT), 
	renderTargetView(nullptr), 
	depthStencilView(nullptr),
	width(_width),
	height(_height)
{
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = _width;
	viewport.Height = _height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

RenderTexture::~RenderTexture()
{
	if (renderTargetView != nullptr) renderTargetView->Release();
	if (depthStencilView != nullptr) depthStencilView->Release();
}

bool RenderTexture::InitTexture(ID3D11Device* d3d11Device)
{
	bool result = false;

	if (typeRT == RenderTexture::RenderTextureType::Default)
	{
		result = InitRenderTextureDefault(d3d11Device);
	}
	else if (typeRT == RenderTexture::RenderTextureType::ShadowMap)
	{
		result = InitRenderTextureShadowMap(d3d11Device);
	}
	else if (typeRT == RenderTexture::RenderTextureType::ShadowMapPCF)
	{
		result = InitRenderTextureShadowMapPCF(d3d11Device);
	}

	return result;
}

bool RenderTexture::InitRenderTextureDefault(ID3D11Device* d3d11Device)
{
	CD3D11_TEXTURE2D_DESC textureDesc;
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);//ZeroMemory is bad, CD3D11 contains all helper class

	HRESULT hr;

	///////////////////////////////////////////////////////////////////////////////
	// Initialize the  texture description.

	// Setup the texture description, bind this texture as a render target AND a shader resource
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the texture
	hr = d3d11Device->CreateTexture2D(&textureDesc, NULL, &textureRes);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	hr = d3d11Device->CreateRenderTargetView(textureRes, &renderTargetViewDesc, &renderTargetView);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	hr = d3d11Device->CreateShaderResourceView(textureRes, &shaderResourceViewDesc, &textureResView);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//default
	//sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.MinLOD = -FLT_MAX;//default
	//sampDesc.MinLOD = FLT_MAX;//default
	//sampDesc.MipLODBias = 0.f;//default
	//sampDesc.MaxAnisotropy = 1;//default
	//sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;//default
	//sampDesc.BorderColor[0] = 1.f;//default
	//sampDesc.BorderColor[1] = 1.f;//default
	//sampDesc.BorderColor[2] = 1.f;//default
	//sampDesc.BorderColor[3] = 1.f;//default

	hr = d3d11Device->CreateSamplerState(&sampDesc, &samplerState);
	if (!CheckError(hr, nullptr)) return false;

	return true;
}

bool RenderTexture::InitRenderTextureShadowMap(ID3D11Device* d3d11Device)
{
	CD3D11_TEXTURE2D_DESC textureDesc;
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);//ZeroMemory is bad, CD3D11 contains all helper class

	HRESULT hr;

	///////////////////////////////////////////////////////////////////////////////
	// Initialize the  texture description.

	// Setup the texture description, bind this texture as a render target AND a shader resource
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16_UNORM;// DXGI_FORMAT_R32_FLOAT;// DXGI_FORMAT_R16G16B16A16_FLOAT;// 
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the texture
	hr = d3d11Device->CreateTexture2D(&textureDesc, NULL, &textureRes);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	hr = d3d11Device->CreateRenderTargetView(textureRes, &renderTargetViewDesc, &renderTargetView);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	hr = d3d11Device->CreateShaderResourceView(textureRes, &shaderResourceViewDesc, &textureResView);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;//linear makes no sense for shadow map
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.MinLOD = -FLT_MAX;//default
	//sampDesc.MinLOD = FLT_MAX;//default
	//sampDesc.MipLODBias = 0.f;//default
	//sampDesc.MaxAnisotropy = 1;//default
	//sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//sampDesc.BorderColor[0] = 1.f;//default
	//sampDesc.BorderColor[1] = 1.f;//default
	//sampDesc.BorderColor[2] = 1.f;//default
	//sampDesc.BorderColor[3] = 1.f;//default

	hr = d3d11Device->CreateSamplerState(&sampDesc, &samplerState);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the depth stencil buffer and view.
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

bool RenderTexture::InitRenderTextureShadowMapPCF(ID3D11Device* d3d11Device)
{
	CD3D11_TEXTURE2D_DESC textureDesc;
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);//ZeroMemory is bad, CD3D11 contains all helper class

	HRESULT hr;

	///////////////////////////////////////////////////////////////////////////////
	// Initialize the  texture description.

	// Setup the texture description, bind this texture as a render target AND a shader resource
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16_UNORM;// DXGI_FORMAT_R32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the texture
	hr = d3d11Device->CreateTexture2D(&textureDesc, NULL, &textureRes);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	hr = d3d11Device->CreateRenderTargetView(textureRes, &renderTargetViewDesc, &renderTargetView);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	hr = d3d11Device->CreateShaderResourceView(textureRes, &shaderResourceViewDesc, &textureResView);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;// D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;//we are using SamplerCmp in shader, so we need the COMPARISON version of filters
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//default
	//sampDesc.MinLOD = -FLT_MAX;//default
	//sampDesc.MinLOD = FLT_MAX;//default
	//sampDesc.MipLODBias = 0.f;//default
	//sampDesc.MaxAnisotropy = 1;//default
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;// D3D11_COMPARISON_LESS;
	//sampDesc.BorderColor[0] = 1.f;//default
	//sampDesc.BorderColor[1] = 1.f;//default
	//sampDesc.BorderColor[2] = 1.f;//default
	//sampDesc.BorderColor[3] = 1.f;//default

	hr = d3d11Device->CreateSamplerState(&sampDesc, &samplerState);
	if (!CheckError(hr, nullptr)) return false;

	///////////////////////////////////////////////////////////////////////////////
	// Setup the depth stencil buffer and view.
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

ID3D11RenderTargetView* RenderTexture::GetRenderTargetView()
{
	return renderTargetView;
}

ID3D11DepthStencilView* RenderTexture::GetDepthStencilView()
{
	return depthStencilView;
}

D3D11_VIEWPORT RenderTexture::GetViewport()
{
	return viewport;
}