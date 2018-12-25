#include "myInclude.hlsli"

float main(v2f IN) : SV_TARGET
{
    float depth = IN.pos.z;//no need to do perspective division b/c SV_Position will do it automatically
    //float depth = IN.posEye.z;//eye space z is more accurate
    
    return depth;
}
