#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    float4 albedo = MainTexture.Sample(SamplerMainTexture, FlipV(IN.uv));
    float4 col = albedo *
                 LIGHT_COL *
                 (dot(IN.norW, normalize(LIGHT_POS - IN.posW)) * 0.5 + 0.5) *
                 AttenuateSpotLight(IN.posW); //half lambert

    if (InFrustum(VP_SHADOW, IN.posW))
        col.rgb *= ShadowMultiTap(IN.posW, 5);
    else
        col = float4(0, 0, 0, 0);
    
    return col;
}
