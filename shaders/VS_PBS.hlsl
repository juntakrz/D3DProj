cbuffer TransformBuffer
{
    matrix world;
    matrix view;
    matrix viewProj;
};

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
    float3 WV_Pos : POSITION0;
    float3 camPos : POSITION1;
    float3 worldPos : POSITION2;
    float3 W_normal : NORMAL0;
    float3 WV_normal : NORMAL1;
    float2 tex : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 pos : SV_POSITION;
};

VSOut main(VSIn iVS)
{
    VSOut oVS;
    oVS.pos = mul(float4(iVS.pos, 1.0f), mul(world, viewProj));
    oVS.WV_Pos = (float3) mul(float4(iVS.pos, 1.0f), mul(world, view));
    oVS.W_normal = normalize(mul(iVS.normal, (float3x3) world));
    oVS.WV_normal = normalize(mul(iVS.normal, (float3x3) mul(world, view)));
    oVS.tex = iVS.tex;
    oVS.tangent = normalize(mul(iVS.tangent, (float3x3) world));
    oVS.binormal = normalize(mul(iVS.binormal, (float3x3) world));
    oVS.worldPos = (float3) mul(float4(iVS.pos, 1.0f), world);
    oVS.camPos = camPos;

    return oVS;
}