#include "Camera.h"

Camera::Camera(XMFLOAT3 _pos, XMFLOAT3 _target, XMFLOAT3 _up,
	float _fovDegree, float _aspect, float _nearClipPlane, float _farClipPlane)
	: pos(_pos), target(_target), up(_up), 
	fovDegree(_fovDegree), aspect(_aspect), nearClipPlane(_nearClipPlane), farClipPlane(_farClipPlane),
	frameUniformBuffer(nullptr)
{
}

Camera::~Camera()
{
	if (frameUniformBuffer!=nullptr) frameUniformBuffer->Release();
}

bool Camera::CreateBuffer(ID3D11Device* d3d11Device)
{
	HRESULT hr;

	//PerFrame Constant Buffer Start
	D3D11_BUFFER_DESC frameBufferDesc;
	ZeroMemory(&frameBufferDesc, sizeof(D3D11_BUFFER_DESC));
	frameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	frameBufferDesc.ByteWidth = sizeof(FrameUniform);
	frameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	frameBufferDesc.CPUAccessFlags = 0;
	frameBufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&frameBufferDesc, NULL, &frameUniformBuffer);
	if (!CheckError(hr, nullptr)) return false;
	//PerFrame Constant Buffer Finish

	return true;
}

void Camera::FrameUniformBufferData(ID3D11DeviceContext* d3d11DevCon, FrameUniform* uni)
{
	d3d11DevCon->UpdateSubresource(frameUniformBuffer, 0, NULL, uni, 0, 0);
}

void Camera::SetFrameUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(FRAME_UNIFORM_SOLT, 1, &frameUniformBuffer);
}

void Camera::SetFrameUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(FRAME_UNIFORM_SOLT, 1, &frameUniformBuffer);
}

void Camera::SetFrameUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetFrameUniformBufferVS(d3d11DevCon);
	SetFrameUniformBufferPS(d3d11DevCon);
}

void Camera::SetVP(XMFLOAT4X4* VP)
{
	XMStoreFloat4x4(VP, XMMatrixLookAtLH(XMLoadFloat3(&pos),
										 XMLoadFloat3(&target),
										 XMLoadFloat3(&up)) *
						XMMatrixPerspectiveFovLH(fovDegree,
												 aspect,
												 nearClipPlane,
												 farClipPlane));
}

void Camera::SetVP_INV(XMFLOAT4X4* VP, XMFLOAT4X4* VP_INV)
{
	XMMATRIX temp = XMMatrixLookAtLH(XMLoadFloat3(&pos),
									 XMLoadFloat3(&target),
									 XMLoadFloat3(&up)) *
					XMMatrixPerspectiveFovLH(fovDegree,
											 aspect,
											 nearClipPlane,
											 farClipPlane);

	XMStoreFloat4x4(VP, temp);
	XMStoreFloat4x4(VP_INV, XMMatrixInverse(nullptr, temp));
}