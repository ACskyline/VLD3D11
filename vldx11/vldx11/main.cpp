#include "Renderer.h"
#include "Object.h"
#include "Scene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "MYPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYNORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC layoutColor[] =
{
	{ "MYPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYNORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MYUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//window stuff
LPCTSTR WndClassName = "vldx11";
HWND hwnd = NULL;
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;
DIMOUSESTATE mouseLastState;
BYTE keyboardLastState[256];
LPDIRECTINPUT8 DirectInput;
//mystuff
const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 768;
const uint32_t WIDTH_LIGHT = 1024;
const uint32_t HEIGHT_LIGHT = 1024;
XMVECTORF32 ClearColor = {0.f, 0.f, 0.f, 1.f};// { 0.3f, 0.3f, 0.8f, 1.f };
//imgui stuff
bool mouseAcquired = false;
float deltaTime = 0.f;
float fanAngularVel = 60.f;

//renderer
Renderer mRenderer(WIDTH, HEIGHT, ClearColor, 1.f, 0);

//data
//ConeMesh mMeshVolume(32);
CubeMesh mMeshVolume;
//SphereMesh mMeshVolume(16, 16);
AxisMesh mMeshAxis;
GridMesh mMeshGrid(50);
PlaneMesh mMeshDebugShadowMap;
ObjMesh mMeshFanBlade(L"fan_blade.obj");
ObjMesh mMeshFanBase(L"fan_base.obj");
PlaneMesh mMeshFloor;
//Material mMaterialVolume(L"myVert.hlsl", L"myPixelConeFogSpotLight.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialVolume(L"myVert.hlsl", L"myPixelCubeFogSpotLight.hlsl", layout, ARRAYSIZE(layout));
//Material mMaterialVolume(L"myVert.hlsl", L"myPixelSphereFogSpotLight.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialFan(L"myVert.hlsl", L"myPixelHalfLambertSpotLight.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialGizmo(L"myVertColor.hlsl", L"myPixelColor.hlsl", layoutColor, ARRAYSIZE(layoutColor));
Material mMaterialDebugShadowMap(L"myVertUI.hlsl", L"myPixelDebugShadowMap.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialShadowPass(L"myVert.hlsl", L"myPixelShadowPass.hlsl", layout, ARRAYSIZE(layout));
Material mMaterialStandard(L"myVert.hlsl", L"myPixelTextureSpotLightShadow.hlsl", layout, ARRAYSIZE(layout));
Transform mTransformVolume(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(30, 30, 30));
Transform mTransformAxis(XMFLOAT3(0, 20, 0), XMFLOAT3(90, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformGrid(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
Transform mTransformDebugShadowMap(XMFLOAT3(-0.75f, 0.75f, 0.1f), XMFLOAT3(-90, 0, 0), XMFLOAT3(0.5, 1, 0.5));
Transform mTransformFanBase(XMFLOAT3(0, 10, 0), XMFLOAT3(90, 0, 0), XMFLOAT3(3, 3, 3));
Transform mTransformFanBlade(XMFLOAT3(0, 10, 0), XMFLOAT3(90, 0, 0), XMFLOAT3(3, 3, 3));
Transform mTransformFloor(XMFLOAT3(0, -10, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(50, 50, 50));
OrbitCamera mCamera(Camera::CameraType::Perspective, 10.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH / (float)HEIGHT, 0.1f, 100.0f, 0, 0);
Camera mCameraLight(Camera::CameraType::Perspective, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), 90, WIDTH_LIGHT / (float)HEIGHT_LIGHT, 0.1f, 100.0f, 0, 0);
//PointLight mLight(XMFLOAT4(1, 1, 1, 1), XMFLOAT3(0, 0, 0), 50);
SpotLight mLight(XMFLOAT4(1, 1, 1, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), 50, 90);
Texture mTex(Texture::TextureType::Default, L"checkerboard.jpg");
RenderTexture mRTexLightDepth(RenderTexture::RenderTextureType::ShadowMap, WIDTH_LIGHT, HEIGHT_LIGHT);
RenderTexture mRTexLightDepthPCF(RenderTexture::RenderTextureType::ShadowMapPCF, WIDTH_LIGHT, HEIGHT_LIGHT);
RenderTexture mRTexScreenDepth(RenderTexture::RenderTextureType::ShadowMap, WIDTH, HEIGHT);
RenderTexture mRTexScreenDepthPCF(RenderTexture::RenderTextureType::ShadowMapPCF, WIDTH, HEIGHT);
ObjectUniform mObjUniVolume;
ObjectUniform mObjUniAxis;
ObjectUniform mObjUniGrid;
ObjectUniform mObjUniDebugShadowMap;
ObjectUniform mObjUniFanBase;
ObjectUniform mObjUniFanBlade;
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
Object objFanBase;
Object objFanBlade;
Object objFloor; 
Drawable mDrawableVolume(Drawable::DrawableType::TrianlgeList, &mMeshVolume, &mMaterialVolume);
Drawable mDrawableAxis(Drawable::DrawableType::LineList, &mMeshAxis, &mMaterialGizmo);
Drawable mDrawableGrid(Drawable::DrawableType::LineList, &mMeshGrid, &mMaterialGizmo);
Drawable mDrawableDebugShadowMap(Drawable::DrawableType::TrianlgeList, &mMeshDebugShadowMap, &mMaterialDebugShadowMap);
Drawable mDrawableFanBase(Drawable::DrawableType::TrianlgeList, &mMeshFanBase, &mMaterialFan);
Drawable mDrawableFanBlade(Drawable::DrawableType::TrianlgeList, &mMeshFanBlade, &mMaterialFan);
Drawable mDrawableFloor(Drawable::DrawableType::TrianlgeList, &mMeshFloor, &mMaterialStandard);
DrawableGroup grpDrawableVolume(DrawableGroup::DrawableGroupType::VolumeLight);
DrawableGroup grpDrawableGizmo(DrawableGroup::DrawableGroupType::Gizmo);
DrawableGroup grpDrawableStandard(DrawableGroup::DrawableGroupType::Standard);
DrawableGroup grpDrawableDebug(DrawableGroup::DrawableGroupType::Debug);
vector<DrawableGroup*> GlobalDrawableGrpVecShadowPass;
vector<DrawableGroup*> GlobalDrawableGrpVec;

void InitConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* stream;
	freopen_s(&stream, "CON", "w", stdout);
}

bool InitGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Platform/Renderer bindings
	if(!ImGui_ImplWin32_Init(hwnd)) return false;
	if(!ImGui_ImplDX11_Init(mRenderer.d3d11Device, mRenderer.d3d11DevCon)) return false;

	// Setup Style
	ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

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
	mScene.AddMesh(&mMeshFanBase);
	mScene.AddMesh(&mMeshFanBlade);
	mScene.AddMesh(&mMeshFloor);
	mScene.AddMesh(&mMeshGrid);

	//>>>>>I. Texture & Material<<<<<//
	//bind texture to material
	mMaterialDebugShadowMap.SetTexture(TEXTURE_SLOT::SHADOW, &mRTexScreenDepth); //&mRTexScreenDepth);
	mMaterialDebugShadowMap.SetTexture(TEXTURE_SLOT::SHADOW_PCF, &mRTexScreenDepthPCF); //&mRTexScreenDepthPCF);
	mMaterialVolume.SetTexture(TEXTURE_SLOT::SHADOW, &mRTexLightDepth);
	mMaterialVolume.SetTexture(TEXTURE_SLOT::SHADOW_PCF, &mRTexLightDepthPCF);
	mMaterialVolume.SetTexture(TEXTURE_SLOT::SCREEN_DEPTH, &mRTexScreenDepth);
	mMaterialVolume.SetTexture(TEXTURE_SLOT::SCREEN_DEPTH_PCF, &mRTexScreenDepthPCF);
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
	mDrawableFanBase.ConnectObjectUniformRead(&mObjUniFanBase);
	mDrawableFanBase.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableFanBase.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableFanBlade.ConnectObjectUniformRead(&mObjUniFanBlade);
	mDrawableFanBlade.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableFanBlade.ConnectSceneUniformRead(&mSceneUniform);
	mDrawableFloor.ConnectObjectUniformRead(&mObjUniFloor);
	mDrawableFloor.ConnectFrameUniformRead(&mFrameUniform);
	mDrawableFloor.ConnectSceneUniformRead(&mSceneUniform);
	//add drawable to scene
	mScene.AddDrawable(&mDrawableVolume);
	mScene.AddDrawable(&mDrawableAxis);
	mScene.AddDrawable(&mDrawableGrid);
	mScene.AddDrawable(&mDrawableDebugShadowMap);
	mScene.AddDrawable(&mDrawableFanBase);
	mScene.AddDrawable(&mDrawableFanBlade);
	mScene.AddDrawable(&mDrawableFloor);

	//>>>>>III. Drawable Group<<<<<//
	//add drawable to drawable group
	grpDrawableVolume.AddDrawable(&mDrawableVolume);
	grpDrawableGizmo.AddDrawable(&mDrawableAxis);
	grpDrawableGizmo.AddDrawable(&mDrawableGrid);
	grpDrawableDebug.AddDrawable(&mDrawableDebugShadowMap);
	grpDrawableStandard.AddDrawable(&mDrawableFanBase);
	grpDrawableStandard.AddDrawable(&mDrawableFanBlade);
	grpDrawableStandard.AddDrawable(&mDrawableFloor);
	//add drawable group to scene
	mScene.AddDrawableGroup(&grpDrawableStandard);
	mScene.AddDrawableGroup(&grpDrawableGizmo);
	mScene.AddDrawableGroup(&grpDrawableVolume);
	mScene.AddDrawableGroup(&grpDrawableDebug);
	//hide gizmo group
	grpDrawableGizmo.SetEnabled(false);

	//>>>>>IV. Uniform<<<<<//
	//apply uniform
	mFrameUniform.ApplyIntensity(10.f);
	mFrameUniform.ApplyFrameNum(mRenderer.frame);
	mFrameUniform.ApplyTextureSize(WIDTH_LIGHT, HEIGHT_LIGHT);
	mFrameUniformLight.ApplyIntensity(0.f);
	mFrameUniformLight.ApplyFrameNum(mRenderer.frame);
	mSceneUniform.ApplyStep(32);
	mSceneUniform.ApplyVolumeG(0.8);
	mSceneUniform.ApplyVolumeNoiseScale(0.5);
	mSceneUniform.ApplyVolumeNoiseSize(XMFLOAT3(0.5, 0.5, 0.5));
	mSceneUniform.ApplyVolumeNoiseVel(XMFLOAT3(0.05, 0.05, 0.05));
	mSceneUniform.ApplyTimeScale(0.05);
	mSceneUniform.ApplyScreenSize(WIDTH, HEIGHT);
	//add uniform to scene
	mScene.AddObjectUniform(&mObjUniAxis);
	mScene.AddObjectUniform(&mObjUniDebugShadowMap);
	mScene.AddObjectUniform(&mObjUniFanBase);
	mScene.AddObjectUniform(&mObjUniFanBlade);
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
	objFanBase.SetTransform(&mTransformFanBase);
	objFanBase.SetDrawable(&mDrawableFanBase);
	objFanBase.ConnectObjectUniformWrite(&mObjUniFanBase);
	objFanBlade.SetTransform(&mTransformFanBlade);
	objFanBlade.SetDrawable(&mDrawableFanBlade);
	objFanBlade.ConnectObjectUniformWrite(&mObjUniFanBlade);
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
	mScene.AddObject(&objFanBase);
	mScene.AddObject(&objFanBlade);
	mScene.AddObject(&objFloor);
	mScene.AddObject(&objCamera);
	mScene.AddObject(&objLight);

	//init drawable group vectors
	//second pass drawable groups
	GlobalDrawableGrpVec.push_back(&grpDrawableStandard);
	GlobalDrawableGrpVec.push_back(&grpDrawableVolume);
	GlobalDrawableGrpVec.push_back(&grpDrawableGizmo);
	GlobalDrawableGrpVec.push_back(&grpDrawableDebug);
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
	mRenderer.SetRenderTarget(&mRTexScreenDepth);
	mRenderer.ClearCurrentRenderTarget({ 1.f }, 1.f, 0);//since I know this render target is DXGI_FORMAT_R16_FLOAT, I can supply only the red channel
	mRenderer.DrawGroups(GlobalDrawableGrpVecShadowPass, &mMaterialShadowPass, &mFrameUniform);
	//second pass
	mRenderer.SetDefaultRenderTarget();
	mRenderer.ClearCurrentRenderTargetDefault();
	mRenderer.DrawGroups(GlobalDrawableGrpVec);
}

void DrawGui()
{
	static float intensity = mFrameUniform.GetIntensity();
	static int step = mSceneUniform.GetStep();
	static bool show_gizmo = grpDrawableGizmo.IsEnabled();
	static bool show_debug = grpDrawableDebug.IsEnabled();
	static XMFLOAT4 light_color_v = mLight.GetCol();
	static float light_color[4] = { light_color_v.x, light_color_v.y, light_color_v.z, light_color_v.w };
	static float light_radius = mLight.GetRadius();
	static float light_angle = mLight.GetAngle();
	static XMFLOAT3 volume_color_v = mSceneUniform.GetVolumeCol();
	static float volume_color[3] = { volume_color_v.x, volume_color_v.y, volume_color_v.z };
	static float volume_g = mSceneUniform.GetVolumeG();
	static float volume_noise_scale = mSceneUniform.GetVolumeNoiseScale();
	static XMFLOAT3 volume_noise_size_v = mSceneUniform.GetVolumeNoiseSize();
	static float volume_noise_size[3] = { volume_noise_size_v.x, volume_noise_size_v.y, volume_noise_size_v.z };
	static float fan_angular_vel = fanAngularVel;
	static XMFLOAT3 volume_noise_vel_v = mSceneUniform.GetVolumeNoiseVel();
	static float volume_noise_vel[3] = { volume_noise_vel_v.x, volume_noise_vel_v.y,volume_noise_vel_v.z };
	static float time_scale = mSceneUniform.GetTimeScale();

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Control Panel");
	if (ImGui::Checkbox("Show Gizmo", &show_gizmo))
	{
		grpDrawableGizmo.SetEnabled(show_gizmo);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Show Debug", &show_debug))
	{
		grpDrawableDebug.SetEnabled(show_debug);
	}
	if (ImGui::SliderFloat("Fan Angular Vel", &fan_angular_vel, 0.0f, 720.0f))
	{
		fanAngularVel = fan_angular_vel;
	}
	if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 30.0f))
	{
		mFrameUniform.ApplyIntensity(intensity);
	}
	if (ImGui::SliderFloat("Time Scale", &time_scale, 0.0f, 0.1f, "%.6f"))
	{
		mSceneUniform.ApplyTimeScale(time_scale);
	}
	if (ImGui::SliderInt("Step", &step, 0, 100))
	{
		mSceneUniform.ApplyStep(step);
	}
	if (ImGui::ColorEdit4("Light Color", light_color))
	{
		light_color_v = XMFLOAT4(light_color);
		mLight.SetCol(light_color_v);
		objLight.SetNeedToUpdate(true);
	}
	if (ImGui::SliderFloat("Light Radius", &light_radius, 0.0f, 100.0f))
	{
		mLight.SetRadius(light_radius);
		objLight.SetNeedToUpdate(true);
	}
	if (ImGui::SliderFloat("Light Angle", &light_angle, 0.0f, 180.0f))
	{
		mLight.SetAngle(light_angle);
		objLight.SetNeedToUpdate(true);
	}
	if (ImGui::ColorEdit3("Volume Color", volume_color))
	{
		volume_color_v = XMFLOAT3(volume_color);
		mSceneUniform.ApplyVolumeCol(volume_color_v);
	}
	if (ImGui::SliderFloat("Volume G", &volume_g, 0.0f, 1.0f, "%.6f"))
	{
		mSceneUniform.ApplyVolumeG(volume_g);
	}
	if (ImGui::SliderFloat("Volume Noise Scale", &volume_noise_scale, 0.0f, 1.0f, "%.6f"))
	{
		mSceneUniform.ApplyVolumeNoiseScale(volume_noise_scale);
	}
	if (ImGui::SliderFloat3("Volume Noise Size", volume_noise_size, 0.0f, 2.0f))
	{
		volume_noise_size_v = XMFLOAT3(volume_noise_size);
		mSceneUniform.ApplyVolumeNoiseSize(volume_noise_size_v);
	}
	if (ImGui::SliderFloat3("Volume Noise Vel", volume_noise_vel, -1.0f, 1.0f, "%.6f"))
	{
		volume_noise_vel_v = XMFLOAT3(volume_noise_vel);
		mSceneUniform.ApplyVolumeNoiseVel(volume_noise_vel_v);
	}

	ImGui::Text("Z and mouse,W,A,S,D,Q,E - ctrl volume");
	ImGui::Text("X and mouse,<,>,arrows - ctrl light");
	ImGui::Text("C and mouse - ctrl camera");
	deltaTime = 1.f / ImGui::GetIO().Framerate;
	ImGui::Text("fps %.3f ms/frame (%.1f FPS)", 1000.0f * deltaTime, 1.f / deltaTime);
	ImGui::End();

	// Set to default render target
	mRenderer.SetDefaultRenderTarget();
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Draw()
{
	//Draw scene
	DrawScene();
	//Draw gui
	DrawGui();
	//Present the backbuffer to the screen
	mRenderer.SwapChain->Present(0, 0);
}

void DetectInput()
{
	BYTE keyboardCurrState[256];

	DIKeyboard->Acquire();

	DIKeyboard->GetDeviceState(sizeof(keyboardCurrState), (LPVOID)&keyboardCurrState);

	bool updateVolume = false;
	bool updateLight = false;
	bool updateCamera = false;

	//keyboard control
	if (KEYDOWN(keyboardCurrState, DIK_ESCAPE))
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}

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
	if (KEYDOWN(keyboardCurrState, DIK_Z)|| 
		KEYDOWN(keyboardCurrState, DIK_X)|| 
		KEYDOWN(keyboardCurrState, DIK_C))
	{
		if (!mouseAcquired)
		{
			DIMouse->Acquire();
			mouseAcquired = true;
		}

		DIMOUSESTATE mouseCurrState;
		DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

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
		}

		mouseLastState = mouseCurrState;
	}
	else if (mouseAcquired)
	{
		DIMouse->Unacquire();
		mouseAcquired = false;
	}

	if (updateLight)
	{
		objLight.SetNeedToUpdate(true);
		objCamera.SetNeedToUpdate(true);//for ApplyCameraShadow(...)
	}

	if (updateVolume)
	{
		objVolume.SetNeedToUpdate(true);
	}

	if (updateCamera)
	{
		objCamera.SetNeedToUpdate(true);
	}

	std::memcpy(keyboardLastState, keyboardCurrState, 256 * sizeof(BYTE));
}

void UpdateScene()
{
	mRenderer.frame++;
	mFrameUniform.ApplyFrameNum(mRenderer.frame);
	mFrameUniformLight.ApplyFrameNum(mRenderer.frame);
	mScene.UpdateSceneCheck(mRenderer.d3d11DevCon);
	mTransformFanBlade.rotation.y += deltaTime * fanAngularVel;
	if (mTransformFanBlade.rotation.y > 360.f) mTransformFanBlade.rotation.y = 0.f;
	objFanBlade.UpdateObject();
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
			Draw();
		}
	}

	return msg.wParam;
}

void Release()
{
	// imgui shutdown
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	// my release
	DIKeyboard->Unacquire();
	DIKeyboard->Release();
	DIMouse->Unacquire();
	DIMouse->Release();
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

	if (!InitGui())	//Initialize Gui
	{
		MessageBox(0, "Gui Initialization - Failed", "Error", MB_OK);
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
