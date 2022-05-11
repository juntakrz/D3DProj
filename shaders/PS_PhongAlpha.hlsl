cbuffer M_Material : register(b0)
{
    float4 M_Ambient;
    float4 M_F0;
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

struct PSInput
{
    float3 worldPos : POSITION0;
    float3 W_Normal : NORMAL0;
    //float2 tex : TEXCOORD0;
    float3 viewDir : VECTOR0;
};

//Texture2D tex2D : register(t0);
//SamplerState smplr;

//Phong-only Standard shader
float4 main(PSInput iPS) : SV_TARGET
{
    //retrieving texture pixel
    //float4 colorTex = tex2D.Sample(smplr, iPS.tex);
    float4 colorTex = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    //global variables
    float4 specular = { 0.0f, 0.0f, 0.0f, 1.0f };
    float4 globalDiffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    //calculate directional light intensity
    float1 L_DirIntensity = saturate(dot(iPS.W_Normal, normalize(L_DirPos))) * L_DirInt;
    
    //add directional light
    globalDiffuse = L_DirDiffuse * L_DirIntensity * M_MatIntensity;
    
    //calculate directional specular
    if (M_SpecIntensity > 0.0f)
    {
        float3 vecRefl = normalize(2 * L_DirIntensity * -iPS.W_Normal - L_DirPos);
        specular = pow(saturate(dot(vecRefl, -iPS.viewDir)), M_SpecPower) * M_SpecIntensity;
    }
    //final color
    //return float4((globalDiffuse.rgb + M_Ambient.rgb + specular.rgb) * colorTex.rgb, globalDiffuse.a);
    return float4((globalDiffuse.rgb + specular.rgb) * colorTex.rgb, globalDiffuse.a);

}