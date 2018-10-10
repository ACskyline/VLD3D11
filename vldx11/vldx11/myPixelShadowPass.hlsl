#include "myInclude.hlsl"

float main(v2f IN) : SV_TARGET
{
    return IN.pos.z; // projection space depth
}