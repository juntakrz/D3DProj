#include "include/HVS_Transform.hlsli"

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_POSITION;
};

VSOut main( float3 pos : POSITION, float2 tex : TEXCOORD )
{	
	VSOut oVS;
    oVS.pos = mul(float4(pos, 1.0f), mul(world, viewProj));
	oVS.tex = tex;
	return oVS;
}