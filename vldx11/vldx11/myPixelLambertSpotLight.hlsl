#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    return LIGHT_COL * 
           (dot(IN.norW, normalize(LIGHT_POS - IN.posW))) *
           AttenuateSpotLight(IN.posW);
           //AttenuatePointLight(IN.posW);
}
