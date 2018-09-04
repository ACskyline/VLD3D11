#include "myInclude.hlsl"

float4 main(v2f IN) : SV_TARGET
{
    float4 col = ShadowMap.Sample(SamplerShadowMap, IN.uv);
    return float4(col.r, col.g, col.b, 1) *
           LIGHT_COL *
           (dot(IN.norW, normalize(LIGHT_POS - IN.posW)) * 0.5 + 1) *
           AttenuatePointLight(IN.posW); //half lambert
}