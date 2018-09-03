#include "Material.h"

Material::Material(wstring _vsName, 
				   wstring _psName, 
				   D3D11_INPUT_ELEMENT_DESC _vertLayoutDesc[], 
				   uint8_t _vertLayoutDescSize) : 
	vsName(_vsName), 
	psName(_psName), 
	vertLayoutDescSize(_vertLayoutDescSize), 
	initiated(false)
{
	vertLayoutDesc = new D3D11_INPUT_ELEMENT_DESC[vertLayoutDescSize];
	for (int i = 0; i < vertLayoutDescSize; i++)
	{
		vertLayoutDesc[i] = _vertLayoutDesc[i];
	}

	for (uint32_t i = 0; i < MAX_TEXTURE_SLOT; i++)
	{
		pTex[i] = nullptr;
	}
}

Material::~Material()
{
	MY_DELETE_ARRAY(vertLayoutDesc);
	if (VS != nullptr) VS->Release();
	if (PS != nullptr) PS->Release();
	if (VS_Buffer != nullptr) VS_Buffer->Release();
	if (PS_Buffer != nullptr) PS_Buffer->Release();
	if (vertLayout != nullptr) vertLayout->Release();
}

bool Material::CreateShader(ID3D11Device* d3d11Device)
{
	HRESULT hr;
	ID3DBlob* error = nullptr;

	hr = D3DCompileFromFile(vsName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", 0, 0, &VS_Buffer, &error);//entry point is the function name
	if (!CheckError(hr, error)) return false;

	hr = D3DCompileFromFile(psName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_4_0", 0, 0, &PS_Buffer, &error);//entry point is the function name
	if (!CheckError(hr, error)) return false;

	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	if (!CheckError(hr, nullptr)) return false;

	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	if (!CheckError(hr, nullptr)) return false;

	return true;
}

bool Material::CreateLayout(ID3D11Device* d3d11Device)
{
	HRESULT hr;

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout(vertLayoutDesc, vertLayoutDescSize, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertLayout);

	if (!CheckError(hr, nullptr)) return false;

	return true;
}

void Material::SetShader(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);
}

void Material::SetLayoutTriangleList(ID3D11DeviceContext* d3d11DevCon)
{
	//Set the Input Layout
	d3d11DevCon->IASetInputLayout(vertLayout);
	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Material::SetLayoutLineList(ID3D11DeviceContext* d3d11DevCon)
{
	//Set the Input Layout
	d3d11DevCon->IASetInputLayout(vertLayout);
	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

}

bool Material::InitMaterial(ID3D11Device* d3d11Device)
{
	if (!CreateShader(d3d11Device)) return false;
	printf("material create buffer done!\n");
	if (!CreateLayout(d3d11Device)) return false;
	printf("material create layout done!\n");

	for (uint32_t i = 0; i < MAX_TEXTURE_SLOT; i++)
	{
		if (HasTexture(i))
		{
			if (!pTex[i]->IsInitiated())
			{
				if (!pTex[i]->InitTexture(d3d11Device))
					return false;
			}
		}
	}

	initiated = true;
	return true;
}

bool Material::IsInitiated()
{
	return initiated;
}

void Material::SetTexture(uint32_t slot, Texture* _pTex)
{
	pTex[slot] = _pTex;
}

Texture* Material::GetTexture(uint32_t slot)
{
	return pTex[slot];
}

bool Material::HasTexture(uint32_t slot)
{
	return pTex[slot] != nullptr;
}

void Material::UseTexture(uint32_t slot, ID3D11DeviceContext* d3d11DevCon)
{
	pTex[slot]->UseTextureData(slot, d3d11DevCon);//temporary, only use slot 0
}
