#pragma once

#include "GlobalInclude.h"
#include "Drawable.h"

class DrawableGroup
{
public:
	enum DrawableGroupType { VolumeLight, Gizmo, Standard, UI };

	DrawableGroup(DrawableGroupType _type);
	~DrawableGroup();

	bool InitDrawableGroup(ID3D11Device* d3d11Device);
	void AddDrawable(Drawable* pDrawable);
	void RemoveDrawable(Drawable* pDrawable);
	void Draw(ID3D11DeviceContext* d3d11DevCon);
	void Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat);
	bool isInitiated();

private:
	bool initiated;
	DrawableGroupType type;
	vector<Drawable*> drawableVector;
	ID3D11RasterizerState* d3d11RasterizerState;
	ID3D11DepthStencilState* d3d11DepthStencilState;
};

