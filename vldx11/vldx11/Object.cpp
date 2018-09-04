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
	if (pObjectUniform != nullptr&&pTransform != nullptr)
	{
		pObjectUniform->ApplyTransform(pTransform);
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
	if (pObjectUniform != nullptr && pTransform != nullptr)
	{
		//wait and upload in UpdateScene()
		pObjectUniform->ApplyTransform(pTransform);
	}
	if (pFrameUniform != nullptr && pCamera!=nullptr)
	{
		//wait and upload in UpdateScene()
		pFrameUniform->ApplyCamera(pCamera);
	}
	if (pSceneUniform != nullptr && (pCamera != nullptr || pLight != nullptr))
	{
		//wait and upload in UpdateScene()
		if(pCamera != nullptr) pSceneUniform->ApplyCamera(pCamera);
		if(pLight != nullptr) pSceneUniform->ApplyLight(pLight);
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

void Object::ConnectObjectUniform(ObjectUniform *_pObjectUniform)
{
	pObjectUniform = _pObjectUniform;
}

void Object::ConnectFrameUniform(FrameUniform *_pFrameUniform)
{
	pFrameUniform = _pFrameUniform;
}

void Object::ConnectSceneUniform(SceneUniform *_pSceneUniform)
{
	pSceneUniform = _pSceneUniform;
}
