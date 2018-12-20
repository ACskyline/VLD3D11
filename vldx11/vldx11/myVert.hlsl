#include "myInclude.hlsli"

v2f main(a2v IN)
{
    v2f o;
    //this should be mul(float4(IN.pos, 1.0f), MVP), see https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/the-direct3d-transformation-pipeline
    //the matrix(produced by XMMatrixLookAtLH, XMMatrixTranslation and other functions) in this document use right multiply
    //but since there is a row-major-to-column-major conversion from cpu to gpu
    //and we are doing this conversion, so the right multiply becomes left multiply
    //e.g. a * M * V * P = ((VP)^T * M^T * a^T)^T
    //that is, (VP)^T is actually VP in code, M^T is actually M in code
    //since row vector is treated as column vector and vas versa, it becomes
    // a * M * V * P = ((VP)^T * M^T * a)

    o.pos = mul(mul(VP, M), float4(IN.pos, 1.0f));
    o.wtf = o.pos;
    o.color = IN.col * COL_OBJECT * COL_FRAME;
    o.posW = mul(M, float4(IN.pos, 1)).xyz;
    
    //transpose of inverse of the model matrix, 
    //since d3d use right multiply and the cpu to gpu conversion, right multiply the transpose of a matrix
    //becomes right multiply the matrix itself
    o.norW = normalize(mul(float4(IN.nor, 0), M_INV).xyz); 
    o.uv = IN.uv;
    
	return o;
}
