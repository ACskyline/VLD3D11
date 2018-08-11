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
	};

public:
	FrameUniform();
	~FrameUniform();

	bool CreateBuffer(ID3D11Device* d3d11Device);
	void FrameUniformBufferData(ID3D11DeviceContext* d3d11DevCon);
	void SetFrameUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetFrameUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetFrameUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void SetCamera(Camera* pCamera);
	bool InitFrameUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);

	FrameUniformData frameUniformData;

private:
	ID3D11Buffer * frameUniformBuffer;
	void SetVP(Camera* pCamera);
	void SetVP_INV(Camera* pCamera);
};

