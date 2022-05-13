#include "include/HVS_Transform.hlsli"

struct VSIn
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_POSITION;
};

VSOut main( VSIn iVS )
{	
	VSOut oVS;
    oVS.pos = mul(float4(iVS.pos, 1.0f), mul(world, viewProj));
	oVS.tex = iVS.tex;
	return oVS;
}