#include "FrameUniform.h"



FrameUniform::FrameUniform() : frameUniformBuffer(nullptr), needToUpload(false), initiated(false)
{
}


FrameUniform::~FrameUniform()
{
	if (frameUniformBuffer != nullptr) frameUniformBuffer->Release();
}

bool FrameUniform::CreateBuffer(ID3D11Device* d3d11Device)
{
	if (!initiated)
	{
		HRESULT hr;

		//PerFrame Constant Buffer Start
		D3D11_BUFFER_DESC frameBufferDesc;
		ZeroMemory(&frameBufferDesc, sizeof(D3D11_BUFFER_DESC));
		frameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		frameBufferDesc.ByteWidth = sizeof(FrameUniformData);
		frameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		frameBufferDesc.CPUAccessFlags = 0;
		frameBufferDesc.MiscFlags = 0;
		hr = d3d11Device->CreateBuffer(&frameBufferDesc, NULL, &frameUniformBuffer);
		if (!CheckError(hr, nullptr)) return false;
		//PerFrame Constant Buffer Finish
		initiated = true;
	}
	return true;
}

void FrameUniform::FrameUniformBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(frameUniformBuffer, 0, NULL, &frameUniformData, 0, 0);
	needToUpload = false;
}

void FrameUniform::SetFrameUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(FRAME_UNIFORM_SOLT, 1, &frameUniformBuffer);
}

void FrameUniform::SetFrameUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(FRAME_UNIFORM_SOLT, 1, &frameUniformBuffer);
}

void FrameUniform::SetFrameUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetFrameUniformBufferVS(d3d11DevCon);
	SetFrameUniformBufferPS(d3d11DevCon);
}

void FrameUniform::SetVP(Camera* pCamera)
{
	XMMATRIX tempV = XMMatrixLookAtLH(XMLoadFloat3(&pCamera->position),
		XMLoadFloat3(&pCamera->target),
		XMLoadFloat3(&pCamera->up));

	XMMATRIX tempP = XMMatrixPerspectiveFovLH(XMConvertToRadians(pCamera->fovDegree),
		pCamera->aspect,
		pCamera->nearClipPlane,
		pCamera->farClipPlane);

	XMMATRIX temp = tempV * tempP;

	XMStoreFloat4x4(&frameUniformData.VP, temp);
}

void FrameUniform::SetVP_INV(Camera* pCamera)
{
	XMMATRIX tempV = XMMatrixLookAtLH(XMLoadFloat3(&pCamera->position),
		XMLoadFloat3(&pCamera->target),
		XMLoadFloat3(&pCamera->up));

	XMMATRIX tempP = XMMatrixPerspectiveFovLH(XMConvertToRadians(pCamera->fovDegree),
			pCamera->aspect,
			pCamera->nearClipPlane,
			pCamera->farClipPlane);

	XMMATRIX temp = tempV * tempP;

	XMStoreFloat4x4(&frameUniformData.VP, temp);
	XMStoreFloat4x4(&frameUniformData.VP_INV, XMMatrixInverse(nullptr, temp));
}

void FrameUniform::SetP_VP_INV(Camera* pCamera)
{
	XMMATRIX tempV = XMMatrixLookAtLH(XMLoadFloat3(&pCamera->position),
		XMLoadFloat3(&pCamera->target),
		XMLoadFloat3(&pCamera->up));

	XMMATRIX tempP = XMMatrixPerspectiveFovLH(XMConvertToRadians(pCamera->fovDegree),
		pCamera->aspect,
		pCamera->nearClipPlane,
		pCamera->farClipPlane);

	XMMATRIX temp = tempV * tempP;

	XMStoreFloat4x4(&frameUniformData.P, tempP);
	XMStoreFloat4x4(&frameUniformData.VP, temp);
	XMStoreFloat4x4(&frameUniformData.VP_INV, XMMatrixInverse(nullptr, temp));
}

void FrameUniform::ApplyCamera(Camera* pCamera)
{
	frameUniformData.cameraPos = pCamera->position;
	SetP_VP_INV(pCamera);
	needToUpload = true;
}

void FrameUniform::ApplyCol(float r, float g, float b, float a)
{
	frameUniformData.COL = XMFLOAT4(r, g, b, a);
	needToUpload = true;
}

void FrameUniform::ApplyIntensity(float intensity)
{
	frameUniformData.intensity = intensity;
	needToUpload = true;
}

void FrameUniform::ApplyFrameNum(uint32_t frameNum)
{
	frameUniformData.frameNum = frameNum;
	needToUpload = true;
}

bool FrameUniform::InitFrameUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon)
{
	if (!CreateBuffer(d3d11Device)) return false;
	FrameUniformBufferData(d3d11DevCon);
	SetFrameUniformBufferVSPS(d3d11DevCon);
	printf("frameUniform create buffer done!\n");
	return true;
}

bool FrameUniform::NeedToUpload()
{
	return needToUpload;
}