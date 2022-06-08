cbuffer data        : register(b0)
{
    float2 dir;
};

cbuffer gauss : register(b2)
{
    // 0 = kernel size, 1 - 9 = kernel weights
    float gaussData[10];
}

Texture2D tex2D     : register(t0);
SamplerState smplr  : register(s0);

static const int r = (int)gaussData[0];     // kernel

float4 main(float2 tex : TEXCOORD, float4 screenCoord : SV_POSITION) : SV_TARGET
{
    // performance saving approximation
    static const float2 pixelStep = { 0.0012, 0.0022 };        // 0.0018, 0.0032

    // init colors
    float4 accColor = tex2D.Sample(smplr, tex) * gaussData[1];  // core pixel weight
    
    // sample horizontal or vertical depending on direction
    for (int i = 1; i <= r; i++)
    {
        accColor += tex2D.Sample(smplr, tex + float2(pixelStep.x * i * dir.x, pixelStep.y * dir.y * i)) * gaussData[i + 1]
            + tex2D.Sample(smplr, tex - float2(pixelStep.x * i * dir.x, pixelStep.y * dir.y * i)) * gaussData[i + 1];
    }
    
    // bluring makes colors washed out, trying to bring some original color back
    float1 fGamma = 1.0 + (1.0 - accColor.a) * 0.7;
    accColor = pow(accColor, float4(1.0 / fGamma, 1.0 / fGamma, 1.0 / fGamma, 1.0 / fGamma));
    
    return accColor;
}