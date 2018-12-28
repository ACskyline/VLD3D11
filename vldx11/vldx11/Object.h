#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Drawable.h"
#include "Light.h"
#include "Transform.h"
#include "ObjectUniform.h"
#include "FrameUniform.h"
#include "SceneUniform.h"

class Object
{
public:
	Object();
	~Object();

	void UpdateObject();
	void DrawObject(ID3D11DeviceContext *d3d11DevCon);

	void SetDrawable(Drawable *_pDrawable);
	void SetCamera(Camera *_pCamera);
	void SetCameraShadow(Camera *_pCameraShadow);
	void SetLight(Light *_pLight);
	void SetTransform(Transform *_pTransform);
	void SetTransformShadow(Transform *_pTransform);
	void ConnectObjectUniformWrite(ObjectUniform *_pObjectUniform);
	void ConnectFrameUniformWrite(FrameUniform *_pFrameUniform);
	void ConnectSceneUniformWrite(SceneUniform *_pSceneUniform);
	bool NeedToUpdate();
	void SetNeedToUpdate(bool _needToUpdate);

private:
	Drawable* pDrawable;
	Camera* pCamera;
	Camera* pCameraShadow;
	Light* pLight;
	Transform* pTransform;
	Transform* pTransformShadow;
	ObjectUniform* pObjectUniform;
	FrameUniform* pFrameUniform;
	SceneUniform* pSceneUniform;
	bool needToUpdate;
};

