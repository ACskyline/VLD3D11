#pragma once
#include "GlobalInclude.h"
#include "Texture.h"

class Material
{
public:
	Material(wstring _vsName, wstring _psName, D3D11_INPUT_ELEMENT_DESC _vertLayoutDesc[], uint8_t _vertLayoutDescSize);
	~Material();

	bool CreateShader(ID3D11Device* d3d11Device);
	bool CreateLayout(ID3D11Device* d3d11Device);
	void SetShader(ID3D11DeviceContext* d3d11DevCon);
	void SetLayoutTriangleList(ID3D11DeviceContext* d3d11DevCon);
	void SetLayoutLineList(ID3D11DeviceContext* d3d11DevCon);

	bool InitMaterial(ID3D11Device* d3d11Device);

	void SetTexture(uint32_t slot, Texture* pTex);
	Texture* GetTexture(uint32_t slot);
	bool HasTexture(uint32_t slot);
	void UseTexture(uint32_t slot, ID3D11DeviceContext* d3d11DevCon);

private:
	wstring vsName;
	wstring psName;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3DBlob* VS_Buffer;
	ID3DBlob* PS_Buffer; 
	ID3D11InputLayout* vertLayout;
	D3D11_INPUT_ELEMENT_DESC* vertLayoutDesc;
	uint8_t vertLayoutDescSize;
	Texture* pTex[TEXTURE_SLOT::COUNT];
};

