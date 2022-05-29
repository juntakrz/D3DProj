Texture2D texDiffuse    : register(t0);
SamplerState smplr      : register(s0);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    //float3 albedo = texDiffuse.Sample(smplr, tex);
    //return float4(albedo, 1.0);
    
    return texDiffuse.Sample(smplr, tex);
}