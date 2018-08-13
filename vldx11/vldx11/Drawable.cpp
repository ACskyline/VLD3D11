#include "Drawable.h"

Drawable::Drawable(DrawableType _type) : vertexBuffer(nullptr), indexBuffer(nullptr),
objectUniformBuffer(nullptr), pObject(nullptr), type(_type)
{
}


Drawable::~Drawable()
{
	if (vertexBuffer!=nullptr) vertexBuffer->Release();
	if (indexBuffer != nullptr) indexBuffer->Release();
	if (objectUniformBuffer != nullptr) objectUniformBuffer->Release();
}

bool Drawable::CreateBuffer(ID3D11Device* d3d11Device, Mesh* mesh)
{
	HRESULT hr;

	//Vertex Buffer Start
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mesh->vertexNum;
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
	indexBufferDesc.ByteWidth = sizeof(DWORD) * mesh->indexNum;
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

void Drawable::VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh)
{
	d3d11DevCon->UpdateSubresource(vertexBuffer, 0, NULL, mesh->vertices, 0, 0);
	d3d11DevCon->UpdateSubresource(indexBuffer, 0, NULL, mesh->indices, 0, 0);
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

void Drawable::Draw(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh, Material* mat)
{
	if (type == DrawableType::TrianlgeList)
		DrawTriangleList(d3d11DevCon, mesh, mat);
	else if (type == DrawableType::LineList)
		DrawLineList(d3d11DevCon, mesh, mat);
}

void Drawable::DrawTriangleList(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh, Material* mat)
{
	mat->SetShader(d3d11DevCon);
	mat->SetLayoutTriangleList(d3d11DevCon);
	SetVertexIndexBuffer(d3d11DevCon);
	SetObjectUniformBufferVSPS(d3d11DevCon);
	d3d11DevCon->DrawIndexed(mesh->indexNum, 0, 0);
}

void Drawable::DrawLineList(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh, Material* mat)
{
	mat->SetShader(d3d11DevCon);
	mat->SetLayoutLineList(d3d11DevCon);
	SetVertexIndexBuffer(d3d11DevCon);
	SetObjectUniformBufferVSPS(d3d11DevCon);
	d3d11DevCon->DrawIndexed(mesh->indexNum, 0, 0);
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

void Drawable::SetObject(Object* _pObject)
{
	pObject = _pObject;
}

Object* Drawable::GetObject()
{
	return pObject;
}