#pragma once

#include "GlobalInclude.h"

class Light
{
public:
	enum LightType { Point, Directional };
	virtual ~Light();
	virtual XMFLOAT4 GetCol();
	virtual XMFLOAT3 GetPos();
	virtual XMFLOAT3 GetDir();
	virtual float GetRadius();
	virtual void SetCol(XMFLOAT4 _col);
	virtual void SetPos(XMFLOAT3 _pos);
	virtual void SetDir(XMFLOAT3 _dir);
	virtual void SetRadius(float _radius);

protected:
	Light(LightType _type, XMFLOAT4 _col);

private:
	LightType type;
	XMFLOAT4 col;
};

class PointLight : public Light
{
public:
	PointLight(XMFLOAT3 _pos, XMFLOAT4 _col, float _radius);
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
	DirectionalLight(XMFLOAT3 _dir, XMFLOAT4 _col);
	~DirectionalLight();
	XMFLOAT3 GetDir();
	void SetDir(XMFLOAT3 _dir);

private:
	XMFLOAT3 dir;
};