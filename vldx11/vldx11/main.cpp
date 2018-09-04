#include "Renderer.h"
#include "Object.h"

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "MYPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYNORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

LPCTSTR WndClassName = "firstwindow";
HWND hwnd = NULL;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;
DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

Renderer mRenderer(WIDTH, HEIGHT);
CubeMesh mMeshVolume;
AxisMesh mMeshAxis;
GridMesh mMeshGrid(50);
PlaneMesh mMeshPlane;
Material mMaterialVolume(L"myVert.hlsl", L"myPixelTexturePointLight.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialGizmo(L"myVert.hlsl", L"myPixel.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialStandard(L"myVert.hlsl", L"myPixelTexturePointLightShadow.hlsl", layout, ARRAYSIZE(layout));
Drawable mDrawableVolume(Drawable::DrawableType::TrianlgeList, &mMeshVolume, &mMaterialVolume);
Drawable mDrawableAxis(Drawable::DrawableType::LineList, &mMeshAxis, &mMaterialGizmo);
Drawable mDrawableGrid(Drawable::DrawableType::LineList, &mMeshGrid, &mMaterialGizmo);
Drawable mDrawablePlane(Drawable::DrawableType::TrianlgeList, &mMeshPlane, &mMaterialStandard);
Transform mTransformVolume(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(10, 10, 10));
Transform mTransformAxis(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformGrid(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformPlane(XMFLOAT3(0, -10, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(50, 1, 50));
OrbitCamera mCamera(10.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH / (float)HEIGHT, 0.01f, 100.0f);
PointLight mLight(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 1, 1, 1), 20);
Texture mTex(Texture::TextureType::Default, L"checkerboard.jpg");
RenderTexture mRTex(RenderTexture::RenderTextureType::Default);
ObjectUniform mObjUniVolume;
ObjectUniform mObjUniAxis;
ObjectUniform mObjUniGrid;
ObjectUniform mObjUniPlane;
FrameUniform mFrameUniform;
SceneUniform mSceneUniform;
Object objVolume;
Object objLight;
Object objCamera;
Object objAxis;
Object objGrid;
Object objPlane;
DrawableGroup grpDrawableVolume(DrawableGroup::DrawableGroupType::VolumeLight);
DrawableGroup grpDrawableGizmo(DrawableGroup::DrawableGroupType::Gizmo);
DrawableGroup grpDrawableStandard(DrawableGroup::DrawableGroupType::Standard);

vector<Object*> GlobalObjVec; 
vector<DrawableGroup*> GlobalDrawableGrpVec;
vector<ObjectUniform*> GlobalObjectUniVec;
vector<FrameUniform*> GlobalFrameUniVec;
vector<SceneUniform*> GlobalSceneUniVec;

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

bool InitScene()
{
	printf("init scene start!\n");

	//texture
	mMaterialVolume.SetTexture(MAIN_TEXTURE_SLOT, &mTex);
	mMaterialStandard.SetTexture(MAIN_TEXTURE_SLOT, &mTex);
	mMaterialStandard.SetTexture(SHADOW_TEXTURE_SLOT, &mRTex);

	//drawable
	mDrawableVolume.ConnectObjectUniform(&mObjUniVolume);
	mDrawableVolume.ConnectFrameUniform(&mFrameUniform);
	mDrawableVolume.ConnectSceneUniform(&mSceneUniform);

	mDrawableAxis.ConnectObjectUniform(&mObjUniAxis);
	mDrawableAxis.ConnectFrameUniform(&mFrameUniform);
	mDrawableAxis.ConnectSceneUniform(&mSceneUniform);

	mDrawableGrid.ConnectObjectUniform(&mObjUniGrid);
	mDrawableGrid.ConnectFrameUniform(&mFrameUniform);
	mDrawableGrid.ConnectSceneUniform(&mSceneUniform);

	mDrawablePlane.ConnectObjectUniform(&mObjUniPlane);
	mDrawablePlane.ConnectFrameUniform(&mFrameUniform);
	mDrawablePlane.ConnectSceneUniform(&mSceneUniform);

	///////////////////////////////////////////////////////////////////
	grpDrawableVolume.AddDrawable(&mDrawableVolume);

	grpDrawableGizmo.AddDrawable(&mDrawableAxis);
	grpDrawableGizmo.AddDrawable(&mDrawableGrid);

	grpDrawableStandard.AddDrawable(&mDrawablePlane);

	///////////////////////////////////////////////////////////////////
	mFrameUniform.ApplyCol(1, 1, 1, 1);
	mFrameUniform.ApplyIntensity(5.f);
	mFrameUniform.ApplyFrameNum(mRenderer.frame);

	mSceneUniform.ApplyStep(50);

	///////////////////////////////////////////////////////////////////
	objVolume.SetTransform(&mTransformVolume);
	objVolume.SetDrawable(&mDrawableVolume);
	objVolume.ConnectObjectUniform(&mObjUniVolume);

	objAxis.SetTransform(&mTransformAxis);
	objAxis.SetDrawable(&mDrawableAxis);
	objAxis.ConnectObjectUniform(&mObjUniAxis);

	objGrid.SetTransform(&mTransformGrid);
	objGrid.SetDrawable(&mDrawableGrid);
	objGrid.ConnectObjectUniform(&mObjUniGrid);
	
	objPlane.SetTransform(&mTransformPlane);
	objPlane.SetDrawable(&mDrawablePlane);
	objPlane.ConnectObjectUniform(&mObjUniPlane);

	objCamera.SetCamera(&mCamera);
	objCamera.ConnectFrameUniform(&mFrameUniform);

	objLight.SetLight(&mLight);
	objLight.ConnectSceneUniform(&mSceneUniform);

	///////////////////////////////////////////////////////////////////
	GlobalObjVec.push_back(&objVolume);
	GlobalObjVec.push_back(&objGrid);
	GlobalObjVec.push_back(&objAxis);
	GlobalObjVec.push_back(&objPlane);
	GlobalObjVec.push_back(&objCamera);
	GlobalObjVec.push_back(&objLight);

	GlobalObjectUniVec.push_back(&mObjUniVolume);
	GlobalObjectUniVec.push_back(&mObjUniPlane);
	GlobalObjectUniVec.push_back(&mObjUniAxis);
	GlobalObjectUniVec.push_back(&mObjUniGrid);

	GlobalFrameUniVec.push_back(&mFrameUniform);
	
	GlobalSceneUniVec.push_back(&mSceneUniform);

	///////////////////////////////////////////////////////////////////
	for (auto item = GlobalObjVec.begin(); item != GlobalObjVec.end(); item++)
	{
		if (!(*item)->InitObject(mRenderer.d3d11Device, mRenderer.d3d11DevCon)) return false;
	}

	for (auto item = GlobalObjectUniVec.begin(); item != GlobalObjectUniVec.end(); item++)
	{
		if (!(*item)->InitObjectUniform(mRenderer.d3d11Device, mRenderer.d3d11DevCon)) return false;
	}

	for (auto item = GlobalFrameUniVec.begin(); item != GlobalFrameUniVec.end(); item++)
	{
		if (!(*item)->InitFrameUniform(mRenderer.d3d11Device, mRenderer.d3d11DevCon)) return false;
	}

	for (auto item = GlobalSceneUniVec.begin(); item != GlobalSceneUniVec.end(); item++)
	{
		if (!(*item)->InitSceneUniform(mRenderer.d3d11Device, mRenderer.d3d11DevCon)) return false;
	}

	///////////////////////////////////////////////////////////////////

	GlobalDrawableGrpVec.push_back(&grpDrawableVolume);
	GlobalDrawableGrpVec.push_back(&grpDrawableGizmo);
	GlobalDrawableGrpVec.push_back(&grpDrawableStandard);

	for (auto item = GlobalDrawableGrpVec.begin(); item != GlobalDrawableGrpVec.end(); item++)
	{
		if (!(*item)->InitDrawableGroup(mRenderer.d3d11Device)) return false;
	}

	printf("init scene finish!\n");
	return true;
}

void DrawScene()
{
	//Crucial
	mRenderer.SetRenderTarget(&mRTex);
	mRenderer.DrawGroups(GlobalDrawableGrpVec);
	mRenderer.SetDefaultRenderTarget();
	mRenderer.DrawGroups(GlobalDrawableGrpVec);
	//Present the backbuffer to the screen
	mRenderer.SwapChain->Present(0, 0);
}

void DetectInput()
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	//keyboard control
	if (keyboardState[DIK_ESCAPE] & 0x80)
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		mTransformVolume.position.y += 0.001;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		mTransformVolume.position.y -= 0.001;
	}
	if (keyboardState[DIK_A] & 0x80)
	{
		mTransformVolume.position.x -= 0.001;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		mTransformVolume.position.x += 0.001;
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
		XMFLOAT3 lightPosTemp = mLight.GetPos(); 
		lightPosTemp.y += 0.005;
		mLight.SetPos(lightPosTemp);
		mTransformAxis.position = lightPosTemp;
	}
	if (keyboardState[DIK_DOWN] & 0x80)
	{
		XMFLOAT3 lightPosTemp = mLight.GetPos();
		lightPosTemp.y -= 0.005;
		mLight.SetPos(lightPosTemp);
		mTransformAxis.position = lightPosTemp;
	}
	if (keyboardState[DIK_LEFT] & 0x80)
	{
		XMFLOAT3 lightPosTemp = mLight.GetPos();
		lightPosTemp.x -= 0.005;
		mLight.SetPos(lightPosTemp);
		mTransformAxis.position = lightPosTemp;
	}
	if (keyboardState[DIK_RIGHT] & 0x80)
	{
		XMFLOAT3 lightPosTemp = mLight.GetPos();
		lightPosTemp.x += 0.005;
		mLight.SetPos(lightPosTemp);
		mTransformAxis.position = lightPosTemp;
	}
	if (keyboardState[DIK_COMMA] & 0x80)
	{
		XMFLOAT3 lightPosTemp = mLight.GetPos();
		lightPosTemp.z -= 0.005;
		mLight.SetPos(lightPosTemp);
		mTransformAxis.position = lightPosTemp;
	}
	if (keyboardState[DIK_PERIOD] & 0x80)
	{
		XMFLOAT3 lightPosTemp = mLight.GetPos();
		lightPosTemp.z += 0.005;
		mLight.SetPos(lightPosTemp);
		mTransformAxis.position = lightPosTemp;
	}

	//mouse control
	if (keyboardState[DIK_LCONTROL] & 0x80)
	{
		if (mouseCurrState.lX != 0)
		{
			mTransformVolume.rotation.y += mouseCurrState.lX * 0.01;
		}
		if (mouseCurrState.lY != 0)
		{
			mTransformVolume.rotation.x += mouseCurrState.lY * 0.01;
			
		}
		if (mouseCurrState.lZ != 0)
		{
			mTransformVolume.rotation.z += mouseCurrState.lZ * 0.002;
		}
	}
	else
	{
		if (mouseCurrState.lX != 0)
		{
			mCamera.horizontalAngle += mouseCurrState.lX * 0.1;
		}
		if (mouseCurrState.lY != 0)
		{
			mCamera.verticalAngle += mouseCurrState.lY * 0.1;
			if (mCamera.verticalAngle > 90 - EPSILON) mCamera.verticalAngle = 89 - EPSILON;
			if (mCamera.verticalAngle < -90 + EPSILON) mCamera.verticalAngle = -89 + EPSILON;
		
		}
		if (mouseCurrState.lZ != 0)
		{
			mCamera.distance -= mouseCurrState.lZ * 0.01;
			if (mCamera.distance < 0 + EPSILON) mCamera.distance = 0.1 + EPSILON;
		}
	}

	mouseLastState = mouseCurrState;

	return;
}

void UpdateScene()
{
	mRenderer.frame++;

	for (auto item = GlobalObjVec.begin(); item != GlobalObjVec.end(); item++)
	{
		(*item)->UpdateObject(mRenderer.d3d11DevCon);
	}

	//upload to GPU
	for (auto item = GlobalObjectUniVec.begin(); item != GlobalObjectUniVec.end(); item++)
	{
		if((*item)->NeedToUpload()) (*item)->ObjectUniformBufferData(mRenderer.d3d11DevCon);
	}

	//upload to GPU
	for (auto item = GlobalFrameUniVec.begin(); item != GlobalFrameUniVec.end(); item++)
	{
		(*item)->ApplyFrameNum(mRenderer.frame);
		if ((*item)->NeedToUpload()) (*item)->FrameUniformBufferData(mRenderer.d3d11DevCon);
	}

	//upload to GPU
	for (auto item = GlobalSceneUniVec.begin(); item != GlobalSceneUniVec.end(); item++)
	{
		if ((*item)->NeedToUpload()) (*item)->SceneUniformBufferData(mRenderer.d3d11DevCon);
	}
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

void Release()
{
	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitConsole();

	if (!InitWindow(hInstance, nShowCmd, mRenderer.width, mRenderer.height, true))
	{
		MessageBox(0, "Window Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!mRenderer.InitRenderer(hwnd))    //Initialize Direct3D
	{
		MessageBox(0, "Direct3D Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitDirectInput(hInstance))	//Initialize input device
	{
		MessageBox(0, "Input Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitScene())    //Initialize our scene
	{
		MessageBox(0, "Level Initialization - Failed", "Error", MB_OK);
		return 0;
	}

	Messageloop();

	Release();

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
