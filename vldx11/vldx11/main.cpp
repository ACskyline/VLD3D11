#include <windows.h>
#include <dinput.h>
#include "Object.h"
#include "OrbitCamera.h"
#include "DrawableGroup.h"

IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "MYPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYNORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

LPCTSTR WndClassName = "firstwindow";
HWND hwnd = NULL;
const int Width = 800;
const int Height = 600;
uint32_t frame = 0;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;
DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

Mesh mMeshVolume(Mesh::MeshType::Sphere, 32, 32);
Mesh mMeshAxis(Mesh::MeshType::Axis);
Mesh mMeshGrid(Mesh::MeshType::Grid);
Material mMaterialVolume(L"myVert.hlsl", L"myPixelHalfLambert.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialGizmo(L"myVert.hlsl", L"myPixel.hlsl", layout, ARRAYSIZE(layout));
Drawable mDrawableVolume(Drawable::DrawableType::TrianlgeList);
Drawable mDrawableAxis(Drawable::DrawableType::LineList);
Drawable mDrawableGrid(Drawable::DrawableType::LineList);
Transform mTransformVolume(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(8, 8, 8));
Transform mTransformAxis(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
OrbitCamera mCamera(10.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 120, Width / (float)Height, 0.01f, 100.0f);
Light mLight(XMFLOAT3(0, 0, 0), XMFLOAT4(0.8, 0.7, 0, 1));
FrameUniform mFrameUniform;
SceneUniform mSceneUniform;
Object objVolume;
Object objLight;
Object objCamera;
Object objAxis;
Object objGrid;
DrawableGroup grpDrawableVolume(DrawableGroup::DrawableGroupType::VolumeLight);
DrawableGroup grpDrawableGizmo(DrawableGroup::DrawableGroupType::Gizmo);

void InitConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* stream;
	freopen_s(&stream, "CON", "w", stdout);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, "Are you sure you want to exit?", "Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool InitWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);// IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);// IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);// IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error registering class", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(NULL, WndClassName, "RakimIsGod", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);

	if (!hwnd)
	{
		MessageBox(NULL, "Error creating window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

bool InitDepthStencilView()
{
	HRESULT hr;
	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	if (!CheckError(hr, nullptr)) return false;

	hr = d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (!CheckError(hr, nullptr)) return false;

	return true;
}

bool InitD3D11App()
{
	HRESULT hr;

	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;//double buffer
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);
	if (!CheckError(hr, nullptr)) return false;

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create our Render Target
	hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();
	if (!CheckError(hr, nullptr)) return false;

	//Create our Depth Stencil View
	if (!InitDepthStencilView()) return false;

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	return true;
}

void InitViewport()
{
	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);
}

bool InitDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;

	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	if (!CheckError(hr, nullptr)) return false;

	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
	if (!CheckError(hr, nullptr)) return false;

	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	if (!CheckError(hr, nullptr)) return false;

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (!CheckError(hr, nullptr)) return false;

	hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (!CheckError(hr, nullptr)) return false;

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	if (!CheckError(hr, nullptr)) return false;

	hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (!CheckError(hr, nullptr)) return false;

	return true;
}

bool InitLevel()
{
	printf("init Level start!\n");
	InitViewport();

	//Create and set buffers
	mDrawableVolume.ApplyColor(1, 1, 1, 1);
	mDrawableAxis.ApplyColor(1, 1, 1, 1);
	mDrawableGrid.ApplyColor(1, 1, 1, 1);

	grpDrawableVolume.AddDrawable(&mDrawableVolume);
	grpDrawableVolume.InitDrawableGroup(d3d11Device);

	grpDrawableGizmo.AddDrawable(&mDrawableAxis);
	grpDrawableGizmo.AddDrawable(&mDrawableGrid);
	grpDrawableGizmo.InitDrawableGroup(d3d11Device);

	mFrameUniform.ApplyCol(1, 1, 1, 1);
	mFrameUniform.ApplyIntensity(5.f);
	mFrameUniform.ApplyFrameNum(frame);

	mSceneUniform.ApplyStep(50);

	objVolume.SetMesh(&mMeshVolume);
	objVolume.SetTransform(&mTransformVolume);
	objVolume.SetMaterial(&mMaterialVolume);
	objVolume.SetDrawable(&mDrawableVolume);

	objGrid.SetMesh(&mMeshGrid);
	objGrid.SetMaterial(&mMaterialGizmo);
	objGrid.SetDrawable(&mDrawableGrid);
	
	objAxis.SetMesh(&mMeshAxis);
	objAxis.SetTransform(&mTransformAxis);
	objAxis.SetMaterial(&mMaterialGizmo);
	objAxis.SetDrawable(&mDrawableAxis);

	objCamera.SetCamera(&mCamera);
	objCamera.ConnectFrameUniform(&mFrameUniform);

	objLight.SetLight(&mLight);
	objLight.ConnectSceneUniform(&mSceneUniform);
	
	if (!objVolume.InitObject(d3d11Device, d3d11DevCon)) return false;
	if (!objAxis.InitObject(d3d11Device, d3d11DevCon)) return false;
	if (!objGrid.InitObject(d3d11Device, d3d11DevCon)) return false;
	if (!objCamera.InitObject(d3d11Device, d3d11DevCon)) return false;
	if (!objLight.InitObject(d3d11Device, d3d11DevCon)) return false;
	if (!mFrameUniform.InitFrameUniform(d3d11Device, d3d11DevCon)) return false;
	if (!mSceneUniform.InitSceneUniform(d3d11Device, d3d11DevCon)) return false;

	printf("init level finish!\n");
	return true;
}

void DetectInput()
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}

	if (keyboardState[DIK_W] & 0x80)
	{
		mTransformVolume.position.y += 0.0005;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		mTransformVolume.position.y -= 0.0005;
	}
	if (keyboardState[DIK_A] & 0x80)
	{
		mTransformVolume.position.x -= 0.0005;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		mTransformVolume.position.x += 0.0005;
	}
	if (keyboardState[DIK_Q] & 0x80)
	{
		mTransformVolume.position.z -= 0.0005;
	}
	if (keyboardState[DIK_E] & 0x80)
	{
		mTransformVolume.position.z += 0.0005;
	}
	if (keyboardState[DIK_UP] & 0x80)
	{
		mLight.pos.y += 0.005;
		mTransformAxis.position.y += 0.005;
	}
	if (keyboardState[DIK_DOWN] & 0x80)
	{
		mLight.pos.y -= 0.005;
		mTransformAxis.position.y -= 0.005;
	}
	if (keyboardState[DIK_LEFT] & 0x80)
	{
		mLight.pos.x -= 0.005;
		mTransformAxis.position.x -= 0.005;
	}
	if (keyboardState[DIK_RIGHT] & 0x80)
	{
		mLight.pos.x += 0.005;
		mTransformAxis.position.x += 0.005;
	}
	if (keyboardState[DIK_COMMA] & 0x80)
	{
		mLight.pos.z -= 0.005;
		mTransformAxis.position.z -= 0.005;
	}
	if (keyboardState[DIK_PERIOD] & 0x80)
	{
		mLight.pos.z += 0.005;
		mTransformAxis.position.z += 0.005;
	}
	if (mouseCurrState.lX != 0)
	{
		if (keyboardState[DIK_LCONTROL] & 0x80)
			mTransformVolume.rotation.y += mouseCurrState.lX * 0.01;
		else
			mCamera.horizontalAngle += mouseCurrState.lX * 0.1;
	}
	if (mouseCurrState.lY != 0)
	{
		if (keyboardState[DIK_LCONTROL] & 0x80)
		{
			mTransformVolume.rotation.z += mouseCurrState.lY * 0.01;
		}
		else
		{
			mCamera.verticalAngle += mouseCurrState.lY * 0.1;
			if (mCamera.verticalAngle > 90 - EPSILON)
				mCamera.verticalAngle = 89 - EPSILON;
			if (mCamera.verticalAngle < -90 + EPSILON)
				mCamera.verticalAngle = -89 + EPSILON;
		}
	}
	if (mouseCurrState.lZ != 0)
	{
		mCamera.distance -= mouseCurrState.lZ * 0.01;
	}

	mouseLastState = mouseCurrState;

	return;
}

void UpdateScene()
{
	objVolume.UpdateObject(d3d11DevCon);
	objAxis.UpdateObject(d3d11DevCon);
	objCamera.UpdateObject(d3d11DevCon);
	objLight.UpdateObject(d3d11DevCon);

	frame++;
	mFrameUniform.ApplyFrameNum(frame);
}

void DrawScene()
{
	//Clear our backbuffer to the updated color
	XMVECTORF32 bgColorV { 0,0,0,1 };

	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColorV);
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Draw Call
	grpDrawableVolume.Draw(d3d11DevCon);
	grpDrawableGizmo.Draw(d3d11DevCon);

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

int Messageloop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// run game code
			DetectInput();
			UpdateScene();
			DrawScene();
		}
	}
	return msg.wParam;
}

void ReleaseObjects()
{
	//Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();

	renderTargetView->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();

	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitConsole();

	if (!InitWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, "Window Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitD3D11App())    //Initialize Direct3D
	{
		MessageBox(0, "Direct3D Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitDirectInput(hInstance))	//Initialize input device
	{
		MessageBox(0, "Input Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitLevel())    //Initialize our scene
	{
		MessageBox(0, "Level Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	Messageloop();

	ReleaseObjects();

	return 0;
}

bool CheckError(HRESULT hr, ID3D10Blob* error_message)
{
	if (FAILED(hr))
	{
		printf("FAILED:0x%x\n", hr);
		if (error_message != nullptr)
		{
			printf("return value: %d, error message: %s\n", hr, (char*)error_message->GetBufferPointer());
		}
		return false;
	}
	return true;
}

void PrintMatrix(XMFLOAT4X4 mat)
{
	std::cout << "mat" << std::endl;
	std::cout << "row 1: " << mat._11 << ", " << mat._12 << ", " << mat._13 << ", " << mat._14 << std::endl;
	std::cout << "row 2: " << mat._21 << ", " << mat._22 << ", " << mat._23 << ", " << mat._24 << std::endl;
	std::cout << "row 3: " << mat._31 << ", " << mat._32 << ", " << mat._33 << ", " << mat._34 << std::endl;
	std::cout << "row 4: " << mat._41 << ", " << mat._42 << ", " << mat._43 << ", " << mat._44 << std::endl;
}
