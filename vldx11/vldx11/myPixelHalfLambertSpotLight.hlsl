#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    return LIGHT_COL *
           (dot(IN.norW, normalize(LIGHT_POS - IN.posW)) * 0.5 + 0.5) * 
            AttenuateSpotLight(IN.posW);
           //AttenuatePointLight(IN.posW);
}
