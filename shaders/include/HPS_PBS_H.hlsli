cbuffer M_Material : register(b0)
{
    float4 M_Ambient;
    float4 M_F0;
    float1 M_MatIntensity;
    float1 M_Metalness;
    float1 M_Roughness;
    float1 M_BumpIntensity;
};

cbuffer L_DirLight : register(b1)
{
    float3 L_DirPos;
    float1 L_DirInt;
    float4 L_DirDiffuse;
};

struct SPointLight
{
    float3 L_PLPos;
    float4 L_PLDiffuse;
    float2 L_PLInt;
};

cbuffer L_PointLight : register(b2)
{
    const uint4 numPLights;
    SPointLight PL[8];
};