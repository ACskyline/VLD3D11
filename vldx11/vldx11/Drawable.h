#pragma once
#include "GlobalInclude.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Object.h"

class Object;

class Drawable
{
	struct ObjectUniformData
	{
		XMFLOAT4X4 M;
		XMFLOAT4X4 M_INV;
		XMFLOAT4 COL;
		ObjectUniformData() :
			M(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			M_INV(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			COL(XMFLOAT4(1, 1, 1, 1))
		{}
	};
public:
	enum DrawableType { TrianlgeList, LineList };

	Drawable(DrawableType _type, Mesh* _pMesh, Material* _pMat);
	~Drawable();
	bool CreateBuffer(ID3D11Device* d3d11Device);
	void VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon);
	void ObjectUniformBufferData(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void Draw(ID3D11DeviceContext* d3d11DevCon);
	void ApplyTransform(Transform* pTransform);
	void ApplyColor(float r, float g, float b, float a);

	bool InitDrawable(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool IsInitiated();

private:
	ObjectUniformData objectUniformData;
	DrawableType type;
	Mesh* pMesh;
	Material* pMat;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* objectUniformBuffer;
	void SetVertexIndexBuffer(ID3D11DeviceContext* d3d11DevCon);
	void SetM(Transform* pTransform);
	void SetM_INV(Transform* pTransform);
	void UseTriangleList(ID3D11DeviceContext* d3d11DevCon);
	void UseLineList(ID3D11DeviceContext* d3d11DevCon);
	bool initiated;
};

