#pragma once
#include "GlobalInclude.h"

class Camera
{
public:
	enum CameraType { Perspective, Orthographic };

	Camera(CameraType _type, XMFLOAT3 _pos, XMFLOAT3 _target, XMFLOAT3 _up, 
		float _fovDegree, float _aspect, float _nearClipPlane, float _farClipPlane, float _width, float _height);
	virtual ~Camera();

	CameraType type;
	XMFLOAT3 position;
	XMFLOAT3 target;
	XMFLOAT3 up;
	float fovDegree;
	float aspect;
	float nearClipPlane;
	float farClipPlane;
	float width;//for orthographic camera
	float height;//for orthographic camera

	virtual void UpdatePosition();
	virtual XMMATRIX GetProjectionMatrix();
	virtual XMMATRIX GetViewMatrix();
};

class OrbitCamera :
	public Camera
{
public:
	OrbitCamera(CameraType _type, float _distance, float _horizontalAngle, float _verticalAngle,
		XMFLOAT3 _target, XMFLOAT3 _up, float _fovDegree, float _aspect,
		float _nearClipPlane, float _farClipPlane, float _width, float _height);
	virtual ~OrbitCamera();

	float distance;
	float horizontalAngle;
	float verticalAngle;

	void UpdatePosition();
};

