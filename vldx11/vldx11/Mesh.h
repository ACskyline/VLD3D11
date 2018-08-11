#pragma once

#include "GlobalInclude.h"

class Mesh
{
public:
	Mesh();
	Mesh(uint8_t _type, uint32_t _vertexNum, uint32_t _indexNum, Vertex* _vertices, Index* _indices);
	~Mesh();

	void PrintAll();

	uint32_t vertexNum;
	uint32_t indexNum;
	Vertex* vertices;
	Index* indices;

private:

	uint8_t type;
	void InitCube();
	void InitSquare();
	void InitAxis();
};

