#pragma once
#include "GlobalInclude.h"
#include "Light.h"
#include "Camera.h"

class SceneUniform
{
	struct SceneUniformData
	{
		XMFLOAT4 lightCol;
		XMFLOAT3 lightPos;
		uint32_t step;
		XMFLOAT3 lightDir;
		float farClip;
		float lightRadius;
		uint32_t PAD1;
		uint32_t PAD2;
		uint32_t PAD3;
		SceneUniformData() :
			lightCol(XMFLOAT4(1, 1, 1, 1)),
			lightPos(XMFLOAT3(0, 0, 0)),
			lightDir(XMFLOAT3(0, 0, 0)),
			lightRadius(0.f),
			step(0),
			farClip(0.f)
		{}
	};

public:
	SceneUniform();
	~SceneUniform();

	bool CreateBuffer(ID3D11Device* d3d11Device);
	void SceneUniformBufferData(ID3D11DeviceContext* d3d11DevCon);
	void SetSceneUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetSceneUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetSceneUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void ApplyLight(Light* pLight);
	void ApplyCamera(Camera* pCamera);
	void ApplyStep(uint32_t step);
	bool InitSceneUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool NeedToUpload();

private:
	bool needToUpload;
	SceneUniformData sceneUniformData;
	ID3D11Buffer* sceneUniformBuffer;
	bool initiated;
};

