#include "myInclude.hlsl"

float4 main(v2f IN) : SV_TARGET
{
    return LIGHT_COL *
           IN.color *
           (dot(IN.norW, normalize(-LIGHT_DIR)) * 0.5 + 1);
}