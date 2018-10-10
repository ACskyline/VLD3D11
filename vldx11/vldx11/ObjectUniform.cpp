#include "ObjectUniform.h"



ObjectUniform::ObjectUniform() : objectUniformBuffer(nullptr), needToUpload(false), initiated(false)
{
}


ObjectUniform::~ObjectUniform()
{
	if (objectUniformBuffer != nullptr) objectUniformBuffer->Release();
}

bool ObjectUniform::CreateBuffer(ID3D11Device* d3d11Device)
{
	HRESULT hr;

	//PerObject Constant Buffer Start
	D3D11_BUFFER_DESC objectBufferDesc;
	ZeroMemory(&objectBufferDesc, sizeof(D3D11_BUFFER_DESC));
	objectBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	objectBufferDesc.ByteWidth = sizeof(ObjectUniformData);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = 0;
	objectBufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&objectBufferDesc, NULL, &objectUniformBuffer);
	if (!CheckError(hr, nullptr)) return false;
	//PerObject Constant Buffer Finish

	return true;
}

void ObjectUniform::ObjectUniformBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(objectUniformBuffer, 0, NULL, &objectUniformData, 0, 0);
	needToUpload = false;
}

void ObjectUniform::SetObjectUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(OBJECT_UNIFORM_SOLT, 1, &objectUniformBuffer);
}

void ObjectUniform::SetObjectUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(OBJECT_UNIFORM_SOLT, 1, &objectUniformBuffer);
}

void ObjectUniform::SetObjectUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetObjectUniformBufferVS(d3d11DevCon);
	SetObjectUniformBufferPS(d3d11DevCon);
}

void ObjectUniform::SetM(Transform* pTransform)
{
	XMMATRIX temp = pTransform->GetTransform();

	XMStoreFloat4x4(&objectUniformData.M, temp);
}

void ObjectUniform::SetM_INV(Transform* pTransform)
{
	XMMATRIX temp = pTransform->GetTransform();

	XMStoreFloat4x4(&objectUniformData.M, temp);
	XMStoreFloat4x4(&objectUniformData.M_INV, XMMatrixInverse(nullptr, temp));
}

void ObjectUniform::SetM_INV_TRANS(Transform* pTransform)
{
	XMMATRIX temp = pTransform->GetTransform();

	XMStoreFloat4x4(&objectUniformData.M, temp);
	XMStoreFloat4x4(&objectUniformData.M_INV, XMMatrixInverse(nullptr, temp));
	XMStoreFloat4x4(&objectUniformData.M_INV_TRANS, XMMatrixTranspose(XMMatrixInverse(nullptr, temp)));
}

void ObjectUniform::ApplyTransform(Transform* pTransform)
{
	SetM_INV_TRANS(pTransform);
	needToUpload = true;
}

void ObjectUniform::ApplyCol(float r, float g, float b, float a)
{
	objectUniformData.COL = XMFLOAT4(r, g, b, a);
	needToUpload = true;
}

bool ObjectUniform::InitObjectUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon)
{
	if (!initiated)
	{
		if (!CreateBuffer(d3d11Device)) return false;
		ObjectUniformBufferData(d3d11DevCon);
		SetObjectUniformBufferVSPS(d3d11DevCon);
		printf("objectUniform create buffer done!\n");
		initiated = true;
	}
	return true;
}

bool ObjectUniform::NeedToUpload()
{
	return needToUpload;
}