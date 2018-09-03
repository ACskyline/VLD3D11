#include "Texture.h"

Texture::Texture(TextureType _type, wstring _fileName) :
	type(_type), fileName(_fileName), initiated(false)
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
	//hr = CreateWICTextureFromFile(d3d11Device, fileName.c_str(), (ID3D11Resource**)(&(textureRes)), &textureResView);
	hr = CreateWICTextureFromFileEx(d3d11Device, 
									fileName.c_str(), 
									0, 
									D3D11_USAGE_DEFAULT, 
									D3D11_BIND_SHADER_RESOURCE, 
									0, 
									0, 
									WIC_LOADER_IGNORE_SRGB, 
									(ID3D11Resource**)(&(textureRes)), 
									&textureResView);
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

	initiated = true;
	return true;
}

void Texture::UseTextureData(uint32_t slot, ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetShaderResources(slot, 1, &textureResView);
	d3d11DevCon->PSSetSamplers(slot, 1, &samplerState);
}

bool Texture::IsInitiated()
{
	return initiated;
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