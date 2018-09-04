#pragma once
#include "GlobalInclude.h"
#include "Camera.h"

class FrameUniform
{
	struct FrameUniformData
	{
		XMFLOAT4X4 VP;
		XMFLOAT4X4 VP_INV;
		XMFLOAT4 COL;
		XMFLOAT3 cameraPos;
		float intensity;
		uint32_t frameNum;
		uint32_t PAD1;//padding
		uint32_t PAD2;//padding
		uint32_t PAD3;//padding
		FrameUniformData() :
			VP(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			VP_INV(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			COL(XMFLOAT4(1, 1, 1, 1)),
			cameraPos(XMFLOAT3(0, 0, 0)),
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
	void ApplyCamera(Camera* pCamera);
	void ApplyCol(float r, float g, float b, float a);
	void ApplyIntensity(float intensity);
	void ApplyFrameNum(uint32_t frameNum);
	bool InitFrameUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool NeedToUpload();

private:
	bool needToUpload;
	FrameUniformData frameUniformData;
	ID3D11Buffer * frameUniformBuffer;
	void SetVP(Camera* pCamera);
	void SetVP_INV(Camera* pCamera);
};

