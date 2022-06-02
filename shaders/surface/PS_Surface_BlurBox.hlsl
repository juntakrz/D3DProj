cbuffer data : register(b0)
{
    float2 dir;
};

Texture2D tex2D : register(t0);
Texture2D<float> texDepth : register(t1);
Texture2D<uint2> texStencil : register(t1);
SamplerState smplr : register(s0);

static const int r = 4; // kernel
static const int divisor = (r * 2 + 1);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // performance saving approximation
    static const float2 pixelStep = { 0.0007, 0.0012 };
    
    float4 accColor = tex2D.Sample(smplr, tex);
    
    // sample horizontal or vertical depending on direction
    for (int i = 1; i <= r; i++)
    {
        accColor += tex2D.Sample(smplr, tex + float2(pixelStep.x * i * dir.x, pixelStep.y * dir.y * i))
            + tex2D.Sample(smplr, tex - float2(pixelStep.x * i * dir.x, pixelStep.y * dir.y * i));
    }

    return accColor / divisor;
}