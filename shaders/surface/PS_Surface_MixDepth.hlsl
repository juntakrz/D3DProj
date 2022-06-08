// mixes two depth buffers into a single one

struct PSIn
{
    float2 texCoord     : TEXCOORD0;
    float4 screenCoord  : SV_POSITION;
};

struct PSOut
{
    float1 depth : SV_DEPTH;
};

Texture2D mainDepthTex : register(t0);
Texture2D blurDepthTex : register(t1);

PSOut main(PSIn iPS)
{
    // init
    PSOut oPS;
    
    // get depth values
    float1 mainDepth = mainDepthTex.Load(int3(iPS.screenCoord.xy, 0)).r;
    float1 blurDepth = blurDepthTex.Load(int3(iPS.screenCoord.xy, 0)).r;
    
    // compare and return the correct depth value (less-equal mode)
    oPS.depth = (mainDepth <= blurDepth) ? mainDepth : blurDepth;
    
    return oPS;
}