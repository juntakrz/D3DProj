cbuffer CBuf
{
    matrix world;
    matrix view;
    matrix viewProj;
};

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_POSITION;
};

VSOut main( float3 pos : POSITION, float2 tex : TEXCOORD )
{	
	VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), mul(world, viewProj));
	vso.tex = tex;
	return vso;
}