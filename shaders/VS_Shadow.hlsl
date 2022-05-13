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
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 W_Normal : NORMAL0;
    float3 worldPos : POSITION0;
    float4 lightViewPos : POSITION1;
};

VSOut main( VSIn iVS )
{
    VSOut oVS;
    float4 W_Pos4 = mul(float4(iVS.pos, 1.0f), world);
    oVS.pos = mul(W_Pos4, mul(world, viewProj));
    oVS.worldPos = W_Pos4.xyz;
    oVS.tex = iVS.tex;
    oVS.W_Normal = normalize(mul(iVS.normal, (float3x3) world));
    
    // pre-calculate light projection
    matrix lightViewProj = mul(lightView, lightProj);
    oVS.lightViewPos = mul(W_Pos4, lightViewProj);
    
	return oVS;
}