#pragma once

#include "GlobalInclude.h"

class Scene
{
public:
	Scene();
	~Scene();

	bool CreateBuffer(ID3D11Device* d3d11Device);
	void SceneUniformBufferData(ID3D11DeviceContext* d3d11DevCon, SceneUniform* uni);
	void SetSceneUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetSceneUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetSceneUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);

private:

	ID3D11Buffer* sceneUniformBuffer;
};

