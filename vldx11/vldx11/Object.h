#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Drawable.h"
#include "Light.h"
#include "Transform.h"
#include "FrameUniform.h"
#include "SceneUniform.h"

class Object
{
public:
	Object();
	~Object();

	Mesh* pMesh;
	Material* pMaterial;
	Drawable* pDrawable;
	Camera* pCamera;
	Light* pLight;
	Transform* pTransform;
	FrameUniform* pFrameUniform;
	SceneUniform* pSceneUniform;

	bool InitObject(ID3D11Device *d3d11Device, ID3D11DeviceContext *d3d11DevCon);
	void UpdateObject(ID3D11DeviceContext *d3d11DevCon);
	void DrawObjectTriangleList(ID3D11DeviceContext *d3d11DevCon);
	void DrawObjectLineList(ID3D11DeviceContext *d3d11DevCon);
};

