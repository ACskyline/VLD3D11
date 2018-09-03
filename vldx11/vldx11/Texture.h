#pragma once

#include "GlobalInclude.h"

//Currently, texture with same source file but different settings are duplicated on GPU side.
//Ideally, they should share the same data on GPU side but with different settings.

class Texture
{
public:
	enum TextureType { Default, Albedo };

	Texture(TextureType _type, wstring _fileName);
	~Texture();

	virtual bool InitTexture(ID3D11Device* d3d11Device);
	void UseTextureData(uint32_t slot, ID3D11DeviceContext* d3d11DevCon);
	bool IsInitiated();

protected:
	Texture(TextureType _type);
	bool initiated;
	ID3D11Texture2D* textureRes;
	ID3D11ShaderResourceView* textureResView;
	ID3D11SamplerState* samplerState;

private:
	TextureType type;
	wstring fileName;

	void Debug();
};

class RenderTexture : public Texture
{
public:
	enum RenderTextureType { ShadowMap };

	RenderTexture(RenderTextureType _typeRT);
	~RenderTexture();

	bool InitTexture(ID3D11Device* d3d11Device);
	
private:
	RenderTextureType typeRT;
	ID3D11RenderTargetView* renderTargetView;
};