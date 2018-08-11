#include "myInclude.hlsl"

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
