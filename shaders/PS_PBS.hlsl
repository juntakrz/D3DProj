cbuffer M_Material              //slot 0
{
    float4 M_Ambient;
    float1 M_MatIntensity;
    float1 M_SpecIntensity;
    float1 M_SpecPower;
};

cbuffer L_DirLight              //slot 1
{
    float3 L_DirPos;
    float1 L_DirInt; 
    float4 L_DirDiffuse;
};

cbuffer L_PointLight            //slot 2
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
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
//--------------------------------------

float4 main(PSInput iPS) : SV_TARGET
{
    //init values
    float4 globalDiffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
    float4 tDiffuse = texDiffuse.Sample(smplr, iPS.tex);
    float4 tMetallic = texMetallic.Sample(smplr, iPS.tex);
    float4 tRoughness = texRoughness.Sample(smplr, iPS.tex);
    float4 tAO = texAO.Sample(smplr, iPS.tex);
    
    //calculate view vector
	float3 V = normalize(iPS.camPos.xyz - iPS.worldPos);
    float3 L = L_DirPos - iPS.worldPos;
    float3 H = V + L;
    
    //weird math shennanigans
    float3 F0 = { 0.04, 0.04, 0.04 };
    F0 = lerp(F0, tDiffuse.rgb, tMetallic.rgb);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    float NDF = DistributionGGX(iPS.W_Normal, H, tRoughness.r);
    float G = GeometrySmith(iPS.W_Normal, V, L, tRoughness.r);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(iPS.W_Normal, V), 0.0) * max(dot(iPS.W_Normal, L), 0.0) + 0.0001;
    float3 specular = numerator / denominator;
    
    //calculate directional light intensity
    float1 L_DirIntensity = saturate(dot(iPS.W_Normal, normalize(L_DirPos))) * L_DirInt;
    
    
    //add directional light
    globalDiffuse += L_DirDiffuse * L_DirIntensity * M_MatIntensity;
    
    return tDiffuse * globalDiffuse;
}