#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    float4 albedo = MainTexture.Sample(SamplerMainTexture, FlipV(IN.uv));
    float4 col = albedo *
                 LIGHT_COL *
                 (dot(IN.norW, normalize(LIGHT_POS - IN.posW)) * 0.5 + 0.5) *
                 AttenuatePointLight(IN.posW); //half lambert

    //if (InShadow(IN.posW))
    //    col = float4(0, 0, 0, 1);

    col.rgb *= Shadow(IN.posW);
    
    return col;
}
