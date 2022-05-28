#include "include/HVS_Transform.hlsli"

struct VSIn
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct VSOut
{
    float2 tex : TEXCOORD0;
    float4 pos : SV_POSITION;
};

VSOut main(VSIn iVS)
{
    VSOut oVS;
    
    // retrieve world matrix's scale to re-apply in billboard-style world * view matrix
    float3 scale = { world[0][0], world[1][1], world[2][2] };
    
    matrix xm = mul(world, view);
    xm[0][0] = scale.x;
    xm[0][1] = 0.0f;    // comment for cylindrical
    xm[0][2] = 0.0f;
    xm[1][0] = 0.0f;
    xm[1][1] = scale.y; // comment for cylindrical
    xm[1][2] = 0.0f;
    xm[2][0] = 0.0f;
    xm[2][1] = 0.0f;    // comment for cylindrical
    xm[2][2] = scale.z;
    oVS.pos = mul(float4(iVS.pos, 1.0f), mul(xm, proj));
    oVS.tex = iVS.tex;
    return oVS;
}