#include "Mesh.h"

Mesh::Mesh(MeshType _type, uint32_t _vertexNum, uint32_t _indexNum, Vertex* _vertices, Index* _indices) : 
	type(_type)
{
	if (type == MeshType::Cube)
	{
		InitCube();
	}
	else if (type == MeshType::Quad)
	{
		InitSquare();
	}
	else if (type == MeshType::Axis)
	{
		InitAxis();
	}
	else if (type == MeshType::Grid)
	{
		InitGrid();
	}
}

Mesh::~Mesh()
{
	MY_DELETE_ARRAY(vertices);
	MY_DELETE_ARRAY(indices);
}

void Mesh::InitGrid()
{
	vertexNum = 40;
	indexNum = 44;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	//11 lines along z axis
	for (int i = 0; i < 11; i++)
	{
		vertices[i * 2] = Vertex(-5 + i, 0, -5, 1, 1, 1, 1);
		vertices[i * 2 + 1] = Vertex(-5 + i, 0, 5, 1, 1, 1, 1);
		indices[i * 2] = i * 2;
		indices[i * 2 + 1] = i * 2 + 1;
	}

	//9 lines along x axis
	for (int i = 0; i < 9; i++)
	{
		vertices[22 + i * 2] = Vertex(-5, 0, -4 + i, 1, 1, 1, 1);
		vertices[22 + i * 2 + 1] = Vertex(5, 0, -4 + i, 1, 1, 1, 1);
		indices[22 + i * 2] = 22 + i * 2;
		indices[22 + i * 2 + 1] = 22 + i * 2 + 1;
	}

	indices[40] = 0;
	indices[41] = 20;
	indices[42] = 1;
	indices[43] = 21;
}

void Mesh::InitAxis()
{
	vertexNum = 6;
	indexNum = 6;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	vertices[0] = Vertex(0, 0, 0, 1, 0, 0, 1);
	vertices[1] = Vertex(1, 0, 0, 1, 0, 0, 1);
	vertices[2] = Vertex(0, 0, 0, 0, 1, 0, 1);
	vertices[3] = Vertex(0, 1, 0, 0, 1, 0, 1);
	vertices[4] = Vertex(0, 0, 0, 0, 0, 1, 1);
	vertices[5] = Vertex(0, 0, 1, 0, 0, 1, 1);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

}

void Mesh::InitCube()
{
	vertexNum = 24;
	indexNum = 36;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	XMFLOAT3 nor[6] = { XMFLOAT3(0,0,-1),
						 XMFLOAT3(1,0,0),
						 XMFLOAT3(0,0,1),
						 XMFLOAT3(-1,0,0),
						 XMFLOAT3(0,-1,0),
						 XMFLOAT3(0,1,0) };

	XMFLOAT3 pos[8] = { XMFLOAT3(-0.5,-0.5,-0.5),
						XMFLOAT3(0.5,-0.5,-0.5),
						XMFLOAT3(0.5,-0.5,0.5),
						XMFLOAT3(-0.5,-0.5,0.5),
						XMFLOAT3(-0.5,0.5,-0.5),
						XMFLOAT3(-0.5,0.5,0.5),
						XMFLOAT3(0.5,0.5,0.5),
						XMFLOAT3(0.5,0.5,-0.5) };

	XMFLOAT4 col[8] = { XMFLOAT4(0.9,0.5,0.1,1),
						XMFLOAT4(0.9,0.1,0.5,1),
						XMFLOAT4(0.5,0.9,0.1,1),
						XMFLOAT4(0.5,0.1,0.9,1),
						XMFLOAT4(0.1,0.9,0.5,1),
						XMFLOAT4(0.1,0.5,0.9,1),
						XMFLOAT4(0.9,0.9,0.5,1),
						XMFLOAT4(0.9,0.5,0.5,1) };

	for (int i = 0; i < 4; i++)
	{
		vertices[i * 4 + 0] = Vertex(pos[0 + i], nor[i], col[0 + i]);
		vertices[i * 4 + 1] = Vertex(pos[(4 - i) % 4 + 4], nor[i], col[(4 - i) % 4 + 4]);
		vertices[i * 4 + 2] = Vertex(pos[7 - i], nor[i], col[7 - i]);
		vertices[i * 4 + 3] = Vertex(pos[(1 + i) % 4], nor[i], col[(1 + i) % 4]);
	}

	for (int i = 0; i < 4; i++)
	{
		vertices[16 + i] = Vertex(pos[i], nor[4], col[i]);
		vertices[20 + i] = Vertex(pos[i + 4], nor[5], col[i + 4]);
	}

	for (int i = 0; i < indexNum; i += 3)
	{
		int ver = i % 3;
		int tri = i / 3;
		int tri_half = tri % 2;
		int face = tri / 2;

		indices[i] = face * 4 + (0 + tri_half * 2 + ver) % 4;
		indices[i + 1] = face * 4 + (1 + tri_half * 2 + ver) % 4;
		indices[i + 2] = face * 4 + (2 + tri_half * 2 + ver) % 4;
	}

}

void Mesh::InitSquare()
{
	vertexNum = 4;
	indexNum = 6;

	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	XMFLOAT3 nor(0, 0, -1);

	vertices[0] = Vertex(XMFLOAT3(0.5, 0.5, 0), nor);
	vertices[1] = Vertex(XMFLOAT3(0.5, -0.5, 0), nor);
	vertices[2] = Vertex(XMFLOAT3(-0.5, -0.5, 0), nor);
	vertices[3] = Vertex(XMFLOAT3(-0.5, 0.5, 0), nor);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;
}

void Mesh::PrintAll()
{
	for (int i = 0; i < vertexNum; i++)
	{
		cout << "v" << i << "p:" << vertices[i].pos.x << "," << vertices[i].pos.y << "," << vertices[i].pos.z << "-----"
			<< "v" << i << "n:" << vertices[i].nor.x << "," << vertices[i].nor.y << "," << vertices[i].nor.z << endl;
	}

	for (int i = 0; i < indexNum; i++)
	{
		cout << "i" << i << ":" << indices[i] << endl;
	}
}