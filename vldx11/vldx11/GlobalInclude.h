#pragma once

#include <windows.h>
#include <dinput.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <cmath>
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "WICTextureLoader\WICTextureLoader.h"
#include "DDSTextureLoader\DDSTextureLoader.h"

#define HALF_PI 1.570796326794896
#define EPSILON 0.00000001
#define DEGREE_TO_RADIAN 0.017453292519943

#define MY_DELETE(ptr) if(ptr!=nullptr){ delete ptr; ptr = nullptr;}
#define MY_DELETE_ARRAY(ptr) if(ptr!=nullptr){ delete [] ptr; ptr = nullptr;}

#define OBJECT_UNIFORM_SOLT 0
#define FRAME_UNIFORM_SOLT 1
#define SCENE_UNIFORM_SOLT 2

#define SHADOW_TEXTURE_SLOT 0
#define MAIN_TEXTURE_SLOT 1
#define MAX_TEXTURE_SLOT 2

#define KEYDOWN(name, key) ((name)[(key)] & 0x80)

using namespace DirectX;
using namespace std;

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float r, float g, float b, float a) : Vertex(XMFLOAT3(x, y, z), XMFLOAT4(r, g, b, a)) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT4 _col) : Vertex(_pos, XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), _col) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT3 _nor) : Vertex(_pos, _nor, XMFLOAT2(0,0), XMFLOAT4(1,1,1,1)) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT3 _nor, XMFLOAT4 _col) : Vertex(_pos, _nor, XMFLOAT2(0, 0), _col) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT3 _nor, XMFLOAT2 _uv) : Vertex(_pos, _nor, _uv, XMFLOAT4(1, 1, 1, 1)) {}
	Vertex(XMFLOAT3 _pos, XMFLOAT3 _nor, XMFLOAT2 _uv, XMFLOAT4 _col) : pos(_pos), nor(_nor), uv(_uv), col(_col) {}

	XMFLOAT3 pos;
	XMFLOAT4 col;
	XMFLOAT3 nor;
	XMFLOAT2 uv;
};

typedef uint32_t Index;

bool CheckError(HRESULT hr, ID3D10Blob* error_message);

void PrintMatrix(XMFLOAT4X4 mat);
