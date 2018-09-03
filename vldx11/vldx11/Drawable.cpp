#include "Drawable.h"

Drawable::Drawable(DrawableType _type, Mesh* _pMesh, Material* _pMat) : 
	pMesh(_pMesh), pMat(_pMat), initiated(false),
	vertexBuffer(nullptr), indexBuffer(nullptr),
	objectUniformBuffer(nullptr), type(_type)
{
}


Drawable::~Drawable()
{
	if (vertexBuffer!=nullptr) vertexBuffer->Release();
	if (indexBuffer != nullptr) indexBuffer->Release();
	if (objectUniformBuffer != nullptr) objectUniformBuffer->Release();
}

bool Drawable::CreateBuffer(ID3D11Device* d3d11Device)
{
	HRESULT hr;

	//Vertex Buffer Start
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * pMesh->vertexNum;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, NULL, &vertexBuffer);//
	if (!CheckError(hr, nullptr)) return false;
	//Vertex Buffer Finish

	//Index Buffer Start
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * pMesh->indexNum;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&indexBufferDesc, NULL, &indexBuffer);//
	if (!CheckError(hr, nullptr)) return false;
	//Index Buffer Finish

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

void Drawable::VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(vertexBuffer, 0, NULL, pMesh->vertices, 0, 0);
	d3d11DevCon->UpdateSubresource(indexBuffer, 0, NULL, pMesh->indices, 0, 0);
}

void Drawable::ObjectUniformBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(objectUniformBuffer, 0, NULL, &objectUniformData, 0, 0);
}

void Drawable::SetVertexIndexBuffer(ID3D11DeviceContext* d3d11DevCon)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Drawable::SetObjectUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(OBJECT_UNIFORM_SOLT, 1, &objectUniformBuffer);
}

void Drawable::SetObjectUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(OBJECT_UNIFORM_SOLT, 1, &objectUniformBuffer);
}

void Drawable::SetObjectUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetObjectUniformBufferVS(d3d11DevCon);
	SetObjectUniformBufferPS(d3d11DevCon);
}

void Drawable::Draw(ID3D11DeviceContext* d3d11DevCon)
{
	//set shader first
	pMat->SetShader(d3d11DevCon);

	SetVertexIndexBuffer(d3d11DevCon);
	SetObjectUniformBufferVSPS(d3d11DevCon);

	for (uint32_t i = 0; i < MAX_TEXTURE_SLOT; i++)
	{
		if (pMat->HasTexture(i)) pMat->UseTexture(i, d3d11DevCon);
	}

	if (type == DrawableType::TrianlgeList)
		UseTriangleList(d3d11DevCon);
	else if (type == DrawableType::LineList)
		UseLineList(d3d11DevCon);

	d3d11DevCon->DrawIndexed(pMesh->indexNum, 0, 0);
}

void Drawable::UseTriangleList(ID3D11DeviceContext* d3d11DevCon)
{
	pMat->SetLayoutTriangleList(d3d11DevCon);
}

void Drawable::UseLineList(ID3D11DeviceContext* d3d11DevCon)
{
	pMat->SetLayoutLineList(d3d11DevCon);
}

void Drawable::SetM(Transform* pTransform)
{
	XMStoreFloat4x4(&objectUniformData.M, XMMatrixScaling(pTransform->scale.x, pTransform->scale.y, pTransform->scale.z) *
					   XMMatrixRotationRollPitchYaw(pTransform->rotation.x, pTransform->rotation.y, pTransform->rotation.z) *
					   XMMatrixTranslation(pTransform->position.x, pTransform->position.y, pTransform->position.z));
}

void Drawable::SetM_INV(Transform* pTransform)
{
	XMMATRIX temp = XMMatrixScaling(pTransform->scale.x, pTransform->scale.y, pTransform->scale.z) *
					XMMatrixRotationRollPitchYaw(pTransform->rotation.x, pTransform->rotation.y, pTransform->rotation.z) *
					XMMatrixTranslation(pTransform->position.x, pTransform->position.y, pTransform->position.z);

	XMStoreFloat4x4(&objectUniformData.M, temp);
	XMStoreFloat4x4(&objectUniformData.M_INV, XMMatrixInverse(nullptr, temp));
}

void Drawable::ApplyTransform(Transform* pTransform)
{
	SetM_INV(pTransform);
}

void Drawable::ApplyColor(float r, float g, float b, float a)
{
	objectUniformData.COL = XMFLOAT4(r, g, b, a);
}

bool Drawable::InitDrawable(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon)
{
	if (!pMesh->IsInitiated())
	{
		if (!pMesh->InitMesh()) return false;
	}
	if (!pMat->IsInitiated())
	{
		if (!pMat->InitMaterial(d3d11Device)) return false;
	}
	if (!CreateBuffer(d3d11Device)) return false;
	VertexIndexBufferData(d3d11DevCon);
	ObjectUniformBufferData(d3d11DevCon);
	SetObjectUniformBufferVSPS(d3d11DevCon);
	printf("drawble create buffer done!\n");
	initiated = true;
	return true;
}

bool Drawable::IsInitiated()
{
	return initiated;
}