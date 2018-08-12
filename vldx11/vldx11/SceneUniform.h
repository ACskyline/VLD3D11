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
		float farClip;
		uint32_t PAD1;//padding
		uint32_t PAD2;//padding
		uint32_t PAD3;//padding
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
	

private:

	SceneUniformData sceneUniformData;
	ID3D11Buffer* sceneUniformBuffer;
};

