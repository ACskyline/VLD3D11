#include "Scene.h"

Scene::Scene() 
{
}

Scene::~Scene()
{
}

bool Scene::InitScene(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon)
{
	//uniform
	for (auto item = pObjectUniformVec.begin(); item != pObjectUniformVec.end(); item++)
	{
		if (!(*item)->InitObjectUniform(d3d11Device, d3d11DevCon)) return false;
	}

	for (auto item = pFrameUniformVec.begin(); item != pFrameUniformVec.end(); item++)
	{
		if (!(*item)->InitFrameUniform(d3d11Device, d3d11DevCon)) return false;
	}

	for (auto item = pSceneUniformVec.begin(); item != pSceneUniformVec.end(); item++)
	{
		if (!(*item)->InitSceneUniform(d3d11Device, d3d11DevCon)) return false;
	}

	printf("init unfiorm - done!\n");

	//mesh
	for (auto item = pMeshVec.begin(); item != pMeshVec.end(); item++)
	{
		if (!(*item)->InitMesh()) return false;
	}

	printf("init mesh - done!\n");

	//material
	for (auto item = pMaterialVec.begin(); item != pMaterialVec.end(); item++)
	{
		if (!(*item)->InitMaterial(d3d11Device)) return false;
	}

	printf("init material - done!\n");

	//texture
	for (auto item = pTextureVec.begin(); item != pTextureVec.end(); item++)
	{
		if (!(*item)->InitTexture(d3d11Device)) return false;
	}

	printf("init texture - done!\n");

	//drawable
	for (auto item = pDrawableVec.begin(); item != pDrawableVec.end(); item++)
	{
		if (!(*item)->InitDrawable(d3d11Device, d3d11DevCon)) return false;
	}

	printf("init drawable - done!\n");

	//drawable group
	for (auto item = pDrawableGroupVec.begin(); item != pDrawableGroupVec.end(); item++)
	{
		if (!(*item)->InitDrawableGroup(d3d11Device)) return false;
	}

	printf("init drawable group - done!\n");

	return true;
}

void Scene::UpdateSceneAll(ID3D11DeviceContext* d3d11DevCon)
{
	//update all objects and set needToUpdate flag on connected uniforms. this is expensive.
	for (auto item = pObjectVec.begin(); item != pObjectVec.end(); item++)
	{
		(*item)->UpdateObject();
	}

	//upload to GPU
	for (auto item = pObjectUniformVec.begin(); item != pObjectUniformVec.end(); item++)
	{
		(*item)->ObjectUniformBufferData(d3d11DevCon);
	}

	//upload to GPU
	for (auto item = pFrameUniformVec.begin(); item != pFrameUniformVec.end(); item++)
	{
		(*item)->FrameUniformBufferData(d3d11DevCon);
	}

	//upload to GPU
	for (auto item = pSceneUniformVec.begin(); item != pSceneUniformVec.end(); item++)
	{
		(*item)->SceneUniformBufferData(d3d11DevCon);
	}
}

void Scene::UpdateSceneCheck(ID3D11DeviceContext* d3d11DevCon)
{
	//update flagged object and set needToUpdate flag on connected uniforms.
	for (auto item = pObjectVec.begin(); item != pObjectVec.end(); item++)
	{
		if((*item)->NeedToUpdate()) (*item)->UpdateObject();
	}

	//upload to GPU
	for (auto item = pObjectUniformVec.begin(); item != pObjectUniformVec.end(); item++)
	{
		if ((*item)->NeedToUpload()) (*item)->ObjectUniformBufferData(d3d11DevCon);
	}

	//upload to GPU
	for (auto item = pFrameUniformVec.begin(); item != pFrameUniformVec.end(); item++)
	{
		if ((*item)->NeedToUpload()) (*item)->FrameUniformBufferData(d3d11DevCon);
	}

	//upload to GPU
	for (auto item = pSceneUniformVec.begin(); item != pSceneUniformVec.end(); item++)
	{
		if ((*item)->NeedToUpload()) (*item)->SceneUniformBufferData(d3d11DevCon);
	}
}

void Scene::AddDrawableGroup(DrawableGroup* pDrawableGroup)
{
	pDrawableGroupVec.push_back(pDrawableGroup);
}

void Scene::AddDrawable(Drawable* pDrawable)
{
	pDrawableVec.push_back(pDrawable);
}

void Scene::AddObjectUniform(ObjectUniform* pObjectUniform)
{
	pObjectUniformVec.push_back(pObjectUniform);
}

void Scene::AddFrameUniform(FrameUniform* pFrameUniform)
{
	pFrameUniformVec.push_back(pFrameUniform);
}

void Scene::AddSceneUniform(SceneUniform* pSceneUniform)
{
	pSceneUniformVec.push_back(pSceneUniform);
}

void Scene::AddMesh(Mesh* pMesh)
{
	pMeshVec.push_back(pMesh);
}

void Scene::AddMaterial(Material* pMaterial)
{
	pMaterialVec.push_back(pMaterial);
}

void Scene::AddTexture(Texture* pTexture)
{
	pTextureVec.push_back(pTexture);
}

void Scene::AddObject(Object* pObject)
{
	pObjectVec.push_back(pObject);
}
