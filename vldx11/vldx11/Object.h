#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Drawable.h"
#include "Light.h"
#include "Transform.h"
#include "FrameUniform.h"
#include "SceneUniform.h"

class Drawable;

class Object
{
public:
	Object();
	~Object();

	bool InitObject(ID3D11Device *d3d11Device, ID3D11DeviceContext *d3d11DevCon);
	void UpdateObject(ID3D11DeviceContext *d3d11DevCon);
	void DrawObject(ID3D11DeviceContext *d3d11DevCon);

	void SetMesh(Mesh *_pMesh);
	void SetMaterial(Material *_pMaterial);
	void SetDrawable(Drawable *_pDrawable);
	void SetCamera(Camera *_pCamera);
	void SetLight(Light *_pLight);
	void SetTransform(Transform *_pTransform);
	void ConnectFrameUniform(FrameUniform *_pFrameUniform);
	void ConnectSceneUniform(SceneUniform *_pSceneUniform);

private:
	Mesh* pMesh;
	Material* pMaterial;
	Drawable* pDrawable;
	Camera* pCamera;
	Light* pLight;
	Transform* pTransform;
	FrameUniform* pFrameUniform;
	SceneUniform* pSceneUniform;

};

