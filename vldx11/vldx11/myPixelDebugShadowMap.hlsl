#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    float depth = ShadowMap.Sample(SamplerShadowMap, FlipV(IN.uv)).r;
    float4 col = float4(depth, depth, depth, 1);
    return col;
}
