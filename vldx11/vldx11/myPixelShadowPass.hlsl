#include "myInclude.hlsli"

float4 main(v2f IN) : SV_TARGET
{
    //These two methods are the same. They are both wrong.
    float depth = IN.pos.z / IN.pos.w;
    //float depth = IN.wtf.z / IN.wtf.w / IN.wtf.w;

    //This method is the correct one
    //float depth = IN.pos.z;//no need to do perspective division b/c SV_Position will do it automatically
    
    return float4(depth, depth, depth, 1); //projection space depth
}
