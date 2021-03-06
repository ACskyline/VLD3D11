#include "myInclude.hlsli"

v2f main(a2v IN)
{
    v2f o;
    o.pos = mul(M, float4(IN.pos, 1.0f));
    o.posW = mul(M, float4(IN.pos, 1.0f)).xyz;
    o.norW = normalize(mul(float4(IN.nor, 0), M_INV).xyz);
    o.uv = IN.uv;

    return o;
}
