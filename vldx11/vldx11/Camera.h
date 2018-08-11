#pragma once
#include "GlobalInclude.h"

class Camera
{
public:
	Camera(XMFLOAT3 _pos, XMFLOAT3 _target, XMFLOAT3 _up, 
		float _fovDegree, float _aspect, float _nearClipPlane, float _farClipPlane);
	virtual ~Camera();

	XMFLOAT3 position;
	XMFLOAT3 target;
	XMFLOAT3 up;
	float fovDegree;
	float aspect;
	float nearClipPlane;
	float farClipPlane;

	virtual void UpdatePosition();
};

