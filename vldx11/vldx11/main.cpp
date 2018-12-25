#include "Renderer.h"
#include "Object.h"
#include "Scene.h"

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
const uint32_t WIDTH_LIGHT = 1024;
const uint32_t HEIGHT_LIGHT = 1024;
XMVECTORF32 ClearColor = {0.f, 0.f, 0.f, 1.f};// { 0.3f, 0.3f, 0.8f, 1.f };

//renderer
Renderer mRenderer(WIDTH, HEIGHT, ClearColor, 1.f, 0);

//data
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
Transform mTransformVolume(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(10, 10, 10));
Transform mTransformAxis(XMFLOAT3(0, 20, 0), XMFLOAT3(90, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformGrid(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformDebugShadowMap(XMFLOAT3(-0.75f, 0.75f, 0.1f), XMFLOAT3(-90, 0, 0), XMFLOAT3(0.5, 1, 0.5));
Transform mTransformFan(XMFLOAT3(0, 10, 0), XMFLOAT3(90, 0, 0), XMFLOAT3(3, 3, 3));
Transform mTransformFloor(XMFLOAT3(0, -10, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(50, 50, 50));
OrbitCamera mCamera(Camera::CameraType::Perspective, 10.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH / (float)HEIGHT, 0.1f, 100.0f, 0, 0);
Camera mCameraLight(Camera::CameraType::Perspective, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH_LIGHT / (float)HEIGHT_LIGHT, 0.1f, 100.0f, 0, 0);
PointLight mLight(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 1, 1, 1), 50);
Texture mTex(Texture::TextureType::Default, L"checkerboard.jpg");
RenderTexture mRTexLightDepth(RenderTexture::RenderTextureType::ShadowMap, WIDTH_LIGHT, HEIGHT_LIGHT);
RenderTexture mRTexLightDepthPCF(RenderTexture::RenderTextureType::ShadowMapPCF, WIDTH_LIGHT, HEIGHT_LIGHT);
RenderTexture mRTexScreenDepth(RenderTexture::RenderTextureType::ShadowMap, WIDTH, HEIGHT);
RenderTexture mRTexScreenDepthPCF(RenderTexture::RenderTextureType::ShadowMapPCF, WIDTH, HEIGHT);
ObjectUniform mObjUniVolume;
ObjectUniform mObjUniAxis;
ObjectUniform mObjUniGrid;
ObjectUniform mObjUniDebugShadowMap;
ObjectUniform mObjUniFan;
ObjectUniform mObjUniFloor;
FrameUniform mFrameUniform;
FrameUniform mFrameUniformLight;
SceneUniform mSceneUniform;

//data holder
Scene mScene;
Object objVolume;
Object objLight;
Object objCamera;
Object objGrid;
Object objDebugShadowMap;
Object objFan;
Object objFloor; 
Drawable mDrawableVolume(Drawable::DrawableType::TrianlgeList, &mMeshVolume, &mMaterialVolume);
Drawable mDrawableAxis(Drawable::DrawableType::LineList, &mMeshAxis, &mMaterialGizmo);
Drawable mDrawableGrid(Drawable::DrawableType::LineList, &mMeshGrid, &mMaterialGizmo);
Drawable mDrawableDebugShadowMap(Drawable::DrawableType::TrianlgeList, &mMeshDebugShadowMap, &mMaterialDebugShadowMap);
Drawable mDrawableFan(Drawable::DrawableType::TrianlgeList, &mMeshFan, &mMaterialFan);
Drawable mDrawableFloor(Drawable::DrawableType::TrianlgeList, &mMeshFloor, &mMaterialStandard);
DrawableGroup grpDrawableVolume(DrawableGroup::DrawableGroupType::VolumeLight);
DrawableGroup grpDrawableGizmo(DrawableGroup::DrawableGroupType::Gizmo);
DrawableGroup grpDrawableStandard(DrawableGroup::DrawableGroupType::Standard);
DrawableGroup grpDrawableUI(DrawableGroup::DrawableGroupType::UI);
vector<DrawableGroup*> GlobalDrawableGrpVecShadowPass;
vector<DrawableGroup*> GlobalDrawableGrpVec;

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

bool CreateScene()
{
	printf("create scene start!\n");

	//>>>>>0. Mesh<<<<<//
	mScene.AddMesh(&mMeshVolume);
	mScene.AddMesh(&mMeshAxis);
	mScene.AddMesh(&mMeshDebugShadowMap);
	mScene.AddMesh(&mMeshFan);
	mScene.AddMesh(&mMeshFloor);
	mScene.AddMesh(&mMeshGrid);

	//>>>>>I. Texture & Material<<<<<//
	//bind texture to material
	mMaterialDebugShadowMap.SetTexture(TEXTURE_SLOT::SHADOW, &mRTexScreenDepth); //&mRTexScreenDepth);
	mMaterialDebugShadowMap.SetTexture(TEXTURE_SLOT::SHADOW_PCF, &mRTexScreenDepthPCF); //&mRTexScreenDepthPCF);
	mMaterialVolume.SetTexture(TEXTURE_SLOT::SHADOW, &mRTexLightDepth);
	mMaterialVolume.SetTexture(TEXTURE_SLOT::SHADOW_PCF, &mRTexLightDepthPCF);
	mMaterialStandard.SetTexture(TEXTURE_SLOT::MAIN, &mTex);
	mMaterialStandard.SetTexture(TEXTURE_SLOT::SHADOW, &mRTexLightDepth);
	mMaterialStandard.SetTexture(TEXTURE_SLOT::SHADOW_PCF, &mRTexLightDepthPCF);
	//add texture to scene
	mScene.AddTexture(&mTex);
	mScene.AddTexture(&mRTexLightDepth);
	mScene.AddTexture(&mRTexLightDepthPCF);
	mScene.AddTexture(&mRTexScreenDepth);
	mScene.AddTexture(&mRTexScreenDepthPCF);
	//add material to scene
	mScene.AddMaterial(&mMaterialDebugShadowMap);
	mScene.AddMaterial(&mMaterialVolume);
	mScene.AddMaterial(&mMaterialStandard);
	mScene.AddMaterial(&mMaterialFan);
	mScene.AddMaterial(&mMaterialGizmo);
	mScene.AddMaterial(&mMaterialShadowPass);

	//>>>>>II. Drawable<<<<<//
	//bind uniform to drawable
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
	//add drawable to scene
	mScene.AddDrawable(&mDrawableVolume);
	mScene.AddDrawable(&mDrawableAxis);
	mScene.AddDrawable(&mDrawableGrid);
	mScene.AddDrawable(&mDrawableDebugShadowMap);
	mScene.AddDrawable(&mDrawableFan);
	mScene.AddDrawable(&mDrawableFloor);

	//>>>>>III. Drawable Group<<<<<//
	//add drawable to drawable group
	grpDrawableVolume.AddDrawable(&mDrawableVolume);
	grpDrawableGizmo.AddDrawable(&mDrawableAxis);
	grpDrawableGizmo.AddDrawable(&mDrawableGrid);
	grpDrawableUI.AddDrawable(&mDrawableDebugShadowMap);
	grpDrawableStandard.AddDrawable(&mDrawableFan);
	grpDrawableStandard.AddDrawable(&mDrawableFloor);
	//add drawable group to scene
	mScene.AddDrawableGroup(&grpDrawableStandard);
	mScene.AddDrawableGroup(&grpDrawableGizmo);
	mScene.AddDrawableGroup(&grpDrawableVolume);
	mScene.AddDrawableGroup(&grpDrawableUI);

	//>>>>>IV. Uniform<<<<<//
	//apply uniform
	mFrameUniform.ApplyCol(1, 1, 1, 1);
	mFrameUniform.ApplyIntensity(5.f);
	mFrameUniform.ApplyFrameNum(mRenderer.frame);
	mFrameUniformLight.ApplyCol(0, 0, 0, 0);
	mFrameUniformLight.ApplyIntensity(0.f);
	mFrameUniformLight.ApplyFrameNum(mRenderer.frame);
	mSceneUniform.ApplyStep(50);
	//add uniform to scene
	mScene.AddObjectUniform(&mObjUniAxis);
	mScene.AddObjectUniform(&mObjUniDebugShadowMap);
	mScene.AddObjectUniform(&mObjUniFan);
	mScene.AddObjectUniform(&mObjUniFloor);
	mScene.AddObjectUniform(&mObjUniGrid);
	mScene.AddObjectUniform(&mObjUniVolume);
	mScene.AddFrameUniform(&mFrameUniform);
	mScene.AddFrameUniform(&mFrameUniformLight);
	mScene.AddSceneUniform(&mSceneUniform);

	//>>>>>V. Object<<<<<//
	//connect object
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
	//add object to scene
	mScene.AddObject(&objVolume);
	mScene.AddObject(&objGrid);
	mScene.AddObject(&objDebugShadowMap);
	mScene.AddObject(&objFan);
	mScene.AddObject(&objFloor);
	mScene.AddObject(&objCamera);
	mScene.AddObject(&objLight);

	//init drawable group vectors
	//second pass drawable groups
	GlobalDrawableGrpVec.push_back(&grpDrawableStandard);
	GlobalDrawableGrpVec.push_back(&grpDrawableVolume);
	GlobalDrawableGrpVec.push_back(&grpDrawableGizmo);
	GlobalDrawableGrpVec.push_back(&grpDrawableUI);
	//shadow pass drawable groups
	GlobalDrawableGrpVecShadowPass.push_back(&grpDrawableStandard);

	printf("create scene finish!\n");
	return true;
}

bool InitScene()
{
	printf("init scene start!\n");
	if (!mScene.InitScene(mRenderer.d3d11Device, mRenderer.d3d11DevCon))
		return false;
	mScene.UpdateSceneAll(mRenderer.d3d11DevCon);
	printf("init scene finish!\n");
	return true;
}

void DrawScene()
{
	//light shadow pass
	mRenderer.SetRenderTarget(&mRTexLightDepth);
	mRenderer.ClearCurrentRenderTarget({ 1.f }, 1.f, 0);//since I know this render target is DXGI_FORMAT_R16_FLOAT, I can supply only the red channel
	mRenderer.DrawGroups(GlobalDrawableGrpVecShadowPass, &mMaterialShadowPass, &mFrameUniformLight);
	//screen shadow pass
	//TODO: mRTexScreenDepth is not bond to a material which is bond to a drawable, so it is not initialized.
	//if set it to the render target, it wouldn't do anything. So the render target is still the mRTexLightDepth.
	//This is why the result is incorrect. The best way to solve this is to utilize the scene class as a way to 
	//initialize all the data.
	mRenderer.SetRenderTarget(&mRTexScreenDepth);
	mRenderer.ClearCurrentRenderTarget({ 1.f }, 1.f, 0);//since I know this render target is DXGI_FORMAT_R16_FLOAT, I can supply only the red channel
	mRenderer.DrawGroups(GlobalDrawableGrpVecShadowPass, &mMaterialShadowPass, &mFrameUniform);
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

	bool updateVolume = false;
	if (KEYDOWN(keyboardCurrState, DIK_W))
	{
		mTransformVolume.position.y += 0.001;
		updateVolume = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_S))
	{
		mTransformVolume.position.y -= 0.001;
		updateVolume = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_A))
	{
		mTransformVolume.position.x -= 0.001;
		updateVolume = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_D))
	{
		mTransformVolume.position.x += 0.001;
		updateVolume = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_Q))
	{
		mTransformVolume.position.z -= 0.0005;
		updateVolume = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_E))
	{
		mTransformVolume.position.z += 0.0005;
		updateVolume = true;
	}

	bool updateLight = false;
	if (KEYDOWN(keyboardCurrState, DIK_UP))
	{
		mTransformAxis.position.y += 0.005;
		updateLight = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_DOWN))
	{
		mTransformAxis.position.y -= 0.005;
		updateLight = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_LEFT))
	{
		mTransformAxis.position.x -= 0.005;
		updateLight = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_RIGHT))
	{
		mTransformAxis.position.x += 0.005;
		updateLight = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_COMMA))
	{
		mTransformAxis.position.z -= 0.005;
		updateLight = true;
	}
	if (KEYDOWN(keyboardCurrState, DIK_PERIOD))
	{
		mTransformAxis.position.z += 0.005;
		updateLight = true;
	}

	//mouse control
	if (KEYDOWN(keyboardCurrState, DIK_Z))//control volume
	{
		if (mouseCurrState.lX != 0)
		{
			mTransformVolume.rotation.y += mouseCurrState.lX * 0.1;
			updateVolume = true;
		}
		if (mouseCurrState.lY != 0)
		{
			mTransformVolume.rotation.x += mouseCurrState.lY * 0.1;
			updateVolume = true;
		}
		if (mouseCurrState.rgbButtons[0] > 0)//MLB
		{
			mTransformVolume.rotation.z += 0.1;
			updateVolume = true;
		}
	}
	else if (KEYDOWN(keyboardCurrState, DIK_X))//control light
	{
		if (mouseCurrState.lX != 0)
		{
			mTransformAxis.rotation.y += mouseCurrState.lX * 0.1;
			updateLight = true;
		}
		if (mouseCurrState.lY != 0)
		{
			mTransformAxis.rotation.x += mouseCurrState.lY * 0.1;
			updateLight = true;
		}
		if (mouseCurrState.rgbButtons[0] > 0)//MLB
		{
			mTransformAxis.rotation.z += 0.1;
			updateLight = true;
		}
	}
	else if (KEYDOWN(keyboardCurrState, DIK_C))//control camera
	{
		bool updateCamera = false;
		if (mouseCurrState.lX != 0)
		{
			mCamera.horizontalAngle += mouseCurrState.lX * 0.1;
			updateCamera = true;
		}
		if (mouseCurrState.lY != 0)
		{
			mCamera.verticalAngle += mouseCurrState.lY * 0.1;
			if (mCamera.verticalAngle > 90 - EPSILON) mCamera.verticalAngle = 89 - EPSILON;
			if (mCamera.verticalAngle < -90 + EPSILON) mCamera.verticalAngle = -89 + EPSILON;
			updateCamera = true;
		}
		if (mouseCurrState.lZ != 0)
		{
			mCamera.distance -= mouseCurrState.lZ * 0.01;
			if (mCamera.distance < 0 + EPSILON) mCamera.distance = 0.1 + EPSILON;
			updateCamera = true;
		}
		if (updateCamera)
		{
			objCamera.UpdateObject();
		}
	}

	if (updateLight)
	{
		objLight.UpdateObject();
		objCamera.UpdateObject();//for ApplyCameraShadow(...)
	}

	if (updateVolume)
	{
		objVolume.UpdateObject();
	}

	mouseLastState = mouseCurrState;
	std::memcpy(keyboardLastState, keyboardCurrState, 256 * sizeof(BYTE));
}

void UpdateScene()
{
	mRenderer.frame++;
	mFrameUniform.ApplyFrameNum(mRenderer.frame);
	mFrameUniformLight.ApplyFrameNum(mRenderer.frame);
	mScene.UpdateSceneCheck(mRenderer.d3d11DevCon);
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

	if (!CreateScene())    //Create our scene
	{
		MessageBox(0, "Scene Creation - Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitScene())    //Initialize our scene
	{
		MessageBox(0, "Scene Initialization - Failed", "Error", MB_OK);
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
