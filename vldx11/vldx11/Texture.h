#pragma once

#include "GlobalInclude.h"

//Currently, texture with same source file but different settings are duplicated on GPU side.
//Ideally, they should share the same data on GPU side but with different settings.

class Texture
{
public:
	enum TextureType { Default };

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
	enum RenderTextureType { Default, ShadowMap, WithDepthStencil };

	RenderTexture(RenderTextureType _typeRT, int _width, int _height);
	~RenderTexture();

	bool InitTexture(ID3D11Device* d3d11Device);
	bool HasRenderTargetView();
	bool HasDepthStencilView();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	
private:
	int width;
	int height;
	RenderTextureType typeRT;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;

	bool InitRenderTextureDefault(ID3D11Device* d3d11Device);
	bool InitRenderTextureShadowMap(ID3D11Device* d3d11Device);
	bool InitRenderTextureWithDepthStencil(ID3D11Device* d3d11Device);
};