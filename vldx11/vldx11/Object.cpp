#include "Object.h"



Object::Object() : 
	pDrawable(nullptr),
	pCamera(nullptr),
	pCameraShadow(nullptr),
	pLight(nullptr),
	pTransform(nullptr),
	pTransformShadow(nullptr)
{
}

Object::~Object()
{
}

//update object and set needToUpdate flag
void Object::UpdateObject()
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
}

void Object::SetCamera(Camera *_pCamera)
{
	pCamera = _pCamera;
}

void Object::SetCameraShadow(Camera *_pCamera)
{
	pCameraShadow = _pCamera;
}

void Object::SetLight(Light *_pLight)
{
	pLight = _pLight;
}

void Object::SetTransform(Transform *_pTransform)
{
	pTransform = _pTransform;
}

void Object::SetTransformShadow(Transform *_pTransform)
{
	pTransformShadow = _pTransform;
}

void Object::ConnectObjectUniformWrite(ObjectUniform *_pObjectUniform)
{
	pObjectUniform = _pObjectUniform;
}

void Object::ConnectFrameUniformWrite(FrameUniform *_pFrameUniform)
{
	pFrameUniform = _pFrameUniform;
}

void Object::ConnectSceneUniformWrite(SceneUniform *_pSceneUniform)
{
	pSceneUniform = _pSceneUniform;
}
