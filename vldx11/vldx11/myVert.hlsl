#include "myInclude.hlsl"

v2f main(a2v IN)
{
    v2f o;
    //this should be mul(float4(IN.pos, 1.0f), MVP), 
    //but since there is a row-major-to-column-major conversion from cpu to gpu£¬
    //and we are doing this conversion, so the right multiply becomes left multiply
    o.pos = mul(mul(VP, M), float4(IN.pos, 1.0f));
    o.color = IN.col * COL_OBJECT * COL_FRAME;
    o.posW = mul(M, float4(IN.pos,1)).xyz;
	o.norW = normalize(mul(float4(IN.nor, 0), M_INV).xyz);//transpose of inverse of the model matrix
	o.uv = IN.uv;

	return o;
}
