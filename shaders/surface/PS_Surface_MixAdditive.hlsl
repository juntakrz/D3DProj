struct PSIn
{
    float2 texCoord : TEXCOORD0;
};

struct PSOut
{
    float4 color : SV_TARGET;
};

Texture2D renderTex1 : register(t0);
Texture2D renderTex2 : register(t1);
SamplerState smplr : register(s0);

PSOut main(PSIn iPS)
{
    // init
    PSOut oPS;

    // sample colors
    float4 color1 = renderTex1.Sample(smplr, iPS.texCoord);
    float4 color2 = renderTex2.Sample(smplr, iPS.texCoord);

    // compare depth and determine if blur layer pixel is used
    oPS.color = float4(color1.rgb + color2.rgb, 1.0);
    
    return oPS;
}