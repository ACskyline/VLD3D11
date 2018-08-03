#pragma once

#include "GlobalInclude.h"

class Transform
{
public:
	Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale);
	~Transform();

	void SetM(XMFLOAT4X4* M);
	void SetM_INV(XMFLOAT4X4* M, XMFLOAT4X4* M_INV);

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
};

