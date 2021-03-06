#define INFINITE_MIN -1000
#define INFINITE_MAX 1000
#define PI 3.14159265
#define EPSILON 0.0000001
#define SHADOW_BIAS 0.00001

#define OUT_OF_FRUSTUM 0
#define OUT_OF_FRUSTUM 0

cbuffer ObjectUniform : register(b0)
{
	float4x4 M;
	float4x4 M_INV;
    float4x4 M_INV_TRANS;
};

cbuffer FrameUniform : register(b1)
{
    float4x4 V;
    float4x4 V_INV;
    float4x4 P;
    float4x4 P_INV;
	float4x4 VP;
	float4x4 VP_INV;
    float4x4 V_SHADOW;
    float4x4 V_SHADOW_INV;
    float4x4 P_SHADOW;
    float4x4 P_SHADOW_INV;
    float4x4 VP_SHADOW;
    float4x4 VP_INV_SHADOW;
	float3 CAMERA_POS;
	float INTENSITY;
    float3 CAMERA_POS_SHADOW;
	uint FRAME_NUM;
    float2 TEXTURE_SIZE;
};

cbuffer SceneUniform : register(b2)
{
	float4 LIGHT_COL;
	float3 LIGHT_POS;//point light, spot light
	uint STEP;
	float3 LIGHT_DIR;//directional light, spot light
	float FAR_CLIP;
    float3 VOLUME_COL;
    float LIGHT_RADIUS;
    float3 VOLUME_NOISE_SIZE;
    float LIGHT_ANGLE;//spot light
    float3 VOLUME_NOISE_VEL;
    float VOLUME_G;
    float2 SCREEN_SIZE;
    float VOLUME_NOISE_SCALE;
    float TIME_SCALE;
    float NEAR_CLIP;
};

struct a2v
{
	float3 pos : MYPOSITION;
	float3 nor : MYNORMAL;
	float2 uv : MYUV;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 posW : TEXCOORD1;
	float3 norW : TEXCOORD2;
    float3 posEye : TEXCOORD3;
};

struct a2v_COLOR
{
    float3 pos : MYPOSITION;
    float4 col : MYCOLOR;
    float3 nor : MYNORMAL;
    float2 uv : MYUV;
};

struct v2f_COLOR
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 posW : TEXCOORD1;
    float3 norW : TEXCOORD2;
    float3 posEye : TEXCOORD3;
    float4 col : TEXCOORD4;
};

Texture2D MainTexture : register(t0);
SamplerState SamplerMainTexture : register(s0);

Texture2D ShadowMap : register(t1);
SamplerState SamplerShadowMap : register(s1);

Texture2D ShadowMapPCF : register(t2);//This is empty because we are only rendering to ShadowMap. 
SamplerComparisonState SamplerShadowMapPCF : register(s2);//We only need this to use PCF

Texture2D ScreenDepth : register(t3);
SamplerState SamplerScreenDepth : register(s3);

Texture2D ScreenDepthPCF : register(t4); //This is empty because we are only rendering to ShadowMap. 
SamplerComparisonState SamplerScreenDepthPCF : register(s4); //We only need this to use PCF

float ScreenDepthToEyeDepth(float screenDepth)
{
    return (FAR_CLIP * NEAR_CLIP) / (FAR_CLIP - screenDepth * (FAR_CLIP - NEAR_CLIP));
}

float2 FlipV(float2 uv)
{
    return float2(uv.x, 1-uv.y);
}

bool InFrustum(float4x4 vp, float3 wpos)
{
    float4 cpos = mul(vp, float4(wpos, 1.0f));
    if (cpos.w < 0 || cpos.z < 0 || cpos.z > cpos.w || cpos.x > cpos.w || cpos.x < -cpos.w || cpos.y > cpos.w || cpos.y < -cpos.w)
        return false;
    else
        return true;
}

//assuming in frustum
float ShadowMultiTap(float3 wpos, int sqrtTap)
{
    float4 cpos = mul(VP_SHADOW, float4(wpos, 1.0f));
    float3 ppos = cpos.xyz / cpos.w;
    float2 uv = (ppos.xy + float2(1.0, 1.0)) * 0.5;
    float depth = ppos.z;
    float depthShadowMap = 0;
    int absOffsetMax = sqrtTap / 2;
    for (int i = -absOffsetMax; i <= absOffsetMax; i++)
    {
        for (int j = -absOffsetMax; j <= absOffsetMax; j++)
        {
            float2 currentUV = uv + float2(i, j) / TEXTURE_SIZE;
            depthShadowMap += ShadowMap.SampleCmpLevelZero(SamplerShadowMapPCF, FlipV(currentUV), depth - SHADOW_BIAS).r;
        }
    }
    depthShadowMap /= (absOffsetMax * 2.0 + 1.0) * (absOffsetMax * 2.0 + 1.0);
    return depthShadowMap;
}

//assuming in frustum
float Shadow(float3 wpos)
{
    float4 cpos = mul(VP_SHADOW, float4(wpos, 1.0f));
    float3 ppos = cpos.xyz / cpos.w;
    float2 uv = (ppos.xy + float2(1.0, 1.0)) * 0.5;
    float depth = ppos.z;
    float depthShadowMap = ShadowMap.SampleCmpLevelZero(SamplerShadowMapPCF, FlipV(uv), depth - SHADOW_BIAS).r;
    return depthShadowMap;
}

//assuming in frustum
bool InShadow(float3 wpos)
{
    float4 cpos = mul(VP_SHADOW, float4(wpos, 1.0f));
    float3 ppos = cpos.xyz / cpos.w;
    float2 uv = (ppos.xy + float2(1.0, 1.0)) * 0.5;
    float depthShadowMap = ShadowMap.SampleLevel(SamplerShadowMap, FlipV(uv), 0).r;
    float depth = ppos.z;
    if (depth > depthShadowMap + SHADOW_BIAS)
        return true;
    else
        return false;
}

//assuming in frustum
//can be optimized by using SV_Position.xy to get uv
float SampleScreenDepth(float3 wpos)
{
    float4 cpos = mul(VP, float4(wpos, 1.0f));
    float3 ppos = cpos.xyz / cpos.w;
    float2 uv = (ppos.xy + float2(1.0, 1.0)) * 0.5;
    float screenDepth = ScreenDepth.SampleLevel(SamplerScreenDepth, FlipV(uv), 0).r;
    return screenDepth;
}

float SampleScreenDepth(float2 spos)
{
    float2 uv = spos / SCREEN_SIZE;//no need to flip because it's in screen space
    float screenDepth = ScreenDepth.SampleLevel(SamplerScreenDepth, uv, 0).r;
    return screenDepth;
}

bool OccludeVolume(in float3 posW, inout float3 start, inout float3 finish)
{
    //handle occlusion begin
    float3 startEye = mul(V, float4(start, 1)).xyz;
    float3 finishEye = mul(V, float4(finish, 1)).xyz;
    float screenDepth = SampleScreenDepth(posW);
    float eyeDepth = ScreenDepthToEyeDepth(screenDepth);
    
    if (eyeDepth < startEye.z)//occluded, we disabled depth test for volume, so we need to do it manually
        return true;
    else //not occluded
        finishEye.z = min(finishEye.z, eyeDepth);

    start = mul(V_INV, float4(startEye, 1)).xyz;
    finish = mul(V_INV, float4(finishEye, 1)).xyz;
    //handle occlusion end

    return false;
}

bool OccludeVolume(in float2 spos, inout float3 start, inout float3 finish)
{
    //handle occlusion begin
    float3 startEye = mul(V, float4(start, 1)).xyz;
    float3 finishEye = mul(V, float4(finish, 1)).xyz;
    float screenDepth = SampleScreenDepth(spos);
    float eyeDepth = ScreenDepthToEyeDepth(screenDepth);
    
    if (eyeDepth < startEye.z)//occluded, we disabled depth test for volume, so we need to do it manually
        return true;
    else //not occluded
        finishEye.z = min(finishEye.z, eyeDepth);

    start = mul(V_INV, float4(startEye, 1)).xyz;
    finish = mul(V_INV, float4(finishEye, 1)).xyz;
    //handle occlusion end

    return false;
}

float AttenuateSpotLight(float3 posW)
{
    float a = dot(normalize(posW - LIGHT_POS), LIGHT_DIR);
    float b = cos(radians(LIGHT_ANGLE / 2.0));
    if (a < 0 || a < b)
        return 0;
    return max(0, (1.f - 1.f / LIGHT_RADIUS * length(LIGHT_POS - posW)) * saturate((a - b) / (1.0 - b)));
}

float AttenuatePointLight(float3 posW)
{
    return max(0, 1.f - 1.f / LIGHT_RADIUS * length(LIGHT_POS - posW));
}

//unit cube face intersection detection
bool IntersectCubeFace(in uint face, in float3 ori, in float3 dir, inout float t)
{
	float3 nor = float3(0, 0, 0);
	float3 pos = float3(0, 0, 0);

	switch (face)
	{
	case 0:
		nor = float3(0, 0, -1);
		pos = float3(0, 0, -0.5);
		break;
	case 1:
		nor = float3(1, 0, 0);
		pos = float3(0.5, 0, 0);
		break;
	case 2:
		nor = float3(0, 0, 1);
		pos = float3(0, 0, 0.5);
		break;
	case 3:
		nor = float3(-1, 0, 0);
		pos = float3(-0.5, 0, 0);
		break;
	case 4:
		nor = float3(0, -1, 0);
		pos = float3(0, -0.5, 0);
		break;
	case 5:
		nor = float3(0, 1, 0);
		pos = float3(0, 0.5, 0);
		break;
	default:
		return false;
		break;
	}

	if (mul(dir, nor) == 0) return false;
	t = -mul(ori - pos, nor) / mul(dir, nor);
	float3 p = ori + t * dir;

	switch (face)
	{
	case 0:
	case 2:
		if (p.x < -0.5 || p.x > 0.5 || p.y < -0.5 || p.y > 0.5)
			return false;
		break;
	case 1:
	case 3:
		if (p.z < -0.5 || p.z > 0.5 || p.y < -0.5 || p.y > 0.5)
			return false;
		break;
	case 4:
	case 5:
		if (p.x < -0.5 || p.x > 0.5 || p.z < -0.5 || p.z > 0.5)
			return false;
		break;
	default:
		return false;
		break;
	}

	return true;
}

//unit cube intersection detection
//clamp start to camera if camera is inside of the volume
bool IntersectCube(in float3 ori, in float3 dir, inout float3 start, inout float3 finish)
{
	float minT = INFINITE_MAX;
	float maxT = INFINITE_MIN;

	bool result = false;
	bool potential = false;

	for (uint i = 0; i<6; i++)
	{
		float t = 0;
		if (IntersectCubeFace(i, ori, dir, t))
		{
			if (t<minT)
			{
				minT = t;
			}
			if (t>maxT)
			{
				maxT = t;
			}
			potential = true;
		}
	}

	if (potential)
	{
		if (maxT >= 0)
		{
            minT = max(0, minT); //clamp start to camera if camera is inside of the volume
			start = ori + minT * dir;
			finish = ori + maxT * dir;
			result = true;
		}
	}

	return result;
}

//unit sphere intersection detection
//clamp start to camera if camera is inside of the volume
bool IntersectSphere(in float3 ori, in float3 dir, inout float3 start, inout float3 finish)
{
	bool result = false;

	float A = dot(dir, dir);
	float B = 2 * dot(ori, dir);
	float C = dot(ori, ori) - 0.25;

	float delta = B * B - 4 * A * C;

	if (delta >= 0)
	{
		float t1 = (-B - sqrt(delta)) / (2 * A);
		float t2 = (-B + sqrt(delta)) / (2 * A);
		
		float tMin = t1 < t2 ? t1 : t2;
		float tMax = t1 < t2 ? t2 : t1;

		if (tMax >= 0)
		{
            tMin = max(0, tMin); //clamp start to camera if camera is inside of the volume

			start = ori + tMin * dir;
			finish = ori + tMax * dir;

			result = true;
		}
	}

	return result;
}

//intersection detection with the bottom face of unit cone
bool IntersectConeCircle(in float3 ori, in float3 dir, inout float t)
{
	bool result = false;

	t = (-0.5 - ori.y) / dir.y;

	float x = ori.x + t * dir.x;
	float z = ori.z + t * dir.z;

	if (x * x + z * z <= 0.25)
	{
		result = true;
	}

	return result;
}

//unit cone intersection detection(tip at (0,0.5,0), radius is 0.5, height is 1)
//clamp start to camera if camera is inside of the volume
bool IntersectCone(in float3 ori, in float3 dir, inout float3 start, inout float3 finish)
{
	bool result = false;

	float A = dir.x * dir.x + dir.z * dir.z - 0.25 * dir.y * dir.y;
	float B = 2 * (ori.x * dir.x + ori.z * dir.z - 0.25 * ori.y * dir.y) + 0.25 * dir.y;
	float C = ori.x * ori.x + ori.z * ori.z - 0.25 * ori.y * ori.y - 0.0625 + 0.25 * ori.y;

	float delta = B * B - 4 * A * C;

	if (delta >= 0)
	{
		float t1 = (-B - sqrt(delta)) / (2 * A);
		float t2 = (-B + sqrt(delta)) / (2 * A);

		float tMin = t1 < t2 ? t1 : t2;
		float tMax = t1 < t2 ? t2 : t1;

		float tRangeMin = dir.y > 0 ? (-0.5 - ori.y) / dir.y : (0.5 - ori.y) / dir.y;
		float tRangeMax = dir.y > 0 ? (0.5 - ori.y) / dir.y : (-0.5 - ori.y) / dir.y;

		if ((tMin < tRangeMin || tMin > tRangeMax) && (tMax < tRangeMin || tMax > tRangeMax)) return false;
		
		float t = 0;

		if ((tMin < tRangeMin || tMin > tRangeMax) && IntersectConeCircle(ori, dir, t))
		{
			tMin = min(tMax, t);
			tMax = max(tMax, t);
		}

		if ((tMax < tRangeMin || tMax > tRangeMax) && IntersectConeCircle(ori, dir, t))
		{
			tMax = max(tMin, t);
			tMin = min(tMin, t);
		}

		if (tMax >= 0)
		{
            tMin = max(0, tMin); //clamp start to camera if camera is inside of the volume
			start = ori + tMin * dir;
			finish = ori + tMax * dir;
			result = true;
		}
	}

	return result;
}

float Phase(in float g, in float cosTheta)
{
	return (1.0f - g) * (1.0f - g) / 4.0f*PI*(1.0f + g * g - 2.0f*g*cosTheta)*(1.0f + g * g - 2.0f*g*cosTheta);
}

//	Classic Perlin 3D Noise 
//	by Stefan Gustavson
float4 permute(float4 x)
{ 
	return fmod(((x*34.0) + 1.0)*x, 289.0); 
}

float4 taylorInvSqrt(float4 r)
{ 
	return 1.79284291400159 - 0.85373472095314 * r; 
}

float3 fade(float3 t)
{ 
	return t * t*t*(t*(t*6.0 - 15.0) + 10.0); 
}

float cnoise(float3 P)
{
	float3 Pi0 = floor(P); // Integer part for indexing
	float3 Pi1 = Pi0 + float3(1.0,1.0,1.0); // Integer part + 1
	Pi0 = fmod(Pi0, 289.0);
	Pi1 = fmod(Pi1, 289.0);
	float3 Pf0 = frac(P); // Fractional part for interpolation
	float3 Pf1 = Pf0 - float3(1.0,1.0,1.0); // Fractional part - 1.0
	float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
	float4 iy = float4(Pi0.yy, Pi1.yy);
	float4 iz0 = Pi0.zzzz;
	float4 iz1 = Pi1.zzzz;

	float4 ixy = permute(permute(ix) + iy);
	float4 ixy0 = permute(ixy + iz0);
	float4 ixy1 = permute(ixy + iz1);

	float4 gx0 = ixy0 / 7.0;
	float4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
	gx0 = frac(gx0);
	float4 gz0 = float4(0.5,0.5,0.5,0.5) - abs(gx0) - abs(gy0);
	float4 sz0 = step(gz0, float4(0.0,0.0,0.0,0.0));
	gx0 -= sz0 * (step(0.0, gx0) - 0.5);
	gy0 -= sz0 * (step(0.0, gy0) - 0.5);

	float4 gx1 = ixy1 / 7.0;
	float4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
	gx1 = frac(gx1);
	float4 gz1 = float4(0.5, 0.5, 0.5, 0.5) - abs(gx1) - abs(gy1);
	float4 sz1 = step(gz1, float4(0.0, 0.0, 0.0, 0.0));
	gx1 -= sz1 * (step(0.0, gx1) - 0.5);
	gy1 -= sz1 * (step(0.0, gy1) - 0.5);

	float3 g000 = float3(gx0.x, gy0.x, gz0.x);
	float3 g100 = float3(gx0.y, gy0.y, gz0.y);
	float3 g010 = float3(gx0.z, gy0.z, gz0.z);
	float3 g110 = float3(gx0.w, gy0.w, gz0.w);
	float3 g001 = float3(gx1.x, gy1.x, gz1.x);
	float3 g101 = float3(gx1.y, gy1.y, gz1.y);
	float3 g011 = float3(gx1.z, gy1.z, gz1.z);
	float3 g111 = float3(gx1.w, gy1.w, gz1.w);

	float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	g000 *= norm0.x;
	g010 *= norm0.y;
	g100 *= norm0.z;
	g110 *= norm0.w;
	float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	g001 *= norm1.x;
	g011 *= norm1.y;
	g101 *= norm1.z;
	g111 *= norm1.w;

	float n000 = dot(g000, Pf0);
	float n100 = dot(g100, float3(Pf1.x, Pf0.yz));
	float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
	float n110 = dot(g110, float3(Pf1.xy, Pf0.z));
	float n001 = dot(g001, float3(Pf0.xy, Pf1.z));
	float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
	float n011 = dot(g011, float3(Pf0.x, Pf1.yz));
	float n111 = dot(g111, Pf1);

	float3 fade_xyz = fade(Pf0);
	float4 n_z = lerp(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
	float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
	float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
	return 2.2 * n_xyz;
}
