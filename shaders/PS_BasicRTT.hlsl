Texture2D tex2D;

SamplerState smplr;

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    return float4(tex2D.Sample(smplr, tex).rgb, 1.0f);
}