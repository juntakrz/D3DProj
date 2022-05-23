Texture2D tex2D;
SamplerState smplr;

float4 main( float2 tex : TEXCOORD0) : SV_TARGET
{
    return tex2D.Sample(smplr, tex);
}