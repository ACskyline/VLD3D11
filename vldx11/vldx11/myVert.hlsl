struct v2f
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer ObjectUniform : register(b0)
{
    float4x4 M;
    float4 COL_OBJECT;
};

cbuffer FrameUniform : register(b1)
{
    float4x4 VP;
    float4 COL_FRAME;
};

v2f main(float3 pos : MYPOSITION, float4 col : MYCOLOR, float3 nor : MYNORMAL, float2 uv : MYUV)
{
    v2f o;
    //this should be mul(float4(pos, 1.0f), MVP), 
    //but since there is a row-major-to-column-major conversion from cpu to gpu£¬
    //and we are doing this conversion, so the right multiply becomes left multiply
    o.pos = mul(mul(VP, M), float4(pos, 1.0f));
    o.color = col * COL_OBJECT * COL_FRAME;

	return o;
}
