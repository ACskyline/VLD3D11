#include "myInclude.hlsl"

float4 main(v2f IN) : SV_TARGET
{
    float3 ori = CAMERA_POS;
    float3 dir = normalize(IN.posW - ori);
    float3 start = float3(0,0,0);
    float3 finish = float3(0,0,0);

    float3 ori_local = mul(M_INV, float4(ori,1)).xyz;
    float3 dir_local = mul(M_INV, float4(dir,0)).xyz;

    if(!IntersectCube(ori_local, dir_local, start, finish))
        return float4(0,1,0,1);

    start = mul(M, float4(start,1)).xyz;
    finish = mul(M, float4(finish,1)).xyz;

    float3 delta = (finish - start) / STEP;
    float3 fogColor = float3(0,0,0);

    for (int i = 0; i < STEP; i++)
    {
        //TO DO: INTERSECTION DETECTION & RAY MARCHING
        float3 current = start + i * delta;
        float3 dir_light = normalize(LIGHT_POS - current);
        float cosTheta = (dot(-dir, dir_light));
        fogColor += LIGHT_COL.rgb * Phase(0.8, cosTheta) * saturate(cnoise(current));
    }
    
    fogColor = fogColor / STEP * INTENSITY;

    return float4(fogColor, 1);
}
