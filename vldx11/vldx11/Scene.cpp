#include "Scene.h"



Scene::Scene() : sceneUniformBuffer(nullptr)
{
}


Scene::~Scene()
{
}

bool Scene::CreateBuffer(ID3D11Device* d3d11Device)
{
	HRESULT hr;

	//PerScene Constant Buffer Start
	D3D11_BUFFER_DESC sceneBufferDesc;
	ZeroMemory(&sceneBufferDesc, sizeof(D3D11_BUFFER_DESC));
	sceneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	sceneBufferDesc.ByteWidth = sizeof(SceneUniform);
	sceneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sceneBufferDesc.CPUAccessFlags = 0;
	sceneBufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&sceneBufferDesc, NULL, &sceneUniformBuffer);
	if (!CheckError(hr, nullptr)) return false;
	//PerScene Constant Buffer Finish

	return true;
}

void Scene::SceneUniformBufferData(ID3D11DeviceContext* d3d11DevCon, SceneUniform* uni)
{
	d3d11DevCon->UpdateSubresource(sceneUniformBuffer, 0, NULL, uni, 0, 0);
}

void Scene::SetSceneUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(SCENE_UNIFORM_SOLT, 1, &sceneUniformBuffer);
}

void Scene::SetSceneUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(SCENE_UNIFORM_SOLT, 1, &sceneUniformBuffer);
}

void Scene::SetSceneUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetSceneUniformBufferVS(d3d11DevCon);
	SetSceneUniformBufferPS(d3d11DevCon);
}