#pragma once
#include "GlobalInclude.h"
#include "Light.h"
#include "Camera.h"
#include "Transform.h"

class SceneUniform
{
	struct SceneUniformData
	{
		XMFLOAT4 lightCol;
		XMFLOAT3 lightPos;
		uint32_t step;
		XMFLOAT3 lightDir;
		float farClip;
		XMFLOAT3 volumeCol;
		float lightRadius;
		XMFLOAT3 volumeNoiseSize;
		float lightAngle;
		XMFLOAT3 volumeNoiseVel;
		float volumeG;
		XMFLOAT2 screenSize;
		float volumeNoiseScale;
		float timeScale;
		float nearClip;
		uint32_t PAD1;//padding
		uint32_t PAD2;//padding
		uint32_t PAD3;//padding
		SceneUniformData() :
			lightCol(XMFLOAT4(1, 1, 1, 1)),
			lightPos(XMFLOAT3(0, 0, 0)),
			lightDir(XMFLOAT3(0, 0, 0)),
			volumeCol(XMFLOAT3(1, 1, 1)),
			volumeNoiseSize(XMFLOAT3(1, 1, 1)),
			volumeNoiseVel(XMFLOAT3(0, 0, 0)),
			screenSize(XMFLOAT2(0, 0)),
			step(0),
			nearClip(0.f),
			farClip(0.f),
			lightRadius(0.f),
			lightAngle(0.f),
			volumeG(0.f),
			volumeNoiseScale(0.f),
			timeScale(0)
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
	void ApplyLight(Light* pLight, Transform* pTransform = nullptr);
	void ApplyCamera(Camera* pCamera, Transform* pTransform = nullptr);
	void ApplyStep(uint32_t step);
	void ApplyVolumeCol(XMFLOAT3 _volumeCol);
	void ApplyVolumeG(float _volumeG);
	void ApplyVolumeNoiseScale(float _volumeNoiseScale);
	void ApplyVolumeNoiseSize(XMFLOAT3 _volumeNoiseSize);
	void ApplyVolumeNoiseVel(XMFLOAT3 _volumeNoiseVel);
	void ApplyTimeScale(float _timeScale);
	void ApplyScreenSize(int width, int height);
	int GetStep();
	float GetVolumeG();
	float GetVolumeNoiseScale();
	XMFLOAT3 GetVolumeCol();
	XMFLOAT3 GetVolumeNoiseSize();
	XMFLOAT3 GetVolumeNoiseVel();
	float GetTimeScale();
	bool InitSceneUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool NeedToUpload();

private:
	bool needToUpload;
	SceneUniformData sceneUniformData;
	ID3D11Buffer* sceneUniformBuffer;
};

