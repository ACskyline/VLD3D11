#pragma once
#include "GlobalInclude.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

class Drawable
{
	struct ObjectUniformData
	{
		XMFLOAT4X4 M;
		XMFLOAT4X4 M_INV;
		XMFLOAT4 COL;
	};
public:
	Drawable();
	~Drawable();
	bool CreateBuffer(ID3D11Device* d3d11Device, Mesh* mesh);
	void VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh);
	void ObjectUniformBufferData(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void DrawTriangleList(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh, Material* mat);
	void DrawLineList(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh, Material* mat);
	void SetTransform(Transform* pTransform);

	ObjectUniformData objectUniformData;

private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* objectUniformBuffer;
	void SetVertexIndexBuffer(ID3D11DeviceContext* d3d11DevCon);
	void SetM(Transform* pTransform);
	void SetM_INV(Transform* pTransform);
};

