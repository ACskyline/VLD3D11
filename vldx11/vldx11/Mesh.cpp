#include "Mesh.h"

Mesh::Mesh(MeshType _type) : 
	type(_type)
{
}

Mesh::~Mesh()
{
	MY_DELETE_ARRAY(vertices);
	MY_DELETE_ARRAY(indices);
}

void Mesh::DestroyMesh()
{
	vertexNum = 0;
	indexNum = 0;
	MY_DELETE_ARRAY(vertices);
	MY_DELETE_ARRAY(indices);
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

////////////////////////////////////////////////////////////////////////////

ConeMesh::ConeMesh(uint32_t _segmentNum) : Mesh(Mesh::MeshType::Cone), segmentNum(_segmentNum)
{
}

ConeMesh::~ConeMesh()
{
}

bool ConeMesh::InitMesh()
{
	DestroyMesh();

	vertexNum = segmentNum * 3 + segmentNum * 2 + 1;
	indexNum = segmentNum * 3 * 2;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	float deltaAngle = 360.f / segmentNum;

	for (uint32_t i = 0; i < segmentNum; i++)
	{
		//radius is 0.5
		uint32_t iNext = (i + 1) % segmentNum;

		float x = 0.5f * cosf(XMConvertToRadians(deltaAngle * i));
		float z = 0.5f * sinf(XMConvertToRadians(deltaAngle * i));
		float xNext = 0.5f * cosf(XMConvertToRadians(deltaAngle * iNext));
		float zNext = 0.5f * sinf(XMConvertToRadians(deltaAngle * iNext));
		float xNormal = 0.5f * cosf(XMConvertToRadians(deltaAngle * (i + 0.5f)));
		float zNormal = 0.5f * sinf(XMConvertToRadians(deltaAngle * (i + 0.5f)));

		XMFLOAT3 nor = XMFLOAT3(xNormal / sqrtf(0.5*0.5 + 0.25*0.25),
			0.25 / sqrtf(0.5*0.5 + 0.25*0.25),
			zNormal / sqrtf(0.5*0.5 + 0.25*0.25));//can be replaced with calculated result but keeping it this way makes it clear

		//for side triangle
		vertices[i * 5] = Vertex(XMFLOAT3(x, -0.5, z), nor);
		vertices[i * 5 + 1] = Vertex(XMFLOAT3(0, 0.5, 0), nor);//north pole
		vertices[i * 5 + 2] = Vertex(XMFLOAT3(xNext, -0.5, zNext), nor);
		//for bottom triangle
		vertices[i * 5 + 3] = Vertex(XMFLOAT3(x, -0.5, z), XMFLOAT3(0, -1, 0));
		vertices[i * 5 + 4] = Vertex(XMFLOAT3(xNext, -0.5, zNext), XMFLOAT3(0, -1, 0));
	}

	vertices[vertexNum - 1] = Vertex(XMFLOAT3(0, -0.5f, 0), XMFLOAT3(0, -1, 0));//south pole

	for (uint32_t i = 0; i < segmentNum; i++)
	{
		//side triangle
		indices[i * 6] = i * 5;
		indices[i * 6 + 1] = i * 5 + 1;
		indices[i * 6 + 2] = i * 5 + 2;

		//bottom triangle
		indices[i * 6 + 3] = i * 5 + 3;
		indices[i * 6 + 4] = i * 5 + 4;
		indices[i * 6 + 5] = vertexNum - 1;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SphereMesh::SphereMesh(uint32_t _altitudeSegmentNum, uint32_t _azimuthSegmentNum) 
	: Mesh(Mesh::MeshType::Sphere), 
	  altitudeSegmentNum(_altitudeSegmentNum), 
	  azimuthSegmentNum(_azimuthSegmentNum)
{
}

SphereMesh::~SphereMesh()
{
}

//adjacent triangles share vertices
bool SphereMesh::InitMesh()
{
	DestroyMesh();

	vertexNum = azimuthSegmentNum * (altitudeSegmentNum - 1) + 2;
	indexNum = azimuthSegmentNum * (2 + (altitudeSegmentNum - 2) * 2) * 3;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	float deltaAzimuthAngle = 360.f / azimuthSegmentNum;
	float deltaAltitudeAngle = 180.f / altitudeSegmentNum;

	uint32_t count = 0;
	for (uint32_t i = 0; i < azimuthSegmentNum; i++)
	{
		for (uint32_t j = 1; j <= altitudeSegmentNum - 1; j++)//exclude polar vertex
		{
			float y = 0.5f * cosf(XMConvertToRadians(j * deltaAltitudeAngle));//radius is 0.5
			float xoz = 0.5f * fabsf(sinf(XMConvertToRadians(j * deltaAltitudeAngle)));//radius is 0.5
			float x = xoz * cosf(XMConvertToRadians(i * deltaAzimuthAngle));
			float z = xoz * sinf(XMConvertToRadians(i * deltaAzimuthAngle));
			vertices[count++] = Vertex(XMFLOAT3(x, y, z), XMFLOAT3(2 * x, 2 * y, 2 * z));//normal is twice as long as radius
		}
	}
	vertices[count++] = Vertex(XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0, 1, 0));//north pole
	vertices[count++] = Vertex(XMFLOAT3(0, -0.5f, 0), XMFLOAT3(0, -1, 0));//south pole

	count = 0;
	for (uint32_t i = 0; i < azimuthSegmentNum; i++)
	{
		uint32_t iNext = (i + 1) % altitudeSegmentNum;

		//quad strap
		for (uint32_t j = 0; j <= altitudeSegmentNum - 3; j++)
		{
			indices[count++] = i * (altitudeSegmentNum - 1) + j;
			indices[count++] = iNext * (altitudeSegmentNum - 1) + j;
			indices[count++] = iNext * (altitudeSegmentNum - 1) + j + 1;

			indices[count++] = i * (altitudeSegmentNum - 1) + j;
			indices[count++] = iNext * (altitudeSegmentNum - 1) + j + 1;
			indices[count++] = i * (altitudeSegmentNum - 1) + j + 1;
		}

		//upper triangle
		indices[count++] = i * (altitudeSegmentNum - 1);
		indices[count++] = vertexNum - 2;
		indices[count++] = iNext * (altitudeSegmentNum - 1);

		//lower triangle
		indices[count++] = i * (altitudeSegmentNum - 1) + altitudeSegmentNum - 2;
		indices[count++] = iNext * (altitudeSegmentNum - 1) + altitudeSegmentNum - 2;
		indices[count++] = vertexNum - 1;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

ObjMesh::ObjMesh(wstring _fileName) : Mesh(Mesh::MeshType::Obj), fileName(_fileName)
{
}

ObjMesh::~ObjMesh()
{
}

//adjacent triangles do not share vertices(vertices are unique)
bool ObjMesh::InitMesh()
{
	DestroyMesh();
	return LoadObjMesh(fileName);
}

bool ObjMesh::LoadObjMesh(wstring fileName)
{
	fstream file;
	file.open(fileName, std::ios::in);
	if (file.is_open())
	{
		vector<XMFLOAT3> vecPos;
		vector<XMFLOAT3> vecNor;
		vector<XMFLOAT2> vecUV;
		vector<Point> vecPoint;

		std::string str;
		while (std::getline(file, str))
		{
			if (str.substr(0, 2) == "v ")
			{
				stringstream ss;
				ss << str.substr(2);
				XMFLOAT3 v;
				ss >> v.x;
				ss >> v.y;
				ss >> v.z;
				vecPos.push_back(v);
			}
			else if (str.substr(0, 3) == "vt ")
			{
				std::stringstream ss;
				ss << str.substr(3);
				XMFLOAT2 v;
				ss >> v.x;
				ss >> v.y;
				vecUV.push_back(v);
			}
			else if (str.substr(0, 3) == "vn ")
			{
				std::stringstream ss;
				ss << str.substr(3);
				XMFLOAT3 v;
				ss >> v.x;
				ss >> v.y;
				ss >> v.z;
				vecNor.push_back(v);
			}
			else if (str.substr(0, 2) == "f ")
			{
				std::stringstream ss;
				vector<Point> tempVecPoint;
				ss << str.substr(2);

				//Parsing
				ParseObjFace(ss, tempVecPoint);

				//if there are more than 3 vertices for one face then split it in to several triangles
				for (int i = 0; i < tempVecPoint.size(); i++)
				{
					if (i >= 3)
					{
						vecPoint.push_back(tempVecPoint.at(0));
						vecPoint.push_back(tempVecPoint.at(i - 1));
					}
					vecPoint.push_back(tempVecPoint.at(i));
				}

			}
			else if (str[0] == '#')
			{
				//comment
			}
			else
			{
				//others
			}
		}

		AssembleObjMesh(vecPos, vecUV, vecNor, vecPoint);

	}
	else
	{
		printf("can not open: %s\n", fileName.c_str());
		return false;
	}

	file.close();
	return true;
}

void ObjMesh::ParseObjFace(std::stringstream &ss, vector<Point>& tempVecPoint)
{
	char discard;
	char peek;
	int count;
	uint32_t data;
	Point tempPoint;

	//One vertex in one loop
	do
	{
		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			tempPoint.VI = data;//index start at 1 in an .obj file but at 0 in an array
			ss >> discard;
		}
		else
		{
			//push default value
			tempPoint.VI = 0;
		}

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			tempPoint.TI = data;//index start at 1 in an .obj file but at 0 in an array
			ss >> discard;
		}
		else
		{
			//push default value
			tempPoint.TI = 0;
		}

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			tempPoint.NI = data;//index start at 1 in an .obj file but at 0 in an array
			//no discard here because it is the end for this vertex
		}
		else
		{
			//push default value
			tempPoint.NI = 0;
		}

		tempVecPoint.push_back(tempPoint);

	} while (!ss.eof());
}

void ObjMesh::AssembleObjMesh(const vector<XMFLOAT3> &vecPos,
	const vector<XMFLOAT2> &vecUV,
	const vector<XMFLOAT3> &vecNor,
	const vector<Point> &vecPoint)
{
	int n = vecPoint.size();
	vertexNum = n;
	indexNum = n;

	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	for (int i = 0; i < n; i++)
	{
		XMFLOAT3 pos(0, 0, 0);
		XMFLOAT3 nor(0, 0, 0);
		XMFLOAT2 uv(0, 0);

		if (vecPoint[i].VI > 0) pos = vecPos[vecPoint[i].VI - 1];//index start at 1 in an .obj file but at 0 in an array, 0 was used to mark not-have-pos
		if (vecPoint[i].NI > 0) nor = vecNor[vecPoint[i].NI - 1];//index start at 1 in an .obj file but at 0 in an array, 0 was used to mark not-have-nor
		if (vecPoint[i].TI > 0) uv = vecUV[vecPoint[i].TI - 1];//index start at 1 in an .obj file but at 0 in an array, 0 was used to mark not-have-uv

		vertices[i] = Vertex(pos, nor, uv);
		indices[i] = i;//this way, all 3 vertices on every triangle are unique, even though they belong to the same polygon, which increase storing space but allow for finer control
	}
}

////////////////////////////////////////////////////////////////////////////

GridMesh::GridMesh(uint32_t _segmentNum) : Mesh(Mesh::MeshType::Grid), segmentNum(_segmentNum)
{
}

GridMesh::~GridMesh()
{
}

bool GridMesh::InitMesh()
{
	DestroyMesh();

	vertexNum = 4 * segmentNum;
	indexNum = vertexNum + 4;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	//segmentNum + 1 lines along z axis
	uint32_t count = 0;
	for (uint32_t i = 0; i < segmentNum + 1; i++)
	{
		vertices[i * 2] = Vertex(-(int)segmentNum / 2 + (int)i, 0, -(int)segmentNum / 2, 1, 1, 1, 1);
		vertices[i * 2 + 1] = Vertex(-(int)segmentNum / 2 + (int)i, 0, (int)segmentNum / 2, 1, 1, 1, 1);
		indices[i * 2] = i * 2;
		indices[i * 2 + 1] = i * 2 + 1;
		count += 2;
	}

	//segmentNum - 1 lines along x axis
	for (uint32_t i = 0; i < segmentNum - 1; i++)
	{
		vertices[count + i * 2] = Vertex(-(int)segmentNum / 2, 0, -((int)segmentNum / 2 - 1) + (int)i, 1, 1, 1, 1);
		vertices[count + i * 2 + 1] = Vertex((int)segmentNum / 2, 0, -((int)segmentNum / 2 - 1) + (int)i, 1, 1, 1, 1);
		indices[count + i * 2] = count + i * 2;
		indices[count + i * 2 + 1] = count + i * 2 + 1;
	}

	indices[indexNum - 4] = 0;
	indices[indexNum - 3] = 2 * segmentNum;
	indices[indexNum - 2] = 1;
	indices[indexNum - 1] = 1 + 2 * segmentNum;

	return true;
}

////////////////////////////////////////////////////////////////////////////

AxisMesh::AxisMesh() : Mesh(Mesh::MeshType::Axis)
{
}

AxisMesh::~AxisMesh()
{
}

bool AxisMesh::InitMesh()
{
	DestroyMesh();

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

	return true;
}

////////////////////////////////////////////////////////////////////////////

CubeMesh::CubeMesh() : Mesh(Mesh::MeshType::Cube)
{
}

CubeMesh::~CubeMesh()
{
}

//each square face of the cube has 2 shared vertices
bool CubeMesh::InitMesh()
{
	DestroyMesh();

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

	XMFLOAT2 uv[4] = { XMFLOAT2(0.0, 0.0),
					   XMFLOAT2(0.0, 1.0),
					   XMFLOAT2(1.0, 1.0),
					   XMFLOAT2(1.0, 0.0) };

	for (int i = 0; i < 4; i++)
	{
		vertices[i * 4 + 0] = Vertex(pos[0 + i], nor[i], uv[0], col[0 + i]);
		vertices[i * 4 + 1] = Vertex(pos[(4 - i) % 4 + 4], nor[i], uv[1], col[(4 - i) % 4 + 4]);
		vertices[i * 4 + 2] = Vertex(pos[7 - i], nor[i], uv[2], col[7 - i]);
		vertices[i * 4 + 3] = Vertex(pos[(1 + i) % 4], nor[i], uv[3], col[(1 + i) % 4]);
	}

	for (int i = 0; i < 4; i++)
	{
		vertices[16 + i] = Vertex(pos[i], nor[4], uv[i], col[i]);
		vertices[20 + i] = Vertex(pos[i + 4], nor[5], uv[i], col[i + 4]);
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

	return true;
}

////////////////////////////////////////////////////////////////////////////

PlaneMesh::PlaneMesh() : Mesh(Mesh::MeshType::Plane)
{
}

PlaneMesh::~PlaneMesh()
{
}

bool PlaneMesh::InitMesh()
{
	DestroyMesh();

	vertexNum = 4;
	indexNum = 6;
	vertices = new Vertex[vertexNum];
	indices = new Index[indexNum];

	XMFLOAT3 nor(0, 1, 0);//facing up

	vertices[0] = Vertex(XMFLOAT3(-0.5, 0, -0.5), nor, XMFLOAT2(0, 0));
	vertices[1] = Vertex(XMFLOAT3(-0.5, 0, 0.5), nor, XMFLOAT2(0, 1));
	vertices[2] = Vertex(XMFLOAT3(0.5, 0, 0.5), nor, XMFLOAT2(1, 1));
	vertices[3] = Vertex(XMFLOAT3(0.5, 0, -0.5), nor, XMFLOAT2(1, 0));

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	return true;
}
