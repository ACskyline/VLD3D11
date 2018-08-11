#pragma once
#include "Camera.h"

class OrbitCamera :
	public Camera
{
public:
	OrbitCamera(float _distance, float _horizontalAngle, float _verticalAngle,
		XMFLOAT3 _target, XMFLOAT3 _up, float _fovDegree, float _aspect,
		float _nearClipPlane, float _farClipPlane);
	virtual ~OrbitCamera();

	float distance;
	float horizontalAngle;
	float verticalAngle;

	void UpdatePosition();
};

