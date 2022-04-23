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
    float3 WV_Pos : POSITION;
    float3 W_normal : NORMAL0;
    float3 WV_normal : NORMAL1;
    float2 tex : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDir : VECTOR0;
    float4 pos : SV_POSITION;
};

VSOut main(VSIn vsi)
{
    VSOut vso;
    vso.pos = mul(float4(vsi.pos, 1.0f), mul(world, viewProj));
    vso.WV_Pos = (float3)mul(float4(vsi.pos, 1.0f), mul(world, view));
    vso.W_normal = normalize(mul(vsi.normal, (float3x3) world));
    vso.WV_normal = normalize(mul(vsi.normal, (float3x3) mul(world, view)));
    vso.tex = vsi.tex;
    vso.tangent = normalize(mul(vsi.tangent, (float3x3) world));
    vso.binormal = normalize(mul(vsi.binormal, (float3x3) world));

    float3 worldPos = (float3) mul(float4(vsi.pos, 1.0f), world);
    vso.viewDir = normalize(camPos.xyz - worldPos);

    return vso;
}