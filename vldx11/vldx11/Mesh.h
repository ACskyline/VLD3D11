#pragma once

#include "GlobalInclude.h"

class Mesh
{
public:
	enum MeshType { Sphere, Cube, Quad, Axis, Grid, OBJ };

	Mesh(MeshType _type);
	Mesh(MeshType _type, string _fileName);
	Mesh(MeshType _type, uint32_t _altitudeSegmentNum, uint32_t _azimuthSegmentNum);
	~Mesh();

	bool InitMesh();
	void DestroyMesh();
	void PrintAll();

	uint32_t vertexNum;
	uint32_t indexNum;
	Vertex* vertices;
	Index* indices;

private:
	Mesh(MeshType _type, string _fileName, uint32_t _altitudeSegmentNum, uint32_t _azimuthSegmentNum);

	struct Point
	{
		uint32_t VI;
		uint32_t TI;
		uint32_t NI;
	};

	MeshType type;
	string fileName;
	uint32_t altitudeSegmentNum;
	uint32_t azimuthSegmentNum;

	bool InitSphere();
	bool InitCube();
	bool InitSquare();
	bool InitAxis();
	bool InitGrid();
	bool InitOBJ(string fileName);

	bool LoadObjMesh(string fileName);
	void ParseObjFace(stringstream &ss, vector<Point> &tempVecPoint);
	void AssembleMesh(const vector<XMFLOAT3> &vecPos, 
					  const vector<XMFLOAT2> &vecUV, 
					  const vector<XMFLOAT3> &vecNor, 
					  const vector<Point> &vecPoint);
};