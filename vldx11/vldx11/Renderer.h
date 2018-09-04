#pragma once

#include "DrawableGroup.h"

class Renderer
{
public:
	Renderer(uint32_t _width, uint32_t _height);
	~Renderer();

	bool InitRenderer(HWND hwnd);
	void DrawGroups(vector<DrawableGroup*>& GlobalDrawableGrpVec);
	void SetDefaultRenderTarget();
	void SetRenderTarget(RenderTexture* renderTexture);

	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	uint32_t frame;
	const int width;
	const int height;

private:
	XMVECTORF32 bgColor;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11RenderTargetView* currentRTV;
	ID3D11DepthStencilView* currentDSV;

	bool InitD3D11App(HWND hwnd);
	void InitViewport();
	bool InitDefaultRenderTargetView();
	bool InitDepthStencilView();
};

