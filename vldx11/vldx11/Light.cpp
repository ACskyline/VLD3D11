#include "Light.h"

Light::Light(LightType _type, XMFLOAT4 _col) :
	type(_type), col(_col)
{
}

Light::~Light()
{
}

XMFLOAT4 Light::GetCol()
{
	return col;
}

XMFLOAT3 Light::GetPos()
{
	return XMFLOAT3(0, 0, 0);
}

XMFLOAT3 Light::GetDir()
{
	return XMFLOAT3(0, 0, 0);
}

float Light::GetRadius()
{
	return 0;
}

float Light::GetAngle()
{
	return 0;
}

void Light::SetCol(XMFLOAT4 _col)
{
	col = _col;
}

void Light::SetPos(XMFLOAT3 _pos)
{
	//do nothing
}

void Light::SetDir(XMFLOAT3 _dir)
{
	//do nothing
}

void Light::SetRadius(float _radius)
{
	//do nothing
}

void Light::SetAngle(float _angle)
{
	//do nothing
}

///////////////////////////////////////////////////////////////////////////

PointLight::PointLight(XMFLOAT4 _col, XMFLOAT3 _pos, float _radius) : 
	Light(Light::LightType::Point, _col), pos(_pos), radius(_radius)
{
}

PointLight::~PointLight()
{
}

XMFLOAT3 PointLight::GetPos()
{
	return pos;
}

float PointLight::GetRadius()
{
	return radius;
}

void PointLight::SetPos(XMFLOAT3 _pos)
{
	pos = _pos;
}

void PointLight::SetRadius(float _radius)
{
	radius = _radius;
}

///////////////////////////////////////////////////////////////////////////

DirectionalLight::DirectionalLight(XMFLOAT4 _col, XMFLOAT3 _dir) :
	Light(Light::LightType::Directional, _col), dir(_dir)
{
}

DirectionalLight::~DirectionalLight()
{
}

XMFLOAT3 DirectionalLight::GetDir()
{
	return dir;
}

void DirectionalLight::SetDir(XMFLOAT3 _dir)
{
	dir = _dir;
}


///////////////////////////////////////////////////////////////////////////

SpotLight::SpotLight(XMFLOAT4 _col, XMFLOAT3 _pos, XMFLOAT3 _dir, float _radius, float _angle) :
	Light(Light::LightType::Spot, _col), 
	pos(_pos),
	dir(_dir), 
	radius(_radius), 
	angle(_angle)
{
}

SpotLight::~SpotLight()
{
}

XMFLOAT3 SpotLight::GetPos()
{
	return pos;
}

XMFLOAT3 SpotLight::GetDir()
{
	return dir;
}

float SpotLight::GetRadius()
{
	return radius;
}

float SpotLight::GetAngle()
{
	return angle;
}

void SpotLight::SetPos(XMFLOAT3 _pos)
{
	pos = _pos;
}

void SpotLight::SetDir(XMFLOAT3 _dir)
{
	dir = _dir;
}

void SpotLight::SetRadius(float _radius)
{
	radius = _radius;
}

void SpotLight::SetAngle(float _angle)
{
	angle = _angle;
}