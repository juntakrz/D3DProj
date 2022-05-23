// visualization of depth buffer

Texture2D tex2D;
SamplerState smplr;

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // linear
    //float1 color = 0.0001 / (1.0001 - tex2D.Sample(smplr, tex).r);
    
    // exponential 
    //float1 color = tex2D.Sample(smplr, tex).r;
    
    // exponential inverted
    //float1 color = 1.0 - tex2D.Sample(smplr, tex).r;
    
    //float2 coords = tex;
    //coords.x = (coords.x / 4.0) + 0.5;
    
    // color coded
    float1 color = tex2D.Sample(smplr, tex).r;
    
    float1 red = color * 2.0f;
    if(red > 1.0f)
    {
        red = 1.0f - (red - 1.0f);
    }
    float1 green = (color - 0.2f) / 0.6f * 2.0f;
    if(green > 1.0f)
    {
        green = 1.0f - (green - 1.0f);
    }
    float1 blue = 1.0f - color * 2.0f;
    if(blue < 0.0f)
    {
        blue = -blue;
    }
    
    return float4(red, green, blue, 1.0f);
}