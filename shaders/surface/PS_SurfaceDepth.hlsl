// visualization of depth buffer

Texture2D tex2D;
SamplerState smplr;

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // linear
    float1 color = 0.0001 / (1.0001 - tex2D.Sample(smplr, tex).r);
    
    // exponential 
    //float1 color = tex2D.Sample(smplr, tex).r;
    return float4(color, color, color, 1.0);
}