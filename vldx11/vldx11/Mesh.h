#pragma once

#include "GlobalInclude.h"

class Mesh
{
public:
	enum MeshType { Sphere, Cube, Cone, Plane, Axis, Grid, Obj };

	virtual ~Mesh();

	virtual bool InitMesh() = 0;
	void DestroyMesh();
	void PrintAll();
	bool IsInitiated();

	uint32_t vertexNum;
	uint32_t indexNum;
	Vertex* vertices;
	Index* indices;

protected:
	Mesh(MeshType _type);
	bool initiated;

private:
	MeshType type;
};

class CubeMesh : public Mesh
{
public:
	CubeMesh();
	~CubeMesh();

	bool InitMesh();
};

class PlaneMesh : public Mesh
{
public:
	PlaneMesh();
	~PlaneMesh();

	bool InitMesh();
};

class AxisMesh : public Mesh
{
public:
	AxisMesh();
	~AxisMesh();

	bool InitMesh();
};

class GridMesh : public Mesh
{
public:
	GridMesh(uint32_t _segmentNum);
	~GridMesh();

	bool InitMesh();

private:
	uint32_t segmentNum;
};

class ConeMesh : public Mesh
{
public:
	ConeMesh(uint32_t _segmentNum);
	~ConeMesh();

	bool InitMesh();

private:
	uint32_t segmentNum;
};

class SphereMesh : public Mesh
{
public:
	SphereMesh(uint32_t _altitudeSegmentNum, uint32_t _azimuthSegmentNum);
	~SphereMesh();

	bool InitMesh();

private:
	uint32_t altitudeSegmentNum;
	uint32_t azimuthSegmentNum;
};

class ObjMesh : public Mesh
{
public:
	ObjMesh(string _fileName);
	~ObjMesh();

	bool InitMesh();

private:
	struct Point
	{
		uint32_t VI;
		uint32_t TI;
		uint32_t NI;
	};

	string fileName;

	bool LoadObjMesh(string fileName);
	void ParseObjFace(stringstream &ss, vector<Point> &tempVecPoint);
	void AssembleObjMesh(const vector<XMFLOAT3> &vecPos,
		const vector<XMFLOAT2> &vecUV,
		const vector<XMFLOAT3> &vecNor,
		const vector<Point> &vecPoint);
};