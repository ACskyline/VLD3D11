#pragma once

#include "GlobalInclude.h"

class Transform
{
public:
	Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale);
	~Transform();

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
};

