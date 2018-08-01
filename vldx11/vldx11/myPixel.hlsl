struct v2f
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(v2f IN) : SV_TARGET
{
    return IN.color;
}