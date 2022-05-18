#include "include//HPS_PBS_H.hlsli"
#include "include//HPS_PBS.hlsli"

struct PSInput
{
    float3 camPos : POSITION0;
    float3 worldPos : POSITION1;
    float4 lightViewPos : POSITION2;
    float3 W_Normal : NORMAL0;
    float2 tex : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
Texture2D texMetallic : register(t2);
Texture2D texRoughness : register(t3);
Texture2D texAO : register(t4);
Texture2D texDepth : register(t6);
SamplerState smplr : register(s0);
SamplerComparisonState smplrClamp : register(s6);

float1 CalculateShadow(in float4 lightViewPos, in float1 bias)
{
    float3 projCoord;
    float1 projDepth;
    float1 L_Depth;
    float1 shadow = 0.0f;
    float1 texelSize = 0.0007f; // aspect-incorrect approximation for 1080p

    projCoord.x = lightViewPos.x / lightViewPos.w / 2.0f + 0.5f;
    projCoord.y = -lightViewPos.y / lightViewPos.w / 2.0f + 0.5f;
    
    // if saturated coords are within 0 - 1 range - the pixel is lit
    if (saturate(projCoord.x) == projCoord.x && saturate(projCoord.y) == projCoord.y)
    {
        // determine if pixel is behind an object
        // calculate light depth and subtract bias to prevent shader precision errors
        L_Depth = (lightViewPos.z / lightViewPos.w) - bias;
        
        for (int x = -2; x <= 2; x++)
        {
            for (int y = -2; y <= 2; y++)
            {
        // sample the depth value from depth buffer texture
                shadow += texDepth.SampleCmpLevelZero(smplrClamp, projCoord.xy + float2(x, y) * texelSize, L_Depth);
            }
        }
        
        return shadow / 25.0f;
    }
    else
    {
        return 1.0f;
    }
}

float4 main(PSInput iPS) : SV_TARGET
{
    //init values    
    float4 baseMap = pow(texDiffuse.Sample(smplr, iPS.tex), 2.2);
    float3 albedo = baseMap.rgb;
    float3 normalTex = texNormal.Sample(smplr, iPS.tex);
    float1 metallic = texMetallic.Sample(smplr, iPS.tex).r * M_Metalness;
    float1 roughness = texRoughness.Sample(smplr, iPS.tex).r * M_Roughness;
    float1 ao = texAO.Sample(smplr, iPS.tex).r;
    
    float1 shadowBias = 0.0001f;
    float1 shadow = CalculateShadow(iPS.lightViewPos, shadowBias);
    
    float3 L, radiance, color;
    
    float3 Lo = { 0.0f, 0.0f, 0.0f };
    
    //calculate material composition
    float3 F0 = lerp(M_F0.rgb, albedo, metallic);
    
    //set normal map range from (0, +1) to (-1, +1)
    normalTex = (normalTex * 2.0f) - 1.0f;

    float3 N = normalize(normalTex.x * iPS.tangent + normalTex.y * iPS.binormal + normalTex.z * M_BumpIntensity * iPS.W_Normal);
    float3 V = normalize(iPS.camPos - iPS.worldPos); //view vector
    
    for (uint i = 0; i < numPLights.x + 1; i++)
    {
        if (i == 0)
        {
            L = normalize(L_DirPos);
            radiance = L_DirDiffuse.rgb * L_DirInt * M_MatIntensity * shadow;
        }
        else
        {
            L = normalize(L_PLPos[i - 1] - iPS.worldPos);
            float1 distance = length(L_PLPos[i - 1] - iPS.worldPos);
            float1 attenuation = 1.0 / (distance * distance);
            radiance = L_PLDiffuse[i - 1].rgb * attenuation * L_PLInt[i - 1].x;
        }
        float3 H = normalize(V + L); //half vector between view and light
    
        //Cook-Torrance BRDF
        float1 NdotV = max(dot(N, V), 0.00000001f); //prevent divide by zero
        float1 NdotL = max(dot(N, L), 0.00000001f);
        float1 NdotH = max(dot(N, H), 0.0f);
        float1 HdotV = max(dot(H, V), 0.0f);
        
        float1 NDF = DistributionGGX(NdotH, roughness);
        float1 G = GeometrySmith(NdotV, NdotL, roughness);
        float3 F = FresnelSchlick(HdotV, F0);
    
        float3 numerator = NDF * G * F;
        float1 denominator = 4.0f * NdotV * NdotL;
        float3 specular = numerator / denominator;
        
        //conservation of energy, must not reflect more light than receives
        float3 Kd = (1.0f - F) * (1.0f - metallic);
    
        Lo += (Kd * albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = 0.03f * albedo * M_Ambient.rgb * ao;
    color = ambient + Lo;
    
    color = color / (color + 1.0f);
    color = pow(color, 1.0f / 2.2f);
    
    return float4(color, baseMap.a);
}