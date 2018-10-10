#include "Transform.h"

Transform::Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale) :
	position(_position), rotation(_rotation), scale(_scale)
{
}


Transform::~Transform()
{
}

XMMATRIX Transform::GetTransform()
{
	XMMATRIX tempS = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX tempR = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMMATRIX tempT = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX temp = tempS * tempR * tempT;
	return temp;
}