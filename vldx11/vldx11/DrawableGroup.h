#pragma once

#include "GlobalInclude.h"
#include "Drawable.h"

class DrawableGroup
{
public:
	enum DrawableGroupType { VolumeLight, Gizmo, Standard, Debug };

	DrawableGroup(DrawableGroupType _type);
	~DrawableGroup();

	bool InitDrawableGroup(ID3D11Device* d3d11Device);
	void AddDrawable(Drawable* pDrawable);
	void RemoveDrawable(Drawable* pDrawable);
	void Draw(ID3D11DeviceContext* d3d11DevCon);
	void Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat);
	void Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat, FrameUniform* _pFrameUniform);
	bool IsInitiated();
	bool IsEnabled();
	void SetEnabled(bool _enabled);

private:
	bool initiated;
	bool enabled;
	DrawableGroupType type;
	vector<Drawable*> drawableVector;
	ID3D11RasterizerState* d3d11RasterizerState;
	ID3D11DepthStencilState* d3d11DepthStencilState;
	ID3D11BlendState* d3d11BlendState;
};
