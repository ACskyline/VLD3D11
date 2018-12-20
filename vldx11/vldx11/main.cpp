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
BYTE keyboardLastState[256];
LPDIRECTINPUT8 DirectInput;
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
XMVECTORF32 ClearColor = {0.f, 0.f, 0.f, 1.f};// { 0.3f, 0.3f, 0.8f, 1.f };

Renderer mRenderer(WIDTH, HEIGHT, ClearColor, 1.f, 0);
CubeMesh mMeshVolume;
AxisMesh mMeshAxis;
GridMesh mMeshGrid(50);
PlaneMesh mMeshDebugShadowMap;
ObjMesh mMeshFan(L"CoolingFan.obj");
PlaneMesh mMeshFloor;
Material mMaterialVolume(L"myVert.hlsl", L"myPixelCubeFog.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialFan(L"myVert.hlsl", L"myPixelHalfLambertPointLight.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialGizmo(L"myVert.hlsl", L"myPixel.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialDebugShadowMap(L"myVertUI.hlsl", L"myPixelDebugShadowMap.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialShadowPass(L"myVert.hlsl", L"myPixelShadowPass.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialStandard(L"myVert.hlsl", L"myPixelTexturePointLightShadow.hlsl", layout, ARRAYSIZE(layout));
Drawable mDrawableVolume(Drawable::DrawableType::TrianlgeList, &mMeshVolume, &mMaterialVolume);
Drawable mDrawableAxis(Drawable::DrawableType::LineList, &mMeshAxis, &mMaterialGizmo);
Drawable mDrawableGrid(Drawable::DrawableType::LineList, &mMeshGrid, &mMaterialGizmo);
Drawable mDrawableDebugShadowMap(Drawable::DrawableType::TrianlgeList, &mMeshDebugShadowMap, &mMaterialDebugShadowMap);
Drawable mDrawableFan(Drawable::DrawableType::TrianlgeList, &mMeshFan, &mMaterialFan);
Drawable mDrawableFloor(Drawable::DrawableType::TrianlgeList, &mMeshFloor, &mMaterialStandard);
Transform mTransformVolume(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(10, 10, 10));
Transform mTransformAxis(XMFLOAT3(0, 10, -10), XMFLOAT3(90, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformGrid(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformDebugShadowMap(XMFLOAT3(-0.75f, 0.75f, 0.1f), XMFLOAT3(-90, 0, 0), XMFLOAT3(0.5, 1, 0.5));
Transform mTransformFan(XMFLOAT3(0, 0, -10), XMFLOAT3(90, 0, 0), XMFLOAT3(3, 3, 3));
Transform mTransformFloor(XMFLOAT3(0, -10, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(50, 50, 50));
OrbitCamera mCamera(Camera::CameraType::Perspective, 10.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH / (float)HEIGHT, 0.1f, 100.0f, 0, 0);
Camera mCameraLight(Camera::CameraType::Perspective, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH / (float)HEIGHT, 0.1f, 100.0f, 0, 0);
PointLight mLight(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 1, 1, 1), 50);
Texture mTex(Texture::TextureType::Default, L"checkerboard.jpg");
RenderTexture mRTex(RenderTexture::RenderTextureType::ShadowMap, 800, 600);
ObjectUniform mObjUniVolume;
ObjectUniform mObjUniAxis;
ObjectUniform mObjUniGrid;
ObjectUniform mObjUniDebugShadowMap;
ObjectUniform mObjUniFan;
ObjectUniform mObjUniFloor;
FrameUniform mFrameUniform;
FrameUniform mFrameUniformLight;
SceneUniform mSceneUniform;
Object objVolume;
Object objLight;
Object objCamera;
Object objGrid;
Object objDebugShadowMap;
Object objFan;
Object objFloor;
DrawableGroup grpDrawableVolume(DrawableGroup::DrawableGroupType::VolumeLight);
DrawableGroup grpDrawableGizmo(DrawableGroup::DrawableGroupType::Gizmo);
DrawableGroup grpDrawableStandard(DrawableGroup::DrawableGroupType::Standard);
DrawableGroup grpDrawableUI(DrawableGroup::DrawableGroupType::UI);

vector<Object*> GlobalObjVec; 
vector<DrawableGroup*> GlobalDrawableGrpVecShadowPass;
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

	//materials
	///////////////////////////////////////////////////////////////////
	//this material needs to be initialized explictly for it is not attached to a drawable here
	if (!mMaterialShadowPass.InitMaterial(mRenderer.d3d11Device)) return false;
	//these materials don't need to be initialized explictly for they are going to be initialized with the drawable
	mMaterialDebugShadowMap.SetTexture(SHADOW_TEXTURE_SLOT, &mRTex);
	mMaterialStandard.SetTexture(MAIN_TEXTURE_SLOT, &mTex);
	mMaterialStandard.SetTexture(SHADOW_TEXTURE_SLOT, &mRTex);

	//drawables
	///////////////////////////////////////////////////////////////////
	mDrawableVolume.ConnectObjectUniformRead(&mObjUniVolume);
	mDrawableVolume.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableVolume.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableAxis.ConnectObjectUniformRead(&mObjUniAxis);
	mDrawableAxis.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableAxis.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableGrid.ConnectObjectUniformRead(&mObjUniGrid);
	mDrawableGrid.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableGrid.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableDebugShadowMap.ConnectObjectUniformRead(&mObjUniDebugShadowMap);
	mDrawableDebugShadowMap.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableDebugShadowMap.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableFan.ConnectObjectUniformRead(&mObjUniFan);
	mDrawableFan.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableFan.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableFloor.ConnectObjectUniformRead(&mObjUniFloor);
	mDrawableFloor.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableFloor.ConnectSceneUniformRead(&mSceneUniform);

	//drawable groups
	///////////////////////////////////////////////////////////////////
	grpDrawableVolume.AddDrawable(&mDrawableVolume);
	grpDrawableGizmo.AddDrawable(&mDrawableAxis);
	grpDrawableGizmo.AddDrawable(&mDrawableGrid);
	grpDrawableUI.AddDrawable(&mDrawableDebugShadowMap);
	grpDrawableStandard.AddDrawable(&mDrawableFan);
	grpDrawableStandard.AddDrawable(&mDrawableFloor);

	//uniforms
	///////////////////////////////////////////////////////////////////
	mFrameUniform.ApplyCol(1, 1, 1, 1);
	mFrameUniform.ApplyIntensity(5.f);
	mFrameUniform.ApplyFrameNum(mRenderer.frame);
	mFrameUniformLight.ApplyCol(1, 0, 0, 1);
	mFrameUniformLight.ApplyIntensity(5.f);
	mFrameUniformLight.ApplyFrameNum(mRenderer.frame);
	mSceneUniform.ApplyStep(50);

	//objects
	///////////////////////////////////////////////////////////////////
	objVolume.SetTransform(&mTransformVolume);
	objVolume.SetDrawable(&mDrawableVolume);
	objVolume.ConnectObjectUniformWrite(&mObjUniVolume);
	objGrid.SetTransform(&mTransformGrid);
	objGrid.SetDrawable(&mDrawableGrid);
	objGrid.ConnectObjectUniformWrite(&mObjUniGrid);
	objDebugShadowMap.SetTransform(&mTransformDebugShadowMap);
	objDebugShadowMap.SetDrawable(&mDrawableDebugShadowMap);
	objDebugShadowMap.ConnectObjectUniformWrite(&mObjUniDebugShadowMap);
	objFan.SetTransform(&mTransformFan);
	objFan.SetDrawable(&mDrawableFan);
	objFan.ConnectObjectUniformWrite(&mObjUniFan);
	objFloor.SetTransform(&mTransformFloor);
	objFloor.SetDrawable(&mDrawableFloor);
	objFloor.ConnectObjectUniformWrite(&mObjUniFloor);
	objCamera.SetCamera(&mCamera);
	objCamera.SetCameraShadow(&mCameraLight);
	objCamera.SetTransformShadow(&mTransformAxis);
	objCamera.ConnectFrameUniformWrite(&mFrameUniform);
	objLight.SetCamera(&mCameraLight);
	objLight.ConnectFrameUniformWrite(&mFrameUniformLight);
	objLight.SetLight(&mLight);
	objLight.ConnectSceneUniformWrite(&mSceneUniform);
	objLight.SetTransform(&mTransformAxis);
	objLight.SetDrawable(&mDrawableAxis);
	objLight.ConnectObjectUniformWrite(&mObjUniAxis);

	//init object vectors
	///////////////////////////////////////////////////////////////////
	GlobalObjVec.push_back(&objVolume);
	GlobalObjVec.push_back(&objGrid);
	GlobalObjVec.push_back(&objDebugShadowMap);
	GlobalObjVec.push_back(&objCamera);
	GlobalObjVec.push_back(&objLight);
	GlobalObjVec.push_back(&objFan);
	GlobalObjVec.push_back(&objFloor);
	GlobalObjectUniVec.push_back(&mObjUniVolume);
	GlobalObjectUniVec.push_back(&mObjUniDebugShadowMap);
	GlobalObjectUniVec.push_back(&mObjUniAxis);
	GlobalObjectUniVec.push_back(&mObjUniGrid);
	GlobalObjectUniVec.push_back(&mObjUniFan);
	GlobalObjectUniVec.push_back(&mObjUniFloor);
	GlobalFrameUniVec.push_back(&mFrameUniform);
	GlobalFrameUniVec.push_back(&mFrameUniformLight);
	GlobalSceneUniVec.push_back(&mSceneUniform);

	//init drawable group vectors
	///////////////////////////////////////////////////////////////////
	// Second pass drawable groups
	GlobalDrawableGrpVec.push_back(&grpDrawableVolume);
	GlobalDrawableGrpVec.push_back(&grpDrawableGizmo);
	GlobalDrawableGrpVec.push_back(&grpDrawableStandard);
	GlobalDrawableGrpVec.push_back(&grpDrawableUI);
	// Shadow pass drawable groups
	GlobalDrawableGrpVecShadowPass.push_back(&grpDrawableVolume);
	GlobalDrawableGrpVecShadowPass.push_back(&grpDrawableStandard);

	//init objects in object vectors
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

	//init drawable groups in drawable group vectors
	///////////////////////////////////////////////////////////////////
	// Second pass drawable groups
	for (auto item = GlobalDrawableGrpVec.begin(); item != GlobalDrawableGrpVec.end(); item++)
	{
		if (!(*item)->InitDrawableGroup(mRenderer.d3d11Device)) return false;
	}
	// Shadow pass drawable groups
	for (auto item = GlobalDrawableGrpVecShadowPass.begin(); item != GlobalDrawableGrpVecShadowPass.end(); item++)
	{
		if (!(*item)->InitDrawableGroup(mRenderer.d3d11Device)) return false;
	}

	printf("init scene finish!\n");
	return true;
}

void DrawScene()
{
	//Crucial
	//shadow pass
	mRenderer.SetRenderTarget(&mRTex);
	mRenderer.ClearCurrentRenderTarget({1.f}, 1.f, 0);//since I know this render target is DXGI_FORMAT_R16_FLOAT, I can supply only the red channel
	//mRenderer.DrawGroups(GlobalDrawableGrpVecShadowPass, &mMaterialShadowPass);
	mRenderer.DrawGroups(GlobalDrawableGrpVecShadowPass, &mMaterialShadowPass, &mFrameUniformLight);
	//second pass
	mRenderer.SetDefaultRenderTarget();
	mRenderer.ClearCurrentRenderTargetDefault();
	mRenderer.DrawGroups(GlobalDrawableGrpVec);
	//Present the backbuffer to the screen
	mRenderer.SwapChain->Present(0, 0);
}

void DetectInput()
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardCurrState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardCurrState), (LPVOID)&keyboardCurrState);

	//keyboard control
	if (KEYDOWN(keyboardCurrState, DIK_ESCAPE))
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
	if (KEYDOWN(keyboardCurrState, DIK_W))
	{
		mTransformVolume.position.y += 0.001;
	}
	if (KEYDOWN(keyboardCurrState, DIK_S))
	{
		mTransformVolume.position.y -= 0.001;
	}
	if (KEYDOWN(keyboardCurrState, DIK_A))
	{
		mTransformVolume.position.x -= 0.001;
	}
	if (KEYDOWN(keyboardCurrState, DIK_D))
	{
		mTransformVolume.position.x += 0.001;
	}
	if (KEYDOWN(keyboardCurrState, DIK_Q))
	{
		mTransformVolume.position.z -= 0.0005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_E))
	{
		mTransformVolume.position.z += 0.0005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_UP))
	{
		mTransformAxis.position.y += 0.005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_DOWN))
	{
		mTransformAxis.position.y -= 0.005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_LEFT))
	{
		mTransformAxis.position.x -= 0.005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_RIGHT))
	{
		mTransformAxis.position.x += 0.005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_COMMA))
	{
		mTransformAxis.position.z -= 0.005;
	}
	if (KEYDOWN(keyboardCurrState, DIK_PERIOD))
	{
		mTransformAxis.position.z += 0.005;
	}

	//mouse control
	if (KEYDOWN(keyboardCurrState, DIK_Z))//control volume
	{
		if (mouseCurrState.lX != 0)
		{
			mTransformVolume.rotation.y += mouseCurrState.lX * 0.1;
		}
		if (mouseCurrState.lY != 0)
		{
			mTransformVolume.rotation.x += mouseCurrState.lY * 0.1;
			
		}
		if (mouseCurrState.rgbButtons[0] > 0)//MLB
		{
			mTransformVolume.rotation.z += 0.1;
		}
	}
	else if (KEYDOWN(keyboardCurrState, DIK_X))//control light
	{
		if (mouseCurrState.lX != 0)
		{
			mTransformAxis.rotation.y += mouseCurrState.lX * 0.1;
		}
		if (mouseCurrState.lY != 0)
		{
			mTransformAxis.rotation.x += mouseCurrState.lY * 0.1;

		}
		if (mouseCurrState.rgbButtons[0] > 0)//MLB
		{
			mTransformAxis.rotation.z += 0.1;
		}
	}
	else if (KEYDOWN(keyboardCurrState, DIK_C))//control camera
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
	memcpy(keyboardLastState, keyboardCurrState, 256 * sizeof(BYTE));

	return;
}

void UpdateScene()
{
	mRenderer.frame++;

	//update object and set needToUpdate flag
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
