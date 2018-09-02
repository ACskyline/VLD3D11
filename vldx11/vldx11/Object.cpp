#include "Object.h"



Object::Object() : pDrawable(nullptr),
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
	if (pDrawable != nullptr)
	{
		if (!pDrawable->IsInitiated()) pDrawable->InitDrawable(d3d11Device, d3d11DevCon);
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
	if (pDrawable != nullptr)
	{
		pDrawable->Draw(d3d11DevCon);
	}
}

void Object::SetDrawable(Drawable *_pDrawable)
{
	pDrawable = _pDrawable;
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
