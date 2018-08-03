
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

v2f main(float3 pos : MYPOSITION, float4 col : MYCOLOR, float3 nor : MYNORMAL, float2 uv : MYUV)
{
    v2f o;
    //this should be mul(float4(pos, 1.0f), MVP), 
    //but since there is a row-major-to-column-major conversion from cpu to gpu£¬
    //and we are doing this conversion, so the right multiply becomes left multiply
    o.pos = mul(mul(VP, M), float4(pos, 1.0f));
    o.color = col * COL_OBJECT * COL_FRAME;
    o.posW = mul(M, float4(pos,1)).xyz;

	return o;
}
