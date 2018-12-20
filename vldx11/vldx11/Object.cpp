#include "Object.h"



Object::Object() : 
	pDrawable(nullptr),
	pCamera(nullptr),
	pCameraShadow(nullptr),
	pLight(nullptr),
	pTransform(nullptr),
	pTransformShadow(nullptr),
	initiated(false)
{
}

Object::~Object()
{
}

bool Object::InitObject(ID3D11Device *d3d11Device, ID3D11DeviceContext *d3d11DevCon)
{
	if (!initiated)
	{
		if (pObjectUniform != nullptr&&pTransform != nullptr)
		{
			pObjectUniform->ApplyTransform(pTransform);
		}
		if (pFrameUniform != nullptr && pCamera != nullptr)
		{
			pFrameUniform->ApplyCamera(pCamera, pTransform);
		}
		if (pFrameUniform != nullptr && pCameraShadow != nullptr)
		{
			pFrameUniform->ApplyCameraShadow(pCameraShadow, pTransformShadow);
		}
		if (pLight != nullptr&&pSceneUniform != nullptr)
		{
			pSceneUniform->ApplyLight(pLight, pTransform);
		}
		if (pCamera != nullptr&&pSceneUniform != nullptr)
		{
			pSceneUniform->ApplyCamera(pCamera, pTransform);
		}
		if (pDrawable != nullptr)
		{
			if(!pDrawable->InitDrawable(d3d11Device, d3d11DevCon)) return false;
		}
		initiated = true;
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
	if (pFrameUniform != nullptr && pCamera!=nullptr)//pTransform could be nullptr
	{
		//wait and upload in UpdateScene()
		pFrameUniform->ApplyCamera(pCamera, pTransform);
	}
	if (pFrameUniform != nullptr && pCameraShadow != nullptr)//pTransformShadow could be nullptr
	{
		//wait and upload in UpdateScene()
		pFrameUniform->ApplyCameraShadow(pCameraShadow, pTransformShadow);
	}
	if (pSceneUniform != nullptr && (pCamera != nullptr || pLight != nullptr))
	{
		//wait and upload in UpdateScene()
		if(pCamera != nullptr) pSceneUniform->ApplyCamera(pCamera, pTransform);
		if(pLight != nullptr) pSceneUniform->ApplyLight(pLight, pTransform);
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
	initiated = false;
}

void Object::SetCamera(Camera *_pCamera)
{
	pCamera = _pCamera;
	initiated = false;
}

void Object::SetCameraShadow(Camera *_pCamera)
{
	pCameraShadow = _pCamera;
	initiated = false;
}

void Object::SetLight(Light *_pLight)
{
	pLight = _pLight;
	initiated = false;
}

void Object::SetTransform(Transform *_pTransform)
{
	pTransform = _pTransform;
	initiated = false;
}

void Object::SetTransformShadow(Transform *_pTransform)
{
	pTransformShadow = _pTransform;
	initiated = false;
}

void Object::ConnectObjectUniformWrite(ObjectUniform *_pObjectUniform)
{
	pObjectUniform = _pObjectUniform;
	initiated = false;
}

void Object::ConnectFrameUniformWrite(FrameUniform *_pFrameUniform)
{
	pFrameUniform = _pFrameUniform;
	initiated = false;
}

void Object::ConnectSceneUniformWrite(SceneUniform *_pSceneUniform)
{
	pSceneUniform = _pSceneUniform;
	initiated = false;
}
