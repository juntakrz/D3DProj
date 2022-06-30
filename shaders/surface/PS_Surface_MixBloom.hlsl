cbuffer variables       : register(b0)
{
    float1 m;           // bloom sampling pow
};

Texture2D texMain       : register(t0);
Texture2D texBloom      : register(t1);
Texture2D texLens       : register(t2);
SamplerState smplr      : register(s0);

float3 jodieReinhardTonemap(float3 c)
{
    float1 l = dot(c, float3(0.2126, 0.7152, 0.0722));
    float3 tc = c / (c + 1.0);

    return lerp(c / (l + 1.0), tc, tc);
}

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    // sample main buffer
    float3 mainColor = pow(texMain.Sample(smplr, tex).rgb, 2.2);
    
    // sample bloom buffer with externally provided bloom power variable
    float3 bloomColor = pow(texBloom.Sample(smplr, tex).rgb, m);
    
    // add chromatic abberation to lens dust
    float1 lensRed = pow(texLens.Sample(smplr, float2(tex.x + 0.001, tex.y + 0.001)), 2.2);
    float1 lensGreen = pow(texLens.Sample(smplr, tex).rgb, 2.2);
    float1 lensBlue = pow(texLens.Sample(smplr, float2(tex.x - 0.001, tex.y - 0.001)), 2.2);
    
    // calculate lens dust color
    float3 lensColor = float3(lensRed, lensGreen, lensBlue) * bloomColor * 8.0;
    
    // mix all colors, tonemap and bring them into LDR space
    float3 finalColor = mainColor + bloomColor + lensColor;
    finalColor = pow(jodieReinhardTonemap(finalColor), 1.0 / 2.2);
    
    // add tonemap makeup gain to final color
    return float4(finalColor * 1.16, 1.0);
}