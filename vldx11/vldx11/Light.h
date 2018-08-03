#pragma once

#include "GlobalInclude.h"

class Light
{
public:
	XMFLOAT3 pos;
	XMFLOAT4 col;

	Light(XMFLOAT3 _pos, XMFLOAT4 _col);
	~Light();
};

