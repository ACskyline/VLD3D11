#pragma once
#include "GlobalInclude.h"
#include "Mesh.h"
#include "Material.h"

class Drawable
{
public:
	Drawable();
	~Drawable();
	bool CreateBuffer(ID3D11Device* d3d11Device, Mesh* mesh);
	void VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh);
	void ObjectUniformBufferData(ID3D11DeviceContext* d3d11DevCon, ObjectUniform* uni);
	void SetObjectUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void Draw(ID3D11DeviceContext* d3d11DevCon, Mesh* mesh, Material* mat);
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* objectUniformBuffer;
	void SetVertexIndexBuffer(ID3D11DeviceContext* d3d11DevCon);
};

