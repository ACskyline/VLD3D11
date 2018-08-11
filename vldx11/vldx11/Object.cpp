#include "Object.h"



Object::Object() : pMesh(nullptr), pMaterial(nullptr), pDrawable(nullptr),
				   pCamera(nullptr), pLight(nullptr), pTransform(nullptr)
{
}

Object::~Object()
{
}

bool Object::InitObject(ID3D11Device *d3d11Device, ID3D11DeviceContext *d3d11DevCon)
{
	if (pDrawable != nullptr&&pTransform != nullptr)
	{
		pDrawable->SetTransform(pTransform);
	}
	if (pCamera != nullptr&&pFrameUniform != nullptr)
	{
		pFrameUniform->SetCamera(pCamera);
	}
	if (pLight != nullptr&&pSceneUniform != nullptr)
	{
		pSceneUniform->SetLight(pLight);
	}
	if (pCamera != nullptr&&pSceneUniform != nullptr)
	{
		pSceneUniform->SetCamera(pCamera);
	}
	if (pMaterial != nullptr)
	{
		if (!pMaterial->CreateShader(d3d11Device)) return false;
		printf("material create buffer done!\n");
		if (!pMaterial->CreateLayout(d3d11Device)) return false;
		printf("material create layout done!\n");
	}
	if (pDrawable != nullptr&&pMesh != nullptr)
	{
		if (!pDrawable->CreateBuffer(d3d11Device, pMesh)) return false;
		pDrawable->VertexIndexBufferData(d3d11DevCon, pMesh);
		pDrawable->ObjectUniformBufferData(d3d11DevCon);
		pDrawable->SetObjectUniformBufferVSPS(d3d11DevCon);
		printf("drawble create buffer done!\n");
	}

	return true;
}

void Object::UpdateObject(ID3D11DeviceContext *d3d11DevCon)
{
	if (pCamera != nullptr)
	{
		pCamera->UpdatePosition();//update position first because of orbit camera
	}
	if (pTransform != nullptr && pDrawable != nullptr)
	{
		pDrawable->SetTransform(pTransform);
		pDrawable->ObjectUniformBufferData(d3d11DevCon);
	}
	if (pCamera!=nullptr && pFrameUniform != nullptr )
	{
		pFrameUniform->SetCamera(pCamera);
		pFrameUniform->FrameUniformBufferData(d3d11DevCon);
	}
	if (pSceneUniform != nullptr && (pCamera != nullptr || pLight != nullptr))
	{
		if(pCamera != nullptr) pSceneUniform->SetCamera(pCamera);
		if(pLight != nullptr) pSceneUniform->SetLight(pLight);
		pSceneUniform->SceneUniformBufferData(d3d11DevCon);
	}
}

void Object::DrawObjectTriangleList(ID3D11DeviceContext *d3d11DevCon)
{
	if (pDrawable != nullptr && pMesh != nullptr && pMaterial != nullptr)
	{
		pDrawable->DrawTriangleList(d3d11DevCon, pMesh, pMaterial);
	}
}

void Object::DrawObjectLineList(ID3D11DeviceContext *d3d11DevCon)
{
	if (pDrawable != nullptr && pMesh != nullptr && pMaterial != nullptr)
	{
		pDrawable->DrawLineList(d3d11DevCon, pMesh, pMaterial);
	}
}