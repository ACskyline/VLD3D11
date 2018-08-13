#pragma once

#include "GlobalInclude.h"

class Mesh
{
public:
	enum MeshType { Cube, Quad, Axis, Grid };

	Mesh(MeshType _type, uint32_t _vertexNum, uint32_t _indexNum, Vertex* _vertices, Index* _indices);
	~Mesh();

	void PrintAll();

	uint32_t vertexNum;
	uint32_t indexNum;
	Vertex* vertices;
	Index* indices;

private:

	MeshType type;

	void InitCube();
	void InitSquare();
	void InitAxis();
	void InitGrid();
};

