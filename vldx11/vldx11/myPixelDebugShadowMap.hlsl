#include "myInclude.hlsl"

float4 main(v2f IN) : SV_TARGET
{
    float depth = ShadowMap.Sample(SamplerShadowMap, FlipV(IN.uv));
    float4 col = float4(depth, depth, depth, 1);
    return col;
}