#pragma once

#include "GlobalInclude.h"

//Currently, texture with same source file but different settings are duplicated on GPU side.
//Ideally, they should share the same data on GPU side but with different settings.

class Texture
{
public:
	enum TextureType { Albedo };

	Texture(TextureType _type, wstring _fileName);
	~Texture();

	bool InitTexture(ID3D11Device* d3d11Device);
	void UseTextureData(uint32_t slot, ID3D11DeviceContext* d3d11DevCon);
	bool IsInitiated();

private:
	TextureType type;
	wstring fileName;
	bool initiated;

	ID3D11Resource* textureRes;
	ID3D11ShaderResourceView* textureResView;
	ID3D11SamplerState* samplerState;

	void Debug();
};

