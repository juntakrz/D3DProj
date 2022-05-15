Texture2D tex2D;
SamplerState smplr;

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    float3 color = tex2D.Sample(smplr, tex);
    return float4(color, 1.0);
}