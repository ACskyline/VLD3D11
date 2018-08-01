#pragma once
#include "GlobalInclude.h"

class Camera
{
public:
	Camera(XMFLOAT3 _pos, XMFLOAT3 _target, XMFLOAT3 _up, 
		float _fovDegree, float _aspect, float _nearClipPlane, float _farClipPlane);
	~Camera();

	bool CreateBuffer(ID3D11Device* d3d11Device);
	void FrameUniformBufferData(ID3D11DeviceContext* d3d11DevCon, FrameUniform* uni);
	bool SetBuffer(ID3D11DeviceContext* d3d11DevCon);
	void SetVP(XMFLOAT4X4* VP);

	XMFLOAT3 pos;
	XMFLOAT3 target;
	XMFLOAT3 up;
	float fovDegree;
	float aspect;
	float nearClipPlane;
	float farClipPlane;

private:
	ID3D11Buffer * frameUniformBuffer;
};

