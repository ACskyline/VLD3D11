#include "FrameUniform.h"



FrameUniform::FrameUniform() : 
	frameUniformBuffer(nullptr), 
	needToUpload(false)
{
}


FrameUniform::~FrameUniform()
{
	if (frameUniformBuffer != nullptr) frameUniformBuffer->Release();
}

bool FrameUniform::CreateBuffer(ID3D11Device* d3d11Device)
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
}

void FrameUniform::FrameUniformBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(frameUniformBuffer, 0, NULL, &frameUniformData, 0, 0);
	needToUpload = false;
}

void FrameUniform::SetFrameUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(UNIFORM_SLOT::FRAME, 1, &frameUniformBuffer);
}

void FrameUniform::SetFrameUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(UNIFORM_SLOT::FRAME, 1, &frameUniformBuffer);
}

void FrameUniform::SetFrameUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetFrameUniformBufferVS(d3d11DevCon);
	SetFrameUniformBufferPS(d3d11DevCon);
}

void FrameUniform::SetV_P_VP_INV(Camera* pCamera, const XMMATRIX& transform)
{
	XMMATRIX tempV = XMMatrixInverse(nullptr, transform) * pCamera->GetViewMatrix();

	XMMATRIX tempP = pCamera->GetProjectionMatrix();

	XMMATRIX temp = tempV * tempP;

	XMStoreFloat4x4(&frameUniformData.V, tempV);
	XMStoreFloat4x4(&frameUniformData.V_INV, XMMatrixInverse(nullptr, tempV));
	XMStoreFloat4x4(&frameUniformData.P, tempP);
	XMStoreFloat4x4(&frameUniformData.P_INV, XMMatrixInverse(nullptr, tempP));
	XMStoreFloat4x4(&frameUniformData.VP, temp);
	XMStoreFloat4x4(&frameUniformData.VP_INV, XMMatrixInverse(nullptr, temp));
}

void FrameUniform::SetV_P_VP_INV_Shadow(Camera* pCamera, const XMMATRIX& transform)
{
	XMMATRIX tempV = XMMatrixInverse(nullptr, transform) * pCamera->GetViewMatrix();

	XMMATRIX tempP = pCamera->GetProjectionMatrix();

	XMMATRIX temp = tempV * tempP;

	XMStoreFloat4x4(&frameUniformData.V_SHADOW, tempV);
	XMStoreFloat4x4(&frameUniformData.V_SHADOW_INV, XMMatrixInverse(nullptr, tempV));
	XMStoreFloat4x4(&frameUniformData.P_SHADOW, tempP);
	XMStoreFloat4x4(&frameUniformData.P_SHADOW_INV, XMMatrixInverse(nullptr, tempP));
	XMStoreFloat4x4(&frameUniformData.VP_SHADOW, temp);
	XMStoreFloat4x4(&frameUniformData.VP_INV_SHADOW, XMMatrixInverse(nullptr, temp));
}

void FrameUniform::ApplyCamera(Camera* pCamera, Transform* pTransform)
{
	XMMATRIX m(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	if (pTransform != nullptr) m = pTransform->GetTransform();

	//camera position
	XMFLOAT4 cameraPosVec4(pCamera->position.x, pCamera->position.y, pCamera->position.z, 1);//w is 1
	XMStoreFloat3(&frameUniformData.cameraPos, XMVector4Transform(XMLoadFloat4(&cameraPosVec4), m));

	//view matrix and projection matrix
	SetV_P_VP_INV(pCamera, m);

	needToUpload = true;
}

void FrameUniform::ApplyCameraShadow(Camera* pCamera, Transform* pTransform)
{
	XMMATRIX m(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	if (pTransform != nullptr) m = pTransform->GetTransform();

	//camera position
	XMFLOAT4 cameraPosVec4(pCamera->position.x, pCamera->position.y, pCamera->position.z, 1);//w is 1
	XMStoreFloat3(&frameUniformData.cameraPos_SHADOW, XMVector4Transform(XMLoadFloat4(&cameraPosVec4), m));

	//view matrix and projection matrix
	SetV_P_VP_INV_Shadow(pCamera, m);

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

void FrameUniform::ApplyTextureSize(int width, int height)
{
	frameUniformData.textureSize = XMFLOAT2(width, height);
	needToUpload = true;
}

float FrameUniform::GetIntensity()
{
	return frameUniformData.intensity;
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