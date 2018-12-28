#include "SceneUniform.h"



SceneUniform::SceneUniform() : sceneUniformBuffer(nullptr), needToUpload(false)
{
}


SceneUniform::~SceneUniform()
{
	if (sceneUniformBuffer != nullptr) sceneUniformBuffer->Release();
}

bool SceneUniform::CreateBuffer(ID3D11Device* d3d11Device)
{
	HRESULT hr;

	//PerScene Constant Buffer Start
	D3D11_BUFFER_DESC sceneBufferDesc;
	ZeroMemory(&sceneBufferDesc, sizeof(D3D11_BUFFER_DESC));
	sceneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	sceneBufferDesc.ByteWidth = sizeof(SceneUniformData);
	sceneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sceneBufferDesc.CPUAccessFlags = 0;
	sceneBufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&sceneBufferDesc, NULL, &sceneUniformBuffer);
	if (!CheckError(hr, nullptr)) return false;
	//PerScene Constant Buffer Finish

	return true;
}

void SceneUniform::SceneUniformBufferData(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->UpdateSubresource(sceneUniformBuffer, 0, NULL, &sceneUniformData, 0, 0);
	needToUpload = false;
}

void SceneUniform::SetSceneUniformBufferVS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->VSSetConstantBuffers(UNIFORM_SLOT::SCENE, 1, &sceneUniformBuffer);
}

void SceneUniform::SetSceneUniformBufferPS(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->PSSetConstantBuffers(UNIFORM_SLOT::SCENE, 1, &sceneUniformBuffer);
}

void SceneUniform::SetSceneUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon)
{
	SetSceneUniformBufferVS(d3d11DevCon);
	SetSceneUniformBufferPS(d3d11DevCon);
}

void SceneUniform::ApplyLight(Light* pLight, Transform* pTransform)
{
	XMMATRIX m(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	if (pTransform != nullptr) m = pTransform->GetTransform();

	//light color
	sceneUniformData.lightCol = pLight->GetCol();

	//light position
	XMFLOAT3 lightPosVec3 = pLight->GetPos();
	XMFLOAT4 lightPosVec4(lightPosVec3.x, lightPosVec3.y, lightPosVec3.z, 1);//w is 1
	XMStoreFloat3(&sceneUniformData.lightPos, XMVector4Transform(XMLoadFloat4(&lightPosVec4), m));
	
	//light direction
	XMFLOAT3 lightDirVec3 = pLight->GetDir();
	XMFLOAT4 lightDirVec4(lightDirVec3.x, lightDirVec3.y, lightDirVec3.z, 0);//w is 0
	XMStoreFloat3(&sceneUniformData.lightDir, XMVector4Normalize(XMVector4Transform(XMLoadFloat4(&lightDirVec4), m)));//normalize in shader

	//light radius
	sceneUniformData.lightRadius = pLight->GetRadius();

	//light angle
	sceneUniformData.lightAngle = pLight->GetAngle();

	needToUpload = true;
}

void SceneUniform::ApplyCamera(Camera* pCamera, Transform* pTransform)
{
	sceneUniformData.nearClip = pCamera->nearClipPlane;
	sceneUniformData.farClip = pCamera->farClipPlane;
	needToUpload = true;
}

void SceneUniform::ApplyStep(uint32_t step)
{
	sceneUniformData.step = step;
	needToUpload = true;
}

void SceneUniform::ApplyVolumeCol(XMFLOAT3 _volumeCol)
{
	sceneUniformData.volumeCol = _volumeCol;
	needToUpload = true;
}

void SceneUniform::ApplyVolumeG(float _volumeG)
{
	sceneUniformData.volumeG = _volumeG;
	needToUpload = true;
}

void SceneUniform::ApplyVolumeNoiseScale(float _volumeNoiseScale)
{
	sceneUniformData.volumeNoiseScale = _volumeNoiseScale;
	needToUpload = true;
}

void SceneUniform::ApplyVolumeNoiseSize(XMFLOAT3 _volumeNoiseSize)
{
	sceneUniformData.volumeNoiseSize = _volumeNoiseSize;
	needToUpload = true;
}

void SceneUniform::ApplyVolumeNoiseVel(XMFLOAT3 _volumeNoiseVel)
{
	sceneUniformData.volumeNoiseVel = _volumeNoiseVel;
	needToUpload = true;
}

void SceneUniform::ApplyTimeScale(float _timeScale)
{
	sceneUniformData.timeScale = _timeScale;
	needToUpload = true;
}

void SceneUniform::ApplyScreenSize(int width, int height)
{
	sceneUniformData.screenSize = XMFLOAT2(width, height);
	needToUpload = true;
}

int SceneUniform::GetStep()
{
	return sceneUniformData.step;
}

float SceneUniform::GetVolumeG()
{
	return sceneUniformData.volumeG;
}

float SceneUniform::GetVolumeNoiseScale()
{
	return sceneUniformData.volumeNoiseScale;
}

XMFLOAT3 SceneUniform::GetVolumeCol()
{
	return sceneUniformData.volumeCol;
}

XMFLOAT3 SceneUniform::GetVolumeNoiseSize()
{
	return sceneUniformData.volumeNoiseSize;
}

XMFLOAT3 SceneUniform::GetVolumeNoiseVel()
{
	return sceneUniformData.volumeNoiseVel;
}

float SceneUniform::GetTimeScale()
{
	return sceneUniformData.timeScale;
}

bool SceneUniform::InitSceneUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon)
{
	if (!CreateBuffer(d3d11Device)) return false;
	SceneUniformBufferData(d3d11DevCon);
	SetSceneUniformBufferVSPS(d3d11DevCon);
	printf("frameUniform create buffer done!\n");
	return true;
}

bool SceneUniform::NeedToUpload()
{
	return needToUpload;
}