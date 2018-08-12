#include "DrawableGroup.h"



DrawableGroup::DrawableGroup()
{
}


DrawableGroup::~DrawableGroup()
{
}

void DrawableGroup::AddDrawable(Drawable* pDrawable)
{
	drawableVector.push_back(pDrawable);
}

void DrawableGroup::RemoveDrawable(Drawable* pDrawable)
{
	vector<Drawable*>::iterator it = find(drawableVector.begin(), drawableVector.end(), pDrawable);
	if (it != drawableVector.end())
		drawableVector.erase(it);
}

void DrawableGroup::Draw(ID3D11DeviceContext* d3d11DevCon)
{
	for(auto item = drawableVector.begin();item!=drawableVector.end();item++)
	{
		(*item)->GetObject()->DrawObject(d3d11DevCon);
	}
}