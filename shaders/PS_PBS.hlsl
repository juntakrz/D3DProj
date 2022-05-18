#include "include//HPS_PBS_H.hlsli"
#include "include//HPS_PBS.hlsli"

struct PSInput
{
    float3 camPos : POSITION0;
    float3 worldPos : POSITION1;
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
SamplerState smplr;

float4 main(PSInput iPS) : SV_TARGET
{
    //init values    
    float4 baseMap = pow(texDiffuse.Sample(smplr, iPS.tex), 2.2);
    float3 albedo = baseMap.rgb;
    float3 normalTex = texNormal.Sample(smplr, iPS.tex);
    float1 metallic = texMetallic.Sample(smplr, iPS.tex).r * M_Metalness;
    float1 roughness = texRoughness.Sample(smplr, iPS.tex).r * M_Roughness;
    float1 ao = texAO.Sample(smplr, iPS.tex).r;
    
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
            radiance = L_DirDiffuse.rgb * L_DirInt * M_MatIntensity;
        }
        else
        {
            L = normalize(L_PLPos[i - 1] - iPS.worldPos);
            float1 distance = length(L_PLPos[i - 1] - iPS.worldPos);
            float1 attenuation = 1.0 / (distance * distance);
            radiance = L_PLDiffuse[i - 1].rgb * attenuation * L_PLInt[i - 1].x;
        }
        float3 H = normalize(V + L);        //half vector between view and light
    
        //Cook-Torrance BRDF
        float1 NdotV = max(dot(N, V), 0.00000001f);        //prevent divide by zero
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