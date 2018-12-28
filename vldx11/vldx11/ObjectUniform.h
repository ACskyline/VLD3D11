#pragma once
#include "GlobalInclude.h"
#include "Transform.h"

class ObjectUniform
{
	struct ObjectUniformData
	{
		XMFLOAT4X4 M;
		XMFLOAT4X4 M_INV;
		XMFLOAT4X4 M_INV_TRANS;
		ObjectUniformData() :
			M(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			M_INV(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)),
			M_INV_TRANS(XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1))
		{}
	};

public:
	ObjectUniform();
	~ObjectUniform();

	bool CreateBuffer(ID3D11Device* d3d11Device);
	void ObjectUniformBufferData(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferPS(ID3D11DeviceContext* d3d11DevCon);
	void SetObjectUniformBufferVSPS(ID3D11DeviceContext* d3d11DevCon);
	void ApplyTransform(Transform* pTransform);
	bool InitObjectUniform(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	bool NeedToUpload();

private:
	bool needToUpload;
	ObjectUniformData objectUniformData;
	ID3D11Buffer* objectUniformBuffer;	
	void SetM(Transform* pTransform);
	void SetM_INV(Transform* pTransform);
	void SetM_INV_TRANS(Transform* pTransform);
};

