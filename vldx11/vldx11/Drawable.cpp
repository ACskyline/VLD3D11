#include "Drawable.h"

Drawable::Drawable(DrawableType _type, Mesh* _pMesh, Material* _pMat) : 
	pMesh(_pMesh), pMat(_pMat), initiated(false),
	vertexBuffer(nullptr), indexBuffer(nullptr), type(_type)
{
}


Drawable::~Drawable()
{
	if (vertexBuffer!=nullptr) vertexBuffer->Release();
	if (indexBuffer != nullptr) indexBuffer->Release();
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

	return true;
}

void Drawable::VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(vertexBuffer, 0, NULL, pMesh->vertices, 0, 0);
	d3d11DevCon->UpdateSubresource(indexBuffer, 0, NULL, pMesh->indices, 0, 0);
}

void Drawable::SetVertexIndexBuffer(ID3D11DeviceContext* d3d11DevCon)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Drawable::Draw(ID3D11DeviceContext* d3d11DevCon)
{
	//set shader first
	pMat->SetShader(d3d11DevCon);

	if (pSceneUniform != nullptr) pSceneUniform->SetSceneUniformBufferVSPS(d3d11DevCon);
	if (pFrameUniform != nullptr) pFrameUniform->SetFrameUniformBufferVSPS(d3d11DevCon);
	if (pObjectUniform != nullptr) pObjectUniform->SetObjectUniformBufferVSPS(d3d11DevCon);

	SetVertexIndexBuffer(d3d11DevCon);

	for (uint32_t i = 0; i < MAX_TEXTURE_SLOT; i++)
	{
		if (pMat->HasTexture(i))
		{
			pMat->UseTexture(i, d3d11DevCon);
		}
	}

	if (type == DrawableType::TrianlgeList)
		UseTriangleList(d3d11DevCon);
	else if (type == DrawableType::LineList)
		UseLineList(d3d11DevCon);

	d3d11DevCon->DrawIndexed(pMesh->indexNum, 0, 0);
}

void Drawable::Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat)
{
	//set shader first
	_pMat->SetShader(d3d11DevCon);

	if (pSceneUniform != nullptr) pSceneUniform->SetSceneUniformBufferVSPS(d3d11DevCon);
	if (pFrameUniform != nullptr) pFrameUniform->SetFrameUniformBufferVSPS(d3d11DevCon);
	if (pObjectUniform != nullptr) pObjectUniform->SetObjectUniformBufferVSPS(d3d11DevCon);

	SetVertexIndexBuffer(d3d11DevCon);

	for (uint32_t i = 0; i < MAX_TEXTURE_SLOT; i++)
	{
		if (_pMat->HasTexture(i))
		{
			_pMat->UseTexture(i, d3d11DevCon);
		}
	}

	if (type == DrawableType::TrianlgeList)
		UseTriangleList(d3d11DevCon);
	else if (type == DrawableType::LineList)
		UseLineList(d3d11DevCon);

	d3d11DevCon->DrawIndexed(pMesh->indexNum, 0, 0);
}

void Drawable::Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat, FrameUniform* _pFrameUniform)
{
	//set shader first
	_pMat->SetShader(d3d11DevCon);

	if (pSceneUniform != nullptr) pSceneUniform->SetSceneUniformBufferVSPS(d3d11DevCon);
	if (_pFrameUniform != nullptr) _pFrameUniform->SetFrameUniformBufferVSPS(d3d11DevCon);
	if (pObjectUniform != nullptr) pObjectUniform->SetObjectUniformBufferVSPS(d3d11DevCon);

	SetVertexIndexBuffer(d3d11DevCon);

	for (uint32_t i = 0; i < MAX_TEXTURE_SLOT; i++)
	{
		if (_pMat->HasTexture(i))
		{
			_pMat->UseTexture(i, d3d11DevCon);
		}
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


bool Drawable::InitDrawable(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon)
{
	if (!initiated)
	{
		if (!pMesh->InitMesh()) return false;
		if (!pMat->InitMaterial(d3d11Device)) return false;
		if (!CreateBuffer(d3d11Device)) return false;
		VertexIndexBufferData(d3d11DevCon);
		printf("drawble create buffer done!\n");
		initiated = true;
	}
	return true;
}

bool Drawable::IsInitiated()
{
	return initiated;
}

void Drawable::ConnectObjectUniformRead(ObjectUniform *_pObjectUniform)
{
	pObjectUniform = _pObjectUniform;
}

void Drawable::ConnectFrameUniformRead(FrameUniform *_pFrameUniform)
{
	pFrameUniform = _pFrameUniform;
}

void Drawable::ConnectSceneUniformRead(SceneUniform *_pSceneUniform)
{
	pSceneUniform = _pSceneUniform;
}
