Texture2D tex2D : register(t0);
SamplerState smplr : register(s0);

static const int r = 8;     // kernel
static const float divisor = (2 * r + 1) * (2 * r + 1);

float3 jodieReinhardTonemap(float3 c)
{
    float l = dot(c, float3(0.2126, 0.7152, 0.0722));
    float3 tc = c / (c + 1.0);

    return lerp(c / (l + 1.0), tc, tc);
}

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // init values
    float2 texCoord = tex;
    
    // performance saving approximation
    static const float2 pixelStep = { 0.0022, 0.0040 };
    
    // init colors
    float4 accColor = { 0.0, 0.0, 0.0, 0.0 };
    float4 mainColor = tex2D.Sample(smplr, tex);
    
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

    //return mainColor + float4(accColor.rgb / divisor * 0.8, 0.0);
    //float1 scalar = accColor / (divisor * 0.5);
    float3 finalColor;

    finalColor = mainColor.rgb + (accColor.rgb / divisor);

    //finalColor = jodieReinhardTonemap(finalColor);
    return float4(finalColor, 1.0);
}