cbuffer M_Material : register(b0)
{
    float4 M_Ambient;
    float1 M_MatIntensity;
    float1 M_SpecIntensity;
    float1 M_SpecPower;
};

cbuffer L_DirLight : register(b1)
{
    float3 L_DirPos;
    float1 L_DirInt; 
    float4 L_DirDiffuse;
};

cbuffer L_PointLight  : register(b2)
{
    float3 L_Pos[4];
    float4 L_Diffuse[4];
    float2 L_Intensity[4];
    uint4 numPLights;
};

struct PSInput
{
    float3 WV_Pos : POSITION0;
    float3 camPos : POSITION1;
    float3 worldPos : POSITION2;
    float3 W_Normal : NORMAL0;
    float3 WV_Normal : NORMAL1;
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

static const float PI = 3.14159265359;

//PBS functions
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
//--------------------------------------

float4 main(PSInput iPS) : SV_TARGET
{
    //init values
    float3 albedo = { 0.5f, 0.0f, 0.0f };
    float metallic = 0.4f;
    float roughness = 0.33f;
    float ao = 1.0f;
    
    float3 lpos = { -12.0f, 15.0f, 25.0f };
    float3 lcolor = { 1.0, 1.0, 1.0 };
    
    float3 N = iPS.W_Normal; //normal
    float3 V = normalize(iPS.camPos - iPS.worldPos); //view vector
    
    float3 Lo = { 0.0f, 0.0f, 0.0f };
    float3 color = { 0.0f, 0.0f, 0.0f };
    float3 F0 = { 0.4, 0.4, 0.4 };
    F0 = lerp(F0, albedo, metallic);
    
    //float4 albedo = pow(texDiffuse.Sample(smplr, iPS.tex), 2.2);
    //float3 N = texNormal.Sample(smplr, iPS.tex);
    //float4 tMetallic = texMetallic.Sample(smplr, iPS.tex);
    //float4 tRoughness = texRoughness.Sample(smplr, iPS.tex);
    //float4 tAO = texAO.Sample(smplr, iPS.tex);
    
    lpos = L_DirPos * 15.0f;
    
    if (numPLights.x > 0)
    {
        float3 L = normalize(lpos - iPS.worldPos);              //light vector
        float3 H = normalize(V + L);                            //halfway vector
    
        float distance = length(lpos - iPS.worldPos);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = L_DirDiffuse * attenuation * L_DirInt * 200.0;
    
        //weird math shennanigans
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        //float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        float3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    
        //specular formula
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;
        
        //optimize this
        float3 Ks = F;
        float3 Kd = float3(1.0, 1.0, 1.0) - Ks;
        Kd *= 1.0 - metallic;
    
        float NdotL = max(dot(N, L), 0.0);
        Lo += (Kd * albedo / PI + specular) * radiance * NdotL;
    
        float3 ambient = 0.0003 * albedo * ao;
        color = ambient + Lo;      
    }
    
    color = color / (color + 1.0);
    color = pow(color, 1.0 / 2.2);
    
    return float4(color, 1.0);
}