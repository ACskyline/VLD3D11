#pragma once

#include "GlobalInclude.h"

class Light
{
public:
	enum LightType { Point, Directional, Spot };
	virtual ~Light();
	//the functions below are not virtual, so that sub-class who does not define
	//these can return the same value defined in these functions.
	virtual XMFLOAT4 GetCol();
	virtual XMFLOAT3 GetPos();
	virtual XMFLOAT3 GetDir();
	virtual float GetRadius();
	virtual float GetAngle();
	virtual void SetCol(XMFLOAT4 _col);
	virtual void SetPos(XMFLOAT3 _pos);
	virtual void SetDir(XMFLOAT3 _dir);
	virtual void SetRadius(float _radius);
	virtual void SetAngle(float _angle);

protected:
	Light(LightType _type, XMFLOAT4 _col);

private:
	LightType type;
	XMFLOAT4 col;
};

class PointLight : public Light
{
public:
	PointLight(XMFLOAT4 _col, XMFLOAT3 _pos, float _radius);
	~PointLight();
	XMFLOAT3 GetPos();
	float GetRadius();
	void SetPos(XMFLOAT3 _pos);
	void SetRadius(float _radius);

private:
	XMFLOAT3 pos;
	float radius;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(XMFLOAT4 _col, XMFLOAT3 _dir);
	~DirectionalLight();
	XMFLOAT3 GetDir();
	void SetDir(XMFLOAT3 _dir);

private:
	XMFLOAT3 dir;
};

class SpotLight : public Light
{
public:
	SpotLight(XMFLOAT4 _col, XMFLOAT3 _pos, XMFLOAT3 _dir, float _radius, float _angle);
	~SpotLight();
	XMFLOAT3 GetPos();
	XMFLOAT3 GetDir();
	float GetRadius();
	float GetAngle();
	void SetPos(XMFLOAT3 _pos);
	void SetDir(XMFLOAT3 _dir);
	void SetRadius(float _radius);
	void SetAngle(float _angle);

private:
	XMFLOAT3 pos;
	XMFLOAT3 dir;
	float radius;
	float angle;
};