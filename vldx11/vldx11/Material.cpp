#include "Material.h"

Material::Material(string _vsName, string _psName, D3D11_INPUT_ELEMENT_DESC _vertLayoutDesc[], uint8_t _vertLayoutDescSize) : vsName(_vsName), psName(_psName), vertLayoutDescSize(_vertLayoutDescSize)
{
	vertLayoutDesc = new D3D11_INPUT_ELEMENT_DESC[vertLayoutDescSize];
	for (int i = 0; i < vertLayoutDescSize; i++)
	{
		vertLayoutDesc[i] = _vertLayoutDesc[i];
	}
}

Material::~Material()
{
	MY_DELETE_ARRAY(vertLayoutDesc);
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
}

bool Material::CreateShader(ID3D11Device* d3d11Device)
{
	HRESULT hr;
	ID3DBlob* error = nullptr;

	hr = D3DCompileFromFile(L"myVert.hlsl", 0, 0, "main", "vs_4_0", 0, 0, &VS_Buffer, &error);//entry point is the function name
	if (!CheckError(hr, error)) return false;

	hr = D3DCompileFromFile(L"myPixel.hlsl", 0, 0, "main", "ps_4_0", 0, 0, &PS_Buffer, &error);//entry point is the function name
	if (!CheckError(hr, error)) return false;

	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	if (!CheckError(hr, nullptr)) return false;

	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	if (!CheckError(hr, nullptr)) return false;
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

void Material::SetLayout(ID3D11DeviceContext* d3d11DevCon)
{
	//Set the Input Layout
	d3d11DevCon->IASetInputLayout(vertLayout);
	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}