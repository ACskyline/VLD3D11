#pragma once
#include "GlobalInclude.h"
#include "Camera.h"
#include "Transform.h"

class FrameUniform
{
	struct FrameUniformData
	{
		XMFLOAT4X4 V;
		XMFLOAT4X4 V_INV;
		XMFLOAT4X4 P;
		XMFLOAT4X4 P_INV;
		XMFLOAT4X4 VP;
		XMFLOAT4X4 VP_INV;
		XMFLOAT4X4 V_SHADOW;
		XMFLOAT4X4 V_SHADOW_INV;
		XMFLOAT4X4 P_SHADOW;
		XMFLOAT4X4 P_SHADOW_INV;
		XMFLOAT4X4 VP_SHADOW;
		XMFLOAT4X4 VP_INV_SHADOW;
		XMFLOAT3 cameraPos;
		float intensity;
		XMFLOAT3 cameraPos_SHADOW;
		uint32_t frameNum;
		XMFLOAT2 textureSize;
		uint32_t PAD1;//padding
		uint32_t PAD2;//padding
		//uint32_t PAD3;//padding
		FrameUniformData() :
			P(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			VP(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			VP_INV(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			P_SHADOW(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			VP_SHADOW(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			VP_INV_SHADOW(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			cameraPos(XMFLOAT3(0, 0, 0)),
			cameraPos_SHADOW(XMFLOAT3(0, 0, 0)),
			textureSize(XMFLOAT2(0, 0)),
			intensity(0.f),
			frameNum(0)
		{}
	};

public:
	FrameUniform();
	~FrameUniform();

	bool CreateBuffer(ID3D11Device* d3d11Device);
	void FrameUniformBufferData(ID3D11DeviceContext* d3d11DevCon);
	void SetFrameUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetFrameUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetFrameUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void ApplyCamera(Camera* pCamera, Transform* pTransform = nullptr);
	void ApplyCameraShadow(Camera* pCamera, Transform* pTransform = nullptr);
	void ApplyIntensity(float intensity);
	void ApplyFrameNum(uint32_t frameNum);
	void ApplyTextureSize(int width, int height);
	float GetIntensity();
	XMFLOAT4 GetFrameColor();
	bool InitFrameUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool NeedToUpload();

private:
	bool needToUpload;
	FrameUniformData frameUniformData;
	ID3D11Buffer* frameUniformBuffer;
	void SetV_P_VP_INV(Camera* pCamera, const XMMATRIX& transform);
	void SetV_P_VP_INV_Shadow(Camera* pCamera, const XMMATRIX& transform);
};

