#include "include/HVS_Transform.hlsli"

cbuffer CameraBuffer
{
    float3 camPos;
};

struct VSIn
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VSOut
{
    float3 camPos : POSITION0;
    float3 worldPos : POSITION1;
    float3 W_normal : NORMAL0;
    float2 tex : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 pos : SV_POSITION;
};

VSOut main(VSIn iVS)
{
    VSOut oVS;
    oVS.pos = mul(float4(iVS.pos, 1.0f), mul(world, viewProj));
    oVS.W_normal = normalize(mul(iVS.normal, (float3x3) world));
    oVS.tex = iVS.tex;
    oVS.tangent = normalize(mul(iVS.tangent, (float3x3) world));
    oVS.binormal = normalize(mul(iVS.binormal, (float3x3) world));
    oVS.worldPos = (float3) mul(float4(iVS.pos, 1.0f), world);
    oVS.camPos = camPos;

    return oVS;
}