#include "Camera.h"

Camera::Camera(CameraType _type, XMFLOAT3 _position, XMFLOAT3 _target, XMFLOAT3 _up,
	float _fovDegree, float _aspect, float _nearClipPlane, float _farClipPlane, float _width, float _height)
	: type(_type), position(_position), target(_target), up(_up), 
	fovDegree(_fovDegree), aspect(_aspect), nearClipPlane(_nearClipPlane), farClipPlane(_farClipPlane), width(_width), height(_height)
{
}

Camera::~Camera()
{
}

void Camera::UpdatePosition()
{
}

XMMATRIX Camera::GetProjectionMatrix()
{
	XMMATRIX result;
	if (type == Perspective)
	{
		result = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovDegree),
										  aspect,
										  nearClipPlane,
										  farClipPlane);
	}
	if (type == Orthographic)
	{
		result = XMMatrixOrthographicLH(width, height, nearClipPlane, farClipPlane);
	}
	return result;
}

XMMATRIX Camera::GetViewMatrix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&position),
							XMLoadFloat3(&target),
							XMLoadFloat3(&up));
}

//////////////////////////////////////////////////////////////////////

OrbitCamera::OrbitCamera(CameraType _type, float _distance, float _horizontalAngle, float _verticalAngle,
	XMFLOAT3 _target, XMFLOAT3 _up,	float _fovDegree, float _aspect, 
	float _nearClipPlane, float _farClipPlane, float _width, float _height) : 
	Camera(_type, XMFLOAT3(0,0,0), _target, _up, _fovDegree, _aspect, _nearClipPlane, _farClipPlane, _width, _height),
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