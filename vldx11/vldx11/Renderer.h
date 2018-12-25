#pragma once

#include "DrawableGroup.h"

class Renderer
{
public:
	Renderer(uint32_t _width, uint32_t _height, XMVECTORF32 _clearColor, float _clearDepth, uint8_t _clearStencil);
	~Renderer();

	bool InitRenderer(HWND hwnd);
	void DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec);
	void DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec, Material* _pMat);
	void DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec, Material* _pMat, FrameUniform* _pFrameUniform);
	void SetDefaultRenderTarget();
	void SetRenderTarget(RenderTexture* renderTexture);
	void ClearCurrentRenderTargetDefault();
	void ClearCurrentRenderTarget(XMVECTORF32 _clearColor, float _clearDepth, uint8_t _clearStencil);

	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	uint32_t frame;
	const int width;
	const int height;

private:
	XMVECTORF32 clearColor;
	float clearDepth;
	uint8_t clearStencil;
	D3D11_VIEWPORT viewport;
	ID3D11Texture2D* depthStencilBuffer;//default depth stencil buffer, initialized in InitRednerer
	ID3D11RenderTargetView* renderTargetView;//default RTV, initialized in InitRednerer
	ID3D11DepthStencilView* depthStencilView;//default DSV, initialized in InitRednerer

	ID3D11RenderTargetView* currentRTV;//could be null
	ID3D11DepthStencilView* currentDSV;//could be null

	bool InitD3D11App(HWND hwnd);
	void InitViewport();
	bool InitDefaultRenderTargetView();
	bool InitDepthStencilView();
};

