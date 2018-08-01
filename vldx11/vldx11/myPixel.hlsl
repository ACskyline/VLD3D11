struct v2f
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer ObjectUniform : register(b0)
{
    float4x4 M;
    float4 COL_OBJECT;
};

cbuffer FrameUniform : register(b1)
{
    float4x4 VP;
    float4 COL_FRAME;
};

float4 main(v2f IN) : SV_TARGET
{
    float4 fogCol = float4(0,0,0,1);
    return IN.color;
}