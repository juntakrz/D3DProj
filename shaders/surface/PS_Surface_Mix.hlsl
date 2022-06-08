// mixes 'standard' layer and 'blur' layer using depths of both to determine if a pixel from blur layer is drawn

struct PSIn
{
    float2 texCoord     : TEXCOORD0;
    float4 screenCoord  : SV_POSITION;
};

struct PSOut
{
    float4 color        : SV_TARGET;
    float1 depth        : SV_DEPTH;
};

Texture2D renderTex1    : register(t0);
Texture2D renderTex2    : register(t1);
Texture2D depthTex1	    : register(t2);
Texture2D depthTex2	    : register(t3);
SamplerState smplr		: register(s0);

PSOut main(PSIn iPS)
{
    // init
    PSOut oPS;

    // sample colors
    float4 color1 = renderTex1.Sample(smplr, iPS.texCoord);
    float4 color2 = renderTex2.Sample(smplr, iPS.texCoord);
    
    // sample depths
    float1 depth1 = depthTex1.Load(int3(iPS.screenCoord.x, iPS.screenCoord.y, 0));
    float1 depth2 = depthTex2.Load(int3(iPS.screenCoord.x, iPS.screenCoord.y, 0));

    // compare depth and determine if blur layer pixel is used
    oPS.color = (depth1 < depth2) ? color1 : float4(lerp(color1.rgb, color2.rgb, color2.a), 1.0);
    oPS.depth = (depth1 < depth2) ? depth1 : depth2;
    
    oPS.color.rgb *= 1.32;      // compensate lost gamma during the blur pass
    
    return oPS;
}