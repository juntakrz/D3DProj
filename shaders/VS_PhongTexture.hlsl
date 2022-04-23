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

struct VSOut
{
    float3 WV_Pos : POSITION;
    float3 W_normal : NORMAL0;
    float3 WV_normal : NORMAL1;
    float2 tex : TEXCOORD;
    float3 viewDir : VECTOR0;
    float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD)
{    
    VSOut vso;    
    vso.pos = mul(float4(pos, 1.0f), mul(world, viewProj));
    vso.WV_Pos = (float3)mul(float4(pos, 1.0f), mul(world, view));
    vso.W_normal = normalize(mul(normal, (float3x3) world));
    vso.WV_normal = normalize(mul(normal, (float3x3) mul(world, view)));
    vso.tex = tex;
    
    float3 worldPos = (float3) mul(float4(pos, 1.0f), world);
    vso.viewDir = normalize(camPos.xyz - worldPos);
    
    return vso;
}