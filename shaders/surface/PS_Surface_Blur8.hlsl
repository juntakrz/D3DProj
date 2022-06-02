Texture2D tex2D;
SamplerState smplr;

static const int r = 4;     // kernel
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // init values
    float2 texCoord = tex;
    
    // performance saving approximation
    static const float2 pixelStep = { 0.0007, 0.0012 };
    
    // init colors
    float4 accColor = { 0.0, 0.0, 0.0, 0.0 };
    
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

    return accColor / divisor;
}