#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    float3 ori = CAMERA_POS;
    float3 dir = normalize(IN.posW - ori);
    float3 start = float3(0, 0, 0);
    float3 finish = float3(0, 0, 0);

    float3 ori_local = mul(M_INV, float4(ori, 1)).xyz;
    float3 dir_local = mul(M_INV, float4(dir, 0)).xyz;

    if (!IntersectSphere(ori_local, dir_local, start, finish))
        return float4(0, 1, 0, 1);

    start = mul(M, float4(start, 1)).xyz;
    finish = mul(M, float4(finish, 1)).xyz;

    if (OccludeVolume(IN.pos.xy, start, finish))
        return float4(0, 0, 0, 0);

    float3 delta = (finish - start) / STEP;
    float4 fogColor = float4(0, 0, 0, 0);

    for (int i = 0; i < STEP; i++)
    {
        float3 current = start + i * delta;
        if (InFrustum(VP_SHADOW, current) && !InShadow(current))
        {
            float3 dir_light = normalize(LIGHT_POS - current);
            float cosTheta = (dot(-dir, dir_light));
            float3 noise_pos = (current + VOLUME_NOISE_VEL * TIME_SCALE * FRAME_NUM) * VOLUME_NOISE_SIZE;
            float noise = lerp(1.0, saturate(cnoise(noise_pos)), VOLUME_NOISE_SCALE);
            fogColor += Phase(VOLUME_G, cosTheta) * noise * AttenuateSpotLight(current);
        }
    }
    
    fogColor = INTENSITY * float4(VOLUME_COL, 1) * float4(LIGHT_COL.rgb, 1) * fogColor / STEP;

    return fogColor;
}
