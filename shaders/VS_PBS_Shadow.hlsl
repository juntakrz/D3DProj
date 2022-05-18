#include "include/HVS_Transform.hlsli"

cbuffer CameraBuffer : register(b1)
{
    float3 camPos;
};


cbuffer LightTransformBuffer : register(b2)
{
    matrix lightView;
    matrix lightProj;
};

struct VSIn
{
    float3 pos          : POSITION;
    float2 tex          : TEXCOORD;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 binormal     : BINORMAL;
};

struct VSOut
{
    float3 camPos       : POSITION0;
    float3 worldPos     : POSITION1;
    float4 lightViewPos : POSITION2;
    float3 W_normal     : NORMAL0;
    float2 tex          : TEXCOORD0;
    float3 tangent      : TANGENT;
    float3 binormal     : BINORMAL;
    float4 pos          : SV_POSITION;
};

VSOut main(VSIn iVS)
{
    VSOut oVS;
    float4 W_Pos4 = mul(float4(iVS.pos, 1.0f), world);
    oVS.pos = mul(W_Pos4, viewProj);
    oVS.worldPos = W_Pos4.xyz;
    oVS.W_normal = normalize(mul(iVS.normal, (float3x3) world));
    oVS.tex = iVS.tex;
    oVS.tangent = normalize(mul(iVS.tangent, (float3x3) world));
    oVS.binormal = normalize(mul(iVS.binormal, (float3x3) world));
    oVS.camPos = camPos;
    
    // pre-calculate light projection
    matrix lightViewProj = mul(lightView, lightProj);
    oVS.lightViewPos = mul(W_Pos4, lightViewProj);

    return oVS;
}