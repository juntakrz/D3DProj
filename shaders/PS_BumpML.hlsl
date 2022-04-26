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
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDir : VECTOR0;
};

Texture2D texColor : register(t0);
Texture2D texNormal : register(t1);
Texture2D texSpecular : register(t2);
SamplerState smplr;

float4 main(VSInput VS) : SV_TARGET
{
    //retrieving texture pixel
    float4 colorTex = texColor.Sample(smplr, VS.tex);
    float4 normalTex = texNormal.Sample(smplr, VS.tex);
    float4 specularTex = texSpecular.Sample(smplr, VS.tex);
    
    colorTex.rgb = pow(colorTex.rgb, 1.0f);

    //global variables
    float4 specular = { 0.0f, 0.0f, 0.0f, 0.0f };
    float4 globalDiffuse = { 0.0f, 0.0f, 0.0f, 0.0f };

    //set normal map range from (0, +1) to (-1, +1)
    normalTex = (normalTex * 2.0f) - 1.0f;

    //calculate bump color
    float3 bump = normalize(normalTex.x * VS.tangent + normalTex.y * VS.binormal + normalTex.z * VS.W_Normal);
    float3 bumpPL = normalize(normalTex.x * VS.tangent + normalTex.y * VS.binormal + normalTex.z * VS.WV_Normal);

    //calculate point lights
    for (uint i = 0; i < numPLights.x; i++)
    {
        //deconstructed normalized vector from light to object
        float3 L_Vec = L_Pos[i] - VS.WV_Pos;
        float1 L_Dist = length(L_Vec);
        float3 L_Dir = L_Vec / L_Dist;

        //diffuse
        float1 L_Curve = 1.0f / L_Dist + (10.0f / (L_Dist * L_Dist));
        float1 L_Att = max(0.0f, dot(bumpPL, L_Dir)) * L_Curve * L_Intensity[i].x;
        globalDiffuse += L_Diffuse[i] * L_Att * M_MatIntensity;
    }

    //calculate directional light intensity
    float1 L_DirIntensity = saturate(dot(bump, normalize(L_DirPos))) * L_DirInt;

    //normalize point lights so that they aren't brighter than the directional light (sun)
    globalDiffuse = globalDiffuse / max(0.4f, L_DirIntensity) / 4.0f;

    //add directional light
    globalDiffuse += L_DirDiffuse * L_DirIntensity * M_MatIntensity;

    //calculate directional specular
    if (M_SpecIntensity > 0.0f)
    {
        float3 vecRefl = normalize(2.0f * L_DirIntensity * -VS.W_Normal - L_DirPos);
        specular = pow(saturate(dot(vecRefl, -VS.viewDir)), M_SpecPower) * M_SpecIntensity;
        
        vecRefl = normalize(2.0f * L_DirIntensity * VS.W_Normal - L_DirPos);
        specular += pow(saturate(dot(vecRefl, -VS.viewDir)), M_SpecPower) * M_SpecIntensity * 1.2f;
        
        specular *= specularTex;
    }
    
    //applying contrast, brightness and saturation curve
    colorTex.rgb = (colorTex.rgb - 0.5f) * 1.5f + 0.5f;
    colorTex.rgb += 0.08f;
 
    float3 luminance = dot(float3(0.299, 0.587, 0.114), colorTex.rgb);
    colorTex.rgb = lerp(luminance, colorTex.rgb, 0.7f);
    
    //final color
    return float4(globalDiffuse + M_Ambient + specular) * colorTex;
}