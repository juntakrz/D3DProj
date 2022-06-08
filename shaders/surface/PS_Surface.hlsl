Texture2D texDiffuse    : register(t0);
SamplerState smplr      : register(s0);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{    
    return texDiffuse.Sample(smplr, tex);
}