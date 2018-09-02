#include "myInclude.hlsl"

float4 main(v2f IN) : SV_TARGET
{
	float4 col = MainTexture.Sample(SamplerMainTexture, IN.uv);//float4(IN.uv, 0, 1);//
	return float4(col.r, col.g, col.b, 1);
}