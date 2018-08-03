
cbuffer ObjectUniform : register(b0)
{
    float4x4 M;
    float4x4 M_INV;
    float4 COL_OBJECT;//for debug
};

cbuffer FrameUniform : register(b1)
{
    float4x4 VP;
    float4x4 VP_INV;
    float4 COL_FRAME;//for debug
    float3 CAMERA_POS;
};

cbuffer SceneUniform : register(b2)
{
    float4 LIGHT_COL;
    float3 LIGHT_POS;
    uint STEP;
    float FAR_CLIP;
};

struct v2f
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float3 posW : TEXCOORD0;
};

float4 main(v2f IN) : SV_TARGET
{
    return IN.color;
}