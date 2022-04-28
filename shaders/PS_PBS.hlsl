cbuffer M_Material : register(b0)
{
    float4 M_Ambient;
    float4 M_F0;
    float1 M_MatIntensity;
    float1 M_Metalness;
    float1 M_Roughness;
};

cbuffer L_DirLight : register(b1)
{
    float3 L_DirPos;
    float1 L_DirInt;
    float4 L_DirDiffuse;
};

cbuffer L_PointLight : register(b2)
{
    float3 L_PLPos[4];
    float4 L_PLDiffuse[4];
    float2 L_PLInt[4];
    uint4 numPLights;
};

#include "include//HPS_PBS.hlsli"

struct PSInput
{
    float3 camPos : POSITION0;
    float3 worldPos : POSITION1;
    float3 W_Normal : NORMAL0;
    float2 tex : TEXCOORD;
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
    float metallic = texMetallic.Sample(smplr, iPS.tex).r * M_Metalness;
    float roughness = texRoughness.Sample(smplr, iPS.tex).r * M_Roughness;
    float ao = texAO.Sample(smplr, iPS.tex).r;
    
    float3 L, radiance, color;
    
    float3 Lo = { 0.0f, 0.0f, 0.0f };
    
    //calculate material composition
    float3 F0 = lerp(M_F0.rgb, albedo, metallic);
    
    //set normal map range from (0, +1) to (-1, +1)
    normalTex = (normalTex * 2.0f) - 1.0f;

    float3 N = normalize(normalTex.x * iPS.tangent + normalTex.y * iPS.binormal + normalTex.z * iPS.W_Normal);
    float3 V = normalize(iPS.camPos - iPS.worldPos); //view vector
    
    for (int i = 0; i < numPLights.x + 1; i++)
    {
        if (i == 0)
        {
            L = normalize(L_DirPos);
            radiance = L_DirDiffuse.rgb * L_DirInt * M_MatIntensity;
        }
        else
        {
            L = normalize(L_PLPos[i - 1] - iPS.worldPos);
            float distance = length(L_PLPos[i - 1] - iPS.worldPos);
            float attenuation = 1.0 / (distance * distance);
            radiance = L_PLDiffuse[i - 1].rgb * attenuation * L_PLInt[i - 1].x;
        }
        float3 H = normalize(V + L);
    
        //Cook-Torrance BRDF
        float NdotV = max(dot(N, V), 0.00000001);        //divide by zero prevention
        float NdotL = max(dot(N, L), 0.00000001);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);
        
        float NDF = DistributionGGX(NdotH, roughness);
        float G = GeometrySmith(NdotV, NdotL, roughness);
        float3 F = FresnelSchlick(HdotV, F0);
    
        //specular formula
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * NdotV * NdotL;
        float3 specular = numerator / denominator;
        
        //conservation of energy
        float3 Kd = float3(1.0, 1.0, 1.0) - F;
        Kd *= 1.0 - metallic;
    
        Lo += (Kd * albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = 0.03 * albedo * M_Ambient.rgb * ao;
    color = ambient + Lo;
    
    color = color / (color + 1.0);
    color = pow(color, 1.0 / 2.2);
    
    return float4(color, baseMap.a);
}