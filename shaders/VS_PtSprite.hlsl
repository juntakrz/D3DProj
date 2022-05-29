#include "include/HVS_Transform.hlsli"

struct VSOut
{
    float3 pos          : POSITION0;
    float2 scale        : TEXCOORD0;
    matrix transform    : MATRIX0;
};

VSOut main( float3 pos : POSITION )
{
    VSOut oVS;
    oVS.pos = mul(float4(pos, 1.0), mul(world, view)).xyz;
    oVS.transform = proj;
    oVS.scale = float2(world[0][0], world[1][1]);   // retrieve scaling from world matrix
	return oVS;
}