#include "include/HVS_Transform.hlsli"

struct VSOut
{
    float3 pos : POSITION0;
    matrix transform : MATRIX0;
};

VSOut main(float3 pos : POSITION)
{
    VSOut oVS;
    oVS.pos = mul(float4(pos, 1.0), mul(world, view)).xyz;
    oVS.transform = proj;
    return oVS;
}