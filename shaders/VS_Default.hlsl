cbuffer CBuf
{
    matrix world;
	matrix view;
    matrix viewProj;
};

float4 main( float3 pos : POSITION ) : SV_Position
{
    return mul(float4(pos, 1.0), mul(world, viewProj));
}