#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    float4 col = MainTexture.Sample(SamplerMainTexture, FlipV(IN.uv));
    return float4(col.r, col.g, col.b, 1) *
           LIGHT_COL *
           (dot(IN.norW, normalize(-LIGHT_DIR)) * 0.5 + 0.5);//half lambert
}