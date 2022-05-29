cbuffer data        : register(b0)
{
    float2 dir;
};

Texture2D tex2D     : register(t0);
SamplerState smplr  : register(s0);

static const int r = 4; // kernel

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{  
    const float weight[] =
    {
        0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162
    };
    
    // init values
    float2 texCoord = tex;
    
    // performance saving approximation
    static const float2 pixelStep = { 0.0018, 0.0032 };
    
    // init colors
    float4 thisColor = tex2D.Sample(smplr, tex) * weight[0];
    float4 accColor = { 0.0, 0.0, 0.0, 0.0 };
    
    // sample horizontal or vertical depending on direction
    for (int i = 1; i <= r; i++)
    {
        accColor += tex2D.Sample(smplr, tex + float2(pixelStep.x * i * dir.x, pixelStep.y * dir.y * i)) * weight[i]
            + tex2D.Sample(smplr, tex - float2(pixelStep.x * i * dir.x, pixelStep.y * dir.y * i)) * weight[i];
    }

    accColor += thisColor;
    return accColor;
}