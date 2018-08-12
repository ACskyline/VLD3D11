#pragma once

#include "GlobalInclude.h"
#include "Drawable.h"

class DrawableGroup
{
public:
	DrawableGroup();
	~DrawableGroup();

	void AddDrawable(Drawable* pDrawable);
	void RemoveDrawable(Drawable* pDrawable);
	void Draw(ID3D11DeviceContext* d3d11DevCon);

private:
	vector<Drawable*> drawableVector;
};

