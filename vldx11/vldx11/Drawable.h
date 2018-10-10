#pragma once
#include "GlobalInclude.h"
#include "Mesh.h"
#include "Material.h"
#include "ObjectUniform.h"
#include "FrameUniform.h"
#include "SceneUniform.h"

class Drawable
{
public:
	enum DrawableType { TrianlgeList, LineList };

	Drawable(DrawableType _type, Mesh* _pMesh, Material* _pMat);
	~Drawable();
	bool CreateBuffer(ID3D11Device* d3d11Device);
	void VertexIndexBufferData(ID3D11DeviceContext* d3d11DevCon);
	void Draw(ID3D11DeviceContext* d3d11DevCon);
	void Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat);
	void Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat, FrameUniform* _pFrameUniform);
	bool InitDrawable(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool IsInitiated();
	void ConnectObjectUniform(ObjectUniform *_pObjectUniform);
	void ConnectFrameUniform(FrameUniform *_pFrameUniform);
	void ConnectSceneUniform(SceneUniform *_pSceneUniform);

private:
	DrawableType type;
	Mesh* pMesh;
	Material* pMat;
	ObjectUniform* pObjectUniform;
	FrameUniform* pFrameUniform;
	SceneUniform* pSceneUniform;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	void SetVertexIndexBuffer(ID3D11DeviceContext* d3d11DevCon);
	void UseTriangleList(ID3D11DeviceContext* d3d11DevCon);
	void UseLineList(ID3D11DeviceContext* d3d11DevCon);
	bool initiated;
};

