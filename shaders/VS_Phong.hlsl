cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 pos : SV_POSITION;
};

VSOut main( float3 pos : POSITION, float3 normal : NORMAL )
{
    VSOut vso;
    vso.worldPos = (float3)mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(normal, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}