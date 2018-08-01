#pragma once
#include "GlobalInclude.h"

class Material
{
public:
	Material(string _vsName, string _psName, D3D11_INPUT_ELEMENT_DESC _vertLayoutDesc[], uint8_t _vertLayoutDescSize);
	~Material();

	bool CreateShader(ID3D11Device* d3d11Device);
	bool CreateLayout(ID3D11Device* d3d11Device);
	void SetShader(ID3D11DeviceContext* d3d11DevCon);
	void SetLayout(ID3D11DeviceContext* d3d11DevCon);

private:
	string vsName;
	string psName;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3DBlob* VS_Buffer;
	ID3DBlob* PS_Buffer; 
	ID3D11InputLayout* vertLayout;
	D3D11_INPUT_ELEMENT_DESC* vertLayoutDesc;
	uint8_t vertLayoutDescSize;
};

