Texture2D tex2D : register(t0);
SamplerState smplr : register(s0);

static const int r = 4;     // kernel
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // init values
    float2 texCoord = tex;
    
    // performance saving approximation
    static const float2 pixelStep = { 0.0042, 0.0072 };
    
    // init colors
    float4 accColor = { 0.0, 0.0, 0.0, 0.0 };
    float4 mainColor = tex2D.Sample(smplr, tex);
    float1 intens = (mainColor.r + mainColor.g + mainColor.b) / 3.0;
    
    // sample neighboring pixels
    /* sample intensity = 1.0 - loop step * multiplier
    */
    for (int1 y = -r; y <= r; y++)
    {
        for (int1 x = -r; x <= r; x++)
        {
            texCoord = tex + float2(x * pixelStep.x, y * pixelStep.y);
            accColor += tex2D.Sample(smplr, texCoord);
        }
    }

    return mainColor + float4(accColor.rgb / divisor * 0.8, 0.0);
}