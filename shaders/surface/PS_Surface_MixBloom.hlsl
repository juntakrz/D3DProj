cbuffer variables       : register(b0)
{
    float1 m;           // bloom sampling pow
};

Texture2D texMain       : register(t0);
Texture2D texBloom      : register(t1);
Texture2D texLens       : register(t2);
SamplerState smplr      : register(s0);

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // sample main buffer
    float3 mainColor = pow(texMain.Sample(smplr, tex).rgb, 2.2);
    
    // sample bloom buffer
    float3 bloomColor = pow(texBloom.Sample(smplr, tex).rgb, m);
    
    // add chromatic abberation to lens dust
    float1 lensRed = pow(texLens.Sample(smplr, float2(tex.x + 0.001, tex.y + 0.001)), 2.2);
    float1 lensGreen = pow(texLens.Sample(smplr, tex).rgb, 2.2);
    float1 lensBlue = pow(texLens.Sample(smplr, float2(tex.x - 0.001, tex.y - 0.001)), 2.2);
    
    float3 lensColor = float3(lensRed, lensGreen, lensBlue) * bloomColor * 8.0;
    
    float3 finalColor = mainColor + bloomColor + lensColor;
    finalColor = pow(finalColor, 1.0 / 2.2);
    
    return float4(finalColor, 1.0);
}