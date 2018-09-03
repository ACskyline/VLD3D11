#include "Camera.h"

Camera::Camera(XMFLOAT3 _position, XMFLOAT3 _target, XMFLOAT3 _up,
	float _fovDegree, float _aspect, float _nearClipPlane, float _farClipPlane)
	: position(_position), target(_target), up(_up), 
	fovDegree(_fovDegree), aspect(_aspect), nearClipPlane(_nearClipPlane), farClipPlane(_farClipPlane)
{
}

Camera::~Camera()
{
}

void Camera::UpdatePosition()
{
}

//////////////////////////////////////////////////////////////////////

OrbitCamera::OrbitCamera(float _distance, float _horizontalAngle, float _verticalAngle,
	XMFLOAT3 _target, XMFLOAT3 _up,	float _fovDegree, float _aspect, 
	float _nearClipPlane, float _farClipPlane) : 
	Camera(XMFLOAT3(0,0,0), _target, _up, _fovDegree, _aspect, _nearClipPlane, _farClipPlane),
	distance(_distance), horizontalAngle(_horizontalAngle), verticalAngle(_verticalAngle)
	
{
	UpdatePosition();
}


OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::UpdatePosition()
{
	float y = distance * XMScalarSin(XMConvertToRadians(verticalAngle));
	float x = distance * XMScalarCos(XMConvertToRadians(verticalAngle)) * XMScalarSin(XMConvertToRadians(horizontalAngle));
	float z = -distance * XMScalarCos(XMConvertToRadians(verticalAngle)) * XMScalarCos(XMConvertToRadians(horizontalAngle));

	position = XMFLOAT3(target.x + x, target.y + y, target.z + z);
}