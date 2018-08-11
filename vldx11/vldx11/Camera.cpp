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