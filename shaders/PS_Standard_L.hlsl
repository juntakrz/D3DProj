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

cbuffer L_PointLight : register(b2)
{
    float3 L_PLPos[4];
    float4 L_PLDiffuse[4];
    float2 L_PLInt[4];
    uint4  numPLights;
};

struct PSInput
{
    float3 worldPos : POSITION0;
    float3 W_Normal : NORMAL0;
    float2 tex : TEXCOORD0;
    float3 viewDir : VECTOR0;
};

Texture2D tex2D : register(t0);
SamplerState smplr;

//Phong-only Standard shader
float4 main(PSInput iPS) : SV_TARGET
{
    //retrieving texture pixel
    float4 colorTex = tex2D.Sample(smplr, iPS.tex);
    
    //global variables
    float4 specular = { 0.0f, 0.0f, 0.0f, 0.0f };
    float3 globalDiffuse = { 0.0f, 0.0f, 0.0f };
    
    //calculate point lights
    for (uint i = 0; i < numPLights.x; i++)
    {
        //deconstructed normalized vector from light to object
        float3 L_Vec = L_PLPos[i] - iPS.worldPos;
        float1 L_Dist = length(L_Vec);
        float3 L_Dir = L_Vec / L_Dist;

        //diffuse
        float1 attenuation = 1.0f / (L_Dist * L_Dist);
        globalDiffuse += L_PLDiffuse[i] * attenuation * L_PLInt[i].x * M_MatIntensity;
    }
    
    //calculate directional light intensity
    float1 L_DirIntensity = saturate(dot(iPS.W_Normal, normalize(L_DirPos))) * L_DirInt;
    
    //normalize point lights so that they aren't brighter than the directional light (sun)
    globalDiffuse = globalDiffuse / max(0.4f, L_DirIntensity) / 4.0f;
    
    //add directional light
    globalDiffuse += L_DirDiffuse.rgb * L_DirIntensity * M_MatIntensity;
    
    //calculate directional specular
    if (M_SpecIntensity > 0.0f)
    {
        float3 vecRefl = normalize(2 * L_DirIntensity * -iPS.W_Normal - L_DirPos);
        specular = pow(saturate(dot(vecRefl, -iPS.viewDir)), M_SpecPower) * M_SpecIntensity;
    }
    //final color
    return float4((globalDiffuse + M_Ambient.rgb + specular.rgb) * colorTex.rgb, 1.0f);
}