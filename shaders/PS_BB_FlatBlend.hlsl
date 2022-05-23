Texture2D tex2D;
SamplerState smplr;

float4 main(float2 tex : TEXCOORD0) : SV_TARGET
{
    float4 albedo = tex2D.Sample(smplr, tex);
    
    float1 alpha = (albedo.r + albedo.g + albedo.b) / 3.0f * 1.35f;
    
    return float4(albedo.r, albedo.g, albedo.b, alpha);
}