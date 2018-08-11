#include "Transform.h"

Transform::Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale) :
	position(_position), rotation(_rotation), scale(_scale)
{
}


Transform::~Transform()
{
}
