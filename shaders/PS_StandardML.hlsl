cbuffer M_Material              //slot 0
{
    float4 M_Ambient;
    float1 M_MatIntensity;
    float1 M_SpecIntensity;
    float1 M_SpecPower;
};

cbuffer L_DirLight              //slot 1
{
    float3 L_DirPos;            //XMFLOAT3
    float1 L_DirInt;            //float
    float4 L_DirDiffuse;        //XMFLOAT4
};

cbuffer L_PointLight            //slot 2
{
    float3 L_Pos[4];
    float4 L_Diffuse[4];
    float2 L_Intensity[4];
    uint4  numPLights;
};

struct VSInput
{
    float3 WV_Pos : POSITION;
    float3 W_Normal : NORMAL0;
    float3 WV_Normal : NORMAL1;
    float2 tex : TEXCOORD;
    float3 viewDir : VECTOR0;
};

Texture2D tex2D : register(t0);
SamplerState smplr;

float4 main(VSInput VS) : SV_TARGET
{
    //retrieving texture pixel
    float4 colorTex = tex2D.Sample(smplr, VS.tex);
    
    //global variables
    float4 specular = { 0.0f, 0.0f, 0.0f, 0.0f };
    float3 globalDiffuse = { 0.0f, 0.0f, 0.0f };
    //float3 globalSpecular = { 0.0f, 0.0f, 0.0f };
    
    //calculate point lights
    for (uint i = 0; i < numPLights.x; i++)
    {
    //deconstructed normalized vector from light to object
        float3 L_Vec = L_Pos[i] - VS.WV_Pos;
        float1 L_Dist = length(L_Vec);
        float3 L_Dir = L_Vec / L_Dist;
    
    //diffuse
        float1 L_Curve = 1.0f / L_Dist + (10.0f / (L_Dist * L_Dist));
        float1 L_Att = max(0.0f, dot(VS.WV_Normal, L_Dir)) * L_Curve * L_Intensity[i].x;
        globalDiffuse += L_Diffuse[i].rgb * L_Att * M_MatIntensity;
        
    //specular
     
    }
    
    //calculate directional light intensity
    float1 L_DirIntensity = saturate(dot(VS.W_Normal, normalize(L_DirPos))) * L_DirInt;
    
    //normalize point lights so that they aren't brighter than the directional light (sun)
    globalDiffuse = globalDiffuse / max(0.4f, L_DirIntensity) / 4.0f;
    
    //add directional light
    globalDiffuse += L_DirDiffuse.rgb * L_DirIntensity * M_MatIntensity;
    
    //calculate directional specular
    if (M_SpecIntensity > 0.0f)
    {
        float3 vecRefl = normalize(2 * L_DirIntensity * VS.W_Normal - L_DirPos);
        specular = pow(saturate(dot(vecRefl, -VS.viewDir)), M_SpecPower) * M_SpecIntensity;
    }
    //final color
    return float4((globalDiffuse + M_Ambient.rgb + specular.rgb) * colorTex.rgb, 1.0f);
}