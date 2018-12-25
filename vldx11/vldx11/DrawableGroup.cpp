#include "DrawableGroup.h"

DrawableGroup::DrawableGroup(DrawableGroupType _type) : 
	type(_type), 
	d3d11RasterizerState(nullptr), 
	d3d11DepthStencilState(nullptr), 
	d3d11BlendState(nullptr),
	initiated(false),
	enabled(true)
{
}

DrawableGroup::~DrawableGroup()
{
	if (d3d11RasterizerState != nullptr) d3d11RasterizerState->Release();
	if (d3d11DepthStencilState != nullptr) d3d11DepthStencilState->Release();
	if (d3d11BlendState != nullptr) d3d11BlendState->Release();
}

bool DrawableGroup::InitDrawableGroup(ID3D11Device* d3d11Device)
{
	if (!initiated)
	{
		HRESULT hr;
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

		if (type == DrawableGroupType::Standard)//for standard mesh
		{
			//rasterizerDesc.FillMode = D3D11_FILL_SOLID;//default, but if comment this, create function will return an error but still perform as expected -> using cd3d11 description with default value fix this
			//rasterizerDesc.CullMode = D3D11_CULL_BACK;//default
			//rasterizerDesc.FrontCounterClockwise = FALSE;//default
			//rasterizerDesc.DepthBias = 0;//default
			//rasterizerDesc.SlopeScaledDepthBias = 0.0f;//default
			//rasterizerDesc.DepthBiasClamp = 0.0f;//default
			//rasterizerDesc.DepthClipEnable = TRUE;//default
			//rasterizerDesc.ScissorEnable = FALSE;//default
			rasterizerDesc.MultisampleEnable = TRUE;
			//rasterizerDesc.AntialiasedLineEnable = FALSE;//default

			//depthStencilDesc.DepthEnable = TRUE;//default
			//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//default
			//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//default
			//depthStencilDesc.StencilEnable = FALSE;//default
			//depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;//default
			//depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;//default
			//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
		
			//blendDesc.AlphaToCoverageEnable = FALSE;//default
			//blendDesc.IndependentBlendEnable = FALSE;//default
			//blendDesc.RenderTarget[0].BlendEnable = FALSE;//default
			//blendDesc.RenderTarget[0].BlendOp =	D3D11_BLEND_OP_ADD;//default
			//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;//default
			//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;//default
			//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;//default
			//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;//default
			//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//default
			//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//default
		}
		if (type == DrawableGroupType::VolumeLight)//for volume light
		{
			//rasterizerDesc.FillMode = D3D11_FILL_SOLID;//default, but if comment this, create function will return an error but still perform as expected -> using cd3d11 description with default value fix this
			rasterizerDesc.CullMode = D3D11_CULL_NONE;//rasterizerDesc.CullMode = D3D11_CULL_BACK;//default
			//rasterizerDesc.FrontCounterClockwise = FALSE;//default
			//rasterizerDesc.DepthBias = 0;//default
			//rasterizerDesc.SlopeScaledDepthBias = 0.0f;//default
			//rasterizerDesc.DepthBiasClamp = 0.0f;//default
			//rasterizerDesc.DepthClipEnable = TRUE;//default
			//rasterizerDesc.ScissorEnable = FALSE;//default
			rasterizerDesc.MultisampleEnable = TRUE;
			//rasterizerDesc.AntialiasedLineEnable = FALSE;//default

			//depthStencilDesc.DepthEnable = TRUE;//default
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//default
			//depthStencilDesc.StencilEnable = FALSE;//default
			//depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;//default
			//depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;//default
			//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
			
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		}
		if (type == DrawableGroupType::Gizmo)//for gizmos
		{
			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			//rasterizerDesc.FrontCounterClockwise = FALSE;//default
			//rasterizerDesc.DepthBias = 0;//default
			//rasterizerDesc.SlopeScaledDepthBias = 0.0f;//default
			//rasterizerDesc.DepthBiasClamp = 0.0f;//default
			//rasterizerDesc.DepthClipEnable = TRUE;//default
			//rasterizerDesc.ScissorEnable = FALSE;//default
			//rasterizerDesc.MultisampleEnable = FALSE;//default
			rasterizerDesc.AntialiasedLineEnable = TRUE;

			//depthStencilDesc.DepthEnable = TRUE;//default
			//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//default
			//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//default
			//depthStencilDesc.StencilEnable = FALSE;//default
			//depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;//default
			//depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;//default
			//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
		
			//blendDesc.AlphaToCoverageEnable = FALSE;//default
			//blendDesc.IndependentBlendEnable = FALSE;//default
			//blendDesc.RenderTarget[0].BlendEnable = FALSE;//default
			//blendDesc.RenderTarget[0].BlendOp =	D3D11_BLEND_OP_ADD;//default
			//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;//default
			//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;//default
			//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;//default
			//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;//default
			//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//default
			//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//default
		}
		if (type == DrawableGroupType::UI)//for UI
		{
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			//rasterizerDesc.FrontCounterClockwise = FALSE;//default
			//rasterizerDesc.DepthBias = 0;//default
			//rasterizerDesc.SlopeScaledDepthBias = 0.0f;//default
			//rasterizerDesc.DepthBiasClamp = 0.0f;//default
			//rasterizerDesc.DepthClipEnable = TRUE;//default
			//rasterizerDesc.ScissorEnable = FALSE;//default
			//rasterizerDesc.MultisampleEnable = FALSE;//default
			//rasterizerDesc.AntialiasedLineEnable = FALSE;//default

			depthStencilDesc.DepthEnable = FALSE;
			//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//default
			//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//default
			//depthStencilDesc.StencilEnable = FALSE;//default
			//depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;//default
			//depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;//default
			//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;//default
			//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;//default
			//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;//default
		
			//blendDesc.AlphaToCoverageEnable = FALSE;//default
			//blendDesc.IndependentBlendEnable = FALSE;//default
			//blendDesc.RenderTarget[0].BlendEnable = FALSE;//default
			//blendDesc.RenderTarget[0].BlendOp =	D3D11_BLEND_OP_ADD;//default
			//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;//default
			//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;//default
			//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;//default
			//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;//default
			//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//default
			//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//default
		}

		hr = d3d11Device->CreateRasterizerState(&rasterizerDesc, &d3d11RasterizerState);
		if (!CheckError(hr, nullptr)) return false;
		printf("rasterizer state created!\n");

		hr = d3d11Device->CreateDepthStencilState(&depthStencilDesc, &d3d11DepthStencilState);
		if (!CheckError(hr, nullptr)) return false;
		printf("depthstencil state created!\n");

		hr = d3d11Device->CreateBlendState(&blendDesc, &d3d11BlendState);
		if (!CheckError(hr, nullptr)) return false;
		printf("blend state created!\n");

		initiated = true;
	}
	return true;
}

void DrawableGroup::AddDrawable(Drawable* pDrawable)
{
	drawableVector.push_back(pDrawable);
}

void DrawableGroup::RemoveDrawable(Drawable* pDrawable)
{
	vector<Drawable*>::iterator it = find(drawableVector.begin(), drawableVector.end(), pDrawable);
	if (it != drawableVector.end())
		drawableVector.erase(it);
}

void DrawableGroup::Draw(ID3D11DeviceContext* d3d11DevCon)
{
	d3d11DevCon->RSSetState(d3d11RasterizerState);
	d3d11DevCon->OMSetDepthStencilState(d3d11DepthStencilState, 0);//Stencil Ref (0 in this case, which is not useful in our situation, it is just a place holder) can be replaced in a shader using SV_StencilRef
	d3d11DevCon->OMSetBlendState(d3d11BlendState, NULL, 0xffffffff);
	for(auto item = drawableVector.begin();item!=drawableVector.end();item++)
	{
		(*item)->Draw(d3d11DevCon);
	}
}

void DrawableGroup::Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat)
{
	d3d11DevCon->RSSetState(d3d11RasterizerState);
	d3d11DevCon->OMSetDepthStencilState(d3d11DepthStencilState, 0);//Stencil Ref (0 in this case, which is not useful in our situation, it is just a place holder) can be replaced in a shader using SV_StencilRef
	d3d11DevCon->OMSetBlendState(d3d11BlendState, NULL, 0xffffffff);
	for (auto item = drawableVector.begin(); item != drawableVector.end(); item++)
	{
		(*item)->Draw(d3d11DevCon, _pMat);
	}
}

void DrawableGroup::Draw(ID3D11DeviceContext* d3d11DevCon, Material* _pMat, FrameUniform* _pFrameUniform)
{
	d3d11DevCon->RSSetState(d3d11RasterizerState);
	d3d11DevCon->OMSetDepthStencilState(d3d11DepthStencilState, 0);//Stencil Ref (0 in this case, which is not useful in our situation, it is just a place holder) can be replaced in a shader using SV_StencilRef
	d3d11DevCon->OMSetBlendState(d3d11BlendState, NULL, 0xffffffff);
	for (auto item = drawableVector.begin(); item != drawableVector.end(); item++)
	{
		(*item)->Draw(d3d11DevCon, _pMat, _pFrameUniform);
	}
}

bool DrawableGroup::IsInitiated()
{
	return initiated;
}

bool DrawableGroup::IsEnabled()
{
	return enabled;
}

void DrawableGroup::SetEnabled(bool _enabled)
{
	enabled = _enabled;
}
