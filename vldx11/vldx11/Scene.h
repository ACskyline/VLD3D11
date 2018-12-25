#pragma once

#include "GlobalInclude.h"
#include "DrawableGroup.h"
#include "Object.h"

//Will be used in the future

class Scene
{
public:
	Scene();
	~Scene();

	bool InitScene(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	void UpdateSceneAll(ID3D11DeviceContext* d3d11DevCon);
	void UpdateSceneCheck(ID3D11DeviceContext* d3d11DevCon);

	void AddDrawableGroup(DrawableGroup* pDrawableGroup);
	void AddDrawable(Drawable* pDrawable);
	void AddObjectUniform(ObjectUniform* pObjectUniform);
	void AddFrameUniform(FrameUniform* pFrameUniform);
	void AddSceneUniform(SceneUniform* pSceneUniform);
	void AddMesh(Mesh* pMesh);
	void AddMaterial(Material* pMaterial);
	void AddTexture(Texture* pTexture);
	void AddObject(Object* pObject);

private:
	vector<DrawableGroup*> pDrawableGroupVec;
	vector<Drawable*> pDrawableVec;
	vector<ObjectUniform*> pObjectUniformVec;
	vector<FrameUniform*> pFrameUniformVec;
	vector<SceneUniform*> pSceneUniformVec;
	vector<Mesh*> pMeshVec;
	vector<Material*> pMaterialVec;
	vector<Texture*> pTextureVec;
	vector<Object*> pObjectVec;
};
