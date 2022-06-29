cbuffer variables       : register(b0)
{
    float1 m;           // interpolation coefficient
};

Texture2D texMain       : register(t0);
Texture2D texBloom      : register(t1);
Texture2D texLens       : register(t2);
SamplerState smplr      : register(s0);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // sample main buffer
    float3 mainColor = texMain.Sample(smplr, tex).rgb;
    
    // sample bloom buffer
    float3 bloomColor = texBloom.Sample(smplr, tex).rgb;
    
    // add chromatic abberation to lens dust
    float1 lensRed = texLens.Sample(smplr, float2(tex.x + 0.001, tex.y + 0.001));
    float1 lensGreen = texLens.Sample(smplr, tex).rgb;
    float1 lensBlue = texLens.Sample(smplr, float2(tex.x - 0.001, tex.y - 0.001));
    
    // average bloom intensity as opposite of main color times 'm'
    float1 bloomIntensity = ((1.0 - mainColor.r * m) + (1.0 - mainColor.g * m) + (1.0 - mainColor.b * m)) / 3.0;
    
    // blend bloom color with lens dust
    bloomColor = bloomColor * max(0.0, bloomIntensity) + float3(lensRed, lensGreen, lensBlue) * bloomColor * 0.5;
    
    // output final color
    return float4(mainColor + bloomColor * m, 1.0);
}