#include "Transform.h"

Transform::Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale) :
	position(_position), rotation(_rotation), scale(_scale)
{
}


Transform::~Transform()
{
}

void Transform::SetM(XMFLOAT4X4* M)
{
	XMStoreFloat4x4(M, XMMatrixScaling(scale.x, scale.y, scale.z) * 
					   XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * 
					   XMMatrixTranslation(position.x, position.y, position.z));
}