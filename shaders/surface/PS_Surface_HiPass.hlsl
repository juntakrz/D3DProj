Texture2D tex2D : register(t0);
SamplerState smplr : register(s0);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    float4 mainColor = tex2D.Sample(smplr, tex);
    float1 intens = (mainColor.r + mainColor.g + mainColor.b) / 3.0;
    
    return (intens > 0.74) ? mainColor : float4(0.0, 0.0, 0.0, 0.0);  //0.65
}