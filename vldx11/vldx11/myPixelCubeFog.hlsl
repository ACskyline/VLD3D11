#define INFINITE_MIN -1000
#define INFINITE_MAX 1000
#define PI 3.141592

cbuffer ObjectUniform : register(b0)
{
    float4x4 M;
    float4x4 M_INV;
    float4 COL_OBJECT;//for debug
};

cbuffer FrameUniform : register(b1)
{
    float4x4 VP;
    float4x4 VP_INV;
    float4 COL_FRAME;//for debug
    float3 CAMERA_POS;
};

cbuffer SceneUniform : register(b2)
{
    float4 LIGHT_COL;
    float3 LIGHT_POS;
    uint STEP;
    float FAR_CLIP;
};

struct v2f
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float3 posW : TEXCOORD0;
};

//unit cube face intersection detection
bool IntersectCubeFace(in uint face, in float3 ori, in float3 dir, inout float t, inout float3 p)
{
    float3 nor = float3(0,0,0);
    float3 pos = float3(0,0,0);
    
    switch(face)
    {
        case 0:
            nor = float3(0,0,-1);
            pos = float3(0,0,-0.5);
            break;   
        case 1:
            nor = float3(1,0,0);
            pos = float3(0.5,0,0);
            break;    
        case 2:
            nor = float3(0,0,1);
            pos = float3(0,0,0.5);
            break;    
        case 3:
            nor = float3(-1,0,0);
            pos = float3(-0.5,0,0);
            break;    
        case 4:
            nor = float3(0,-1,0);
            pos = float3(0,-0.5,0);
            break;    
        case 5:
            nor = float3(0,1,0);
            pos = float3(0,0.5,0);
            break;    
        default:
            return false;
            break;
    }

    if(mul(dir,nor)==0) return false;
    t = - mul(ori - pos, nor) / mul(dir, nor);
    p = ori + t * dir;

    switch(face)
    {
        case 0:
        case 2:
            if(p.x < -0.5 || p.x > 0.5 || p.y < -0.5 || p.y > 0.5)
                return false;
            break;   
        case 1:   
        case 3:
            if(p.z < -0.5 || p.z > 0.5 || p.y < -0.5 || p.y > 0.5)
                return false;
            break;
        case 4:
        case 5:
            if(p.x < -0.5 || p.x > 0.5 || p.z < -0.5 || p.z > 0.5)
                return false;
            break;     
        default:
            return false;
            break;
    }

    return true;
}

//unit cube intersection detection
bool IntersectCube(in float3 ori, in float3 dir, inout float3 start, inout float3 finish)
{
    float minT = INFINITE_MAX;
    float maxT = INFINITE_MIN;
    float3 minP = float3(0,0,0);
    float3 maxP = float3(0,0,0);

    bool result = false;

    for(uint i = 0;i<6;i++)
    {
        float t = 0;
        float3 p = float3(0,0,0);
        if(IntersectCubeFace(i, ori, dir, t, p))
        {
            if(t<minT)
            {
                minT = t;
                minP = p;
            }
            if(t>maxT)
            {
                maxT = t;
                maxP = p;
            }
            result = true;
        }
    }

    if(result)
    {
        start = minP;
        finish = maxP;
    }

    return result;
}

float Phase(in float g, in float cosTheta)
{
    return (1.0f-g) * (1.0f-g) / 4.0f*PI*(1.0f+g*g-2.0f*g*cosTheta)*(1.0f+g*g-2.0f*g*cosTheta);
}

float4 main(v2f IN) : SV_TARGET
{
    float3 ori = CAMERA_POS;
    float3 dir = normalize(IN.posW - ori);
    float3 start = float3(0,0,0);
    float3 finish = float3(0,0,0);

    float3 ori_local = mul(M_INV, float4(ori,1)).xyz;
    float3 dir_local = mul(M_INV, float4(dir,0)).xyz;

    if(!IntersectCube(ori_local, dir_local, start, finish))
        return float4(0,1,0,1);

    start = mul(M, float4(start,1)).xyz;
    finish = mul(M, float4(finish,1)).xyz;

    float3 delta = (finish - start) / STEP;
    float3 fogColor = float3(0,0,0);

    for (int i = 0; i < STEP; i++)
    {
        //TO DO: INTERSECTION DETECTION & RAY MARCHING
        float3 current = start + i * delta;
        float3 dir_light = normalize(LIGHT_POS - current);
        float cosTheta = (dot(-dir, dir_light));
        fogColor += LIGHT_COL.rgb * Phase(0.8, cosTheta);
    }
    
    fogColor = fogColor / STEP;

    return float4(fogColor, 1);
}
