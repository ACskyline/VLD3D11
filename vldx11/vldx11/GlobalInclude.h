#pragma once

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <cmath>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>

#ifndef HALF_PI
#define HALF_PI 1.570796326794896
#endif

#ifndef DEGREE_TO_RADIAN
#define DEGREE_TO_RADIAN 0.017453292519943
#endif

#ifndef MY_DELETE(ptr)
#define MY_DELETE(ptr) if(ptr!=nullptr){ delete ptr; ptr = nullptr;}
#endif

#ifndef MY_DELETE_ARRAY(ptr)
#define MY_DELETE_ARRAY(ptr) if(ptr!=nullptr){ delete [] ptr; ptr = nullptr;}
#endif

#ifndef OBJECT_UNIFORM_SOLT
#define OBJECT_UNIFORM_SOLT 0
#endif

#ifndef FRAME_UNIFORM_SOLT
#define FRAME_UNIFORM_SOLT 1
#endif

using namespace DirectX;
using namespace std;

struct ObjectUniform
{
	XMFLOAT4X4 M;
	XMFLOAT4 COL;
};

struct FrameUniform
{
	XMFLOAT4X4 VP;
	XMFLOAT4 COL;
};

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float r, float g, float b, float a) : pos(x,y,z), col(r, g, b, a) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT3 _nor) : Vertex(_pos, _nor, XMFLOAT4(1,1,1,1)) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT3 _nor, XMFLOAT4 _col) : pos(_pos), nor(_nor), col(_col) {}

	XMFLOAT3 pos;
	XMFLOAT4 col;
	XMFLOAT3 nor;
	XMFLOAT2 uv;
};

typedef uint32_t Index;

bool CheckError(HRESULT hr, ID3D10Blob* error_message);

void PrintMatrix(XMFLOAT4X4 mat);
