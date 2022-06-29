Texture2D tex2D         : register(t0);
Texture2D texLens       : register(t1);
SamplerState smplr      : register(s0);

float Noise1(float n)
{
    return frac(sin(n) * 43758.5453123);
}
float Noise2(float2 n)
{
    return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
}

float3 LensFlare(float2 uv, float2 pos)
{
    float2 main = uv - pos;
    float2 uvd = uv * (length(uv));
	
    // calculate noise
    float angle = atan2(main.x, main.y);
    float dist = length(main);
    float n = max(0.55, Noise2(float2(angle, dist)));

    // flare 0 (central flare * intensity, drawn at sun pos converted to UV coords)
    float f0 = 1.0 / (length(uv - pos) * 32.0 + 1.0) * 1.2;
    
    // converting coordinates from UV 0...1 to screen space -1 ... 1 for secondary flares
    uv = uv * 2.0 - 1.0;
    pos = pos * 2.0 - 1.0;
    
    // flare 1 (largest closest)
    float2 uvx = lerp(uv, uvd, -0.1);
	
    float f1r = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), .0) * 2.0;
    float f1g = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), .0) * 2.0;
    float f1b = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), .0) * 2.0;
	
    // flare 2 (small circular)
    float f2r = max(0.01 - pow(length(uvx - 0.3 * pos), 1.6), .0) * 6.0;
    float f2g = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), .0) * 3.0;
    float f2b = max(0.01 - pow(length(uvx - 0.35 * pos), 1.6), .0) * 5.0;
	
    // flare 3 (larger circular)
    float f3r = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), .0) * 6.0;
    float f3g = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), .0) * 5.0;
    float f3b = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), .0) * 3.0;
    
    // flare 4 (border flare)
    uvd = uv * (length(uv));
    
    float f4r = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), 0.0) * 0.25;
    float f4g = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), 0.0) * 0.23;
    float f4b = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), 0.0) * 0.21;
	
    float3 c = float3(0.0, 0.0, 0.0);
	
    c.r += f1r + f2r + f3r + f4r;
    c.g += f1g + f2g + f3g + f4g;
    c.b += f1b + f2b + f3b + f4b;
    
    c = c * 1.5 - float3(length(uvd) * 0.05, length(uvd) * 0.05, length(uvd) * 0.05) * n;
    c += f0;
	
    return c;
}

float3 cc(float3 color, float factor, float factor2) // color modifier
{
    float w = color.x + color.y + color.z;
    return lerp(color, w * factor, w * factor2);
}

float4 main(float2 tex : TEXCOORD0, float1 inAlpha : TEXCOORD1, float2 fpos : TEXCOORD2) : SV_TARGET
{
    float3 color = float3(1.1, 0.9, 0.6) * LensFlare(tex, fpos);
    
    color -= Noise2(fpos.xy) * 0.02; // light shimmer
    color = cc(color, 0.5, 0.1);
    
    // calculate smooth radial alpha using vector length from sun pos to fragment pos
    float1 dist = length(tex - fpos);    
    float alpha = lerp(0.0, 1.0, 1.0 - dist);
    
    // center textured flare at sun's position
    float2 coords = fpos + 0.5;
    float4 albedo = float4(color, 1.0) + tex2D.Sample(smplr, coords - tex) * 0.5;
    
    // add lens dust effect to flare
    float3 lensColor = texLens.Sample(smplr, tex).rgb * albedo.rgb * 1.5;
    
    return float4(albedo.rgb + lensColor, alpha * inAlpha);
}