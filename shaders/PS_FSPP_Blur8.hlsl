Texture2D tex2D;
SamplerState smplr;

static const int r = 3;
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // init values
    float2 texCoord = tex;
    
    // get texture pixel step
    uint2 renderRes;
    tex2D.GetDimensions(renderRes.x, renderRes.y);
    const float2 pixelStep = { 1.0 / renderRes.x, 1.0 / renderRes.y };
    
    float1 mult = 0.0;
    
    // accumulated color
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
    
    return float4(accColor.rgb / divisor, 1.0);
}