#include "include/HVS_Transform.hlsli"

cbuffer CameraBuffer : register(b1)
{
    float3 camPos;
};

struct VSIn
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct VSOut
{
    float3 worldPos : POSITION0;
    float3 W_Normal : NORMAL0;
    float2 tex : TEXCOORD0;
    float3 viewDir : VECTOR0;
    float4 pos : SV_POSITION;
};

//Phong-only Standard shader
VSOut main(VSIn iVS)
{
    VSOut oVS;
    oVS.pos = mul(float4(iVS.pos, 1.0f), mul(world, viewProj));
    oVS.worldPos = (float3) mul(float4(iVS.pos, 1.0f), world);
    oVS.W_Normal = normalize(mul(iVS.normal, (float3x3) world));
    oVS.tex = iVS.tex;

    oVS.viewDir = normalize(camPos.xyz - oVS.worldPos);

    return oVS;
}