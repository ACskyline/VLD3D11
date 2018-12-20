#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    float depth = IN.pos.z;//no need to do perspective division b/c SV_Position will do it automatically
    return float4(depth, depth, depth, 1); // projection space depth
}