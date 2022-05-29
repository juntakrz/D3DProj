Texture2D texDiffuse : register(t0);
Texture2D texLens : register(t1);
SamplerState smplr;

float4 main(float2 tex : TEXCOORD) : SV_TARGET
{
    float3 color = texDiffuse.Sample(smplr, tex);
    
    // add chromatic abberation to lens dust
    float1 red = texLens.Sample(smplr, float2(tex.x + 0.002, tex.y + 0.002));
    float1 green = texLens.Sample(smplr, tex).rgb;
    float1 blue = texLens.Sample(smplr, float2(tex.x - 0.002, tex.y - 0.002));
    float1 alpha = texDiffuse.Sample(smplr, tex).a;
    
    // add lens dirt to final output
    color += float3(red, green, blue) * color * 0.75;
        
    return float4(color, alpha);
}