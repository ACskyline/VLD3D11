#include "DrawableGroup.h"



DrawableGroup::DrawableGroup(DrawableGroupType _type) : type(_type), d3d11RasterizerState(nullptr), d3d11DepthStencilState(nullptr)
{
}

DrawableGroup::~DrawableGroup()
{
	if (d3d11RasterizerState != nullptr) d3d11RasterizerState->Release();
	if (d3d11DepthStencilState != nullptr) d3d11DepthStencilState->Release();
}

bool DrawableGroup::InitDrawableGroup(ID3D11Device* d3d11Device)
{
	HRESULT hr;
	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);//ZeroMemory is bad, CD3D11 contains all helper class
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);//ZeroMemory is bad, CD3D11 contains all helper class
	//ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

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
	}
	else if (type == DrawableGroupType::Gizmo)//for gizmos
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

		//depthStencilDesc.DepthEnable = FALSE;
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
	}

	hr = d3d11Device->CreateRasterizerState(&rasterizerDesc, &d3d11RasterizerState);
	if (!CheckError(hr, nullptr)) return false;
	printf("rasterizer state created!\n");

	hr = d3d11Device->CreateDepthStencilState(&depthStencilDesc, &d3d11DepthStencilState);
	if (!CheckError(hr, nullptr)) return false;
	printf("depthstencil state created!\n");

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
	for(auto item = drawableVector.begin();item!=drawableVector.end();item++)
	{
		(*item)->Draw(d3d11DevCon);
	}
}