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
		pDrawable->ApplyTransform(pTransform);
	}
	if (pCamera != nullptr&&pFrameUniform != nullptr)
	{
		pFrameUniform->ApplyCamera(pCamera);
	}
	if (pLight != nullptr&&pSceneUniform != nullptr)
	{
		pSceneUniform->ApplyLight(pLight);
	}
	if (pCamera != nullptr&&pSceneUniform != nullptr)
	{
		pSceneUniform->ApplyCamera(pCamera);
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
		pDrawable->ApplyTransform(pTransform);
		pDrawable->ObjectUniformBufferData(d3d11DevCon);
	}
	if (pCamera!=nullptr && pFrameUniform != nullptr )
	{
		pFrameUniform->ApplyCamera(pCamera);
		pFrameUniform->FrameUniformBufferData(d3d11DevCon);
	}
	if (pSceneUniform != nullptr && (pCamera != nullptr || pLight != nullptr))
	{
		if(pCamera != nullptr) pSceneUniform->ApplyCamera(pCamera);
		if(pLight != nullptr) pSceneUniform->ApplyLight(pLight);
		pSceneUniform->SceneUniformBufferData(d3d11DevCon);
	}
}

void Object::DrawObject(ID3D11DeviceContext *d3d11DevCon)
{
	if (pDrawable != nullptr && pMesh != nullptr && pMaterial != nullptr)
	{
		pDrawable->Draw(d3d11DevCon, pMesh, pMaterial);
	}
}

void Object::SetMesh(Mesh *_pMesh)
{
	pMesh = _pMesh;
}

void Object::SetMaterial(Material *_pMaterial)
{
	pMaterial = _pMaterial;
}

void Object::SetDrawable(Drawable *_pDrawable)
{
	pDrawable = _pDrawable;
	pDrawable->SetObject(this);
}

void Object::SetCamera(Camera *_pCamera)
{
	pCamera = _pCamera;
}

void Object::SetLight(Light *_pLight)
{
	pLight = _pLight;
}

void Object::SetTransform(Transform *_pTransform)
{
	pTransform = _pTransform;
}

void Object::ConnectFrameUniform(FrameUniform *_pFrameUniform)
{
	pFrameUniform = _pFrameUniform;
}

void Object::ConnectSceneUniform(SceneUniform *_pSceneUniform)
{
	pSceneUniform = _pSceneUniform;
}
